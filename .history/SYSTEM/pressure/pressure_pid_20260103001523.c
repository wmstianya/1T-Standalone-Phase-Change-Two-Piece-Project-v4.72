/**
 * @file    pressure_pid.c
 * @brief   增量式PI压力控制器 + 自整定功能实现
 * @author  AI Assistant
 * @date    2024-12-29
 * @version 1.0
 */

#include "pressure_pid.h"

/*==============================================================================
 * 预设参数常量
 * 注意: Keil C89 不支持 C99 指定初始化器，使用传统初始化
 * 结构体顺序: { kp, ki, deadband }
 *============================================================================*/

const PiPreset PI_PRESET_STANDARD = { 1.0f, 0.05f, 0.02f };  /* 标准模式 */
const PiPreset PI_PRESET_FAST     = { 1.5f, 0.08f, 0.01f };  /* 快速响应 */
const PiPreset PI_PRESET_STABLE   = { 0.6f, 0.03f, 0.03f };  /* 稳定优先 */

/*==============================================================================
 * PI控制器实现
 *============================================================================*/

/**
 * @brief  初始化PI控制器
 */
void Pi_Init(PiController *pi, float kp, float ki, float outMin, float outMax)
{
    if (pi == NULL)
    {
        return;
    }
    
    /* 设置参数并限幅 */
    pi->kp = kp;
    if (pi->kp > PI_KP_MAX) pi->kp = PI_KP_MAX;
    if (pi->kp < PI_KP_MIN) pi->kp = PI_KP_MIN;
    
    pi->ki = ki;
    if (pi->ki > PI_KI_MAX) pi->ki = PI_KI_MAX;
    if (pi->ki < PI_KI_MIN) pi->ki = PI_KI_MIN;
    
    /* 初始化状态 */
    pi->lastError = 0.0f;
    pi->output = outMin;  /* 从最小功率开始 */
    
    /* 设置输出范围 */
    pi->outMin = outMin;
    pi->outMax = outMax;
    
    /* 默认死区和积分分离阈值 */
    pi->deadband = PI_DEADBAND_DEFAULT;
    pi->integralSepThreshold = 0.1f;  /* 误差>0.1MPa时禁用积分 */
}

/**
 * @brief  PI计算 (每秒调用一次)
 * @param  pi          PI控制器指针
 * @param  setpoint    设定压力 (MPa)
 * @param  measurement 实际压力 (MPa)
 * @return 输出功率百分比
 */
float Pi_Compute(PiController *pi, float setpoint, float measurement)
{
    float rawError;      /* 原始误差 */
    float error;         /* 用于计算的误差 */
    float deltaError;
    float pTerm, iTerm;
    float deltaOutput;
    
    if (pi == NULL)
    {
        return 0.0f;
    }
    
    /* 计算原始误差: 正值表示需要升压 */
    rawError = setpoint - measurement;
    error = rawError;
    
    /* 死区处理: 误差在死区内不调节 (使用闭区间) */
    /* Bug1+Bug8修复: 死区内error置0用于计算，但lastError保存原始值 */
    if (rawError >= -pi->deadband && rawError <= pi->deadband)
    {
        error = 0.0f;
    }
    
    /* 计算误差变化量 - 使用原始误差计算deltaError避免死区突变 */
    deltaError = rawError - pi->lastError;
    
    /* 比例项: 响应误差变化 */
    pTerm = pi->kp * deltaError;
    
    /* 积分项: 消除稳态误差 */
    /* 积分分离: 误差过大时禁用积分，防止超调 */
    if (error > pi->integralSepThreshold || error < -pi->integralSepThreshold)
    {
        iTerm = 0.0f;
    }
    else
    {
        iTerm = pi->ki * error;
    }
    
    /* 增量式输出 */
    deltaOutput = pTerm + iTerm;
    
    /* 累加到输出 */
    pi->output += deltaOutput;
    
    /* 输出限幅 (抗积分饱和) */
    if (pi->output > pi->outMax)
    {
        pi->output = pi->outMax;
    }
    else if (pi->output < pi->outMin)
    {
        pi->output = pi->outMin;
    }
    
    /* 保存原始误差 (不是处理后的error) */
    pi->lastError = rawError;
    
    return pi->output;
}

/**
 * @brief  复位PI控制器状态
 */
void Pi_Reset(PiController *pi)
{
    if (pi == NULL)
    {
        return;
    }
    
    pi->lastError = 0.0f;
    pi->output = pi->outMin;
}

/**
 * @brief  运行时修改PI参数
 * @note   Bug5修复: 参数修改后重置lastError，避免首次计算异常
 */
void Pi_SetParams(PiController *pi, float kp, float ki)
{
    if (pi == NULL)
    {
        return;
    }
    
    pi->kp = kp;
    if (pi->kp > PI_KP_MAX) pi->kp = PI_KP_MAX;
    if (pi->kp < PI_KP_MIN) pi->kp = PI_KP_MIN;
    
    pi->ki = ki;
    if (pi->ki > PI_KI_MAX) pi->ki = PI_KI_MAX;
    if (pi->ki < PI_KI_MIN) pi->ki = PI_KI_MIN;
    
    /* Bug5修复: 参数修改后重置历史误差 */
    pi->lastError = 0.0f;
}

/**
 * @brief  设置死区
 */
void Pi_SetDeadband(PiController *pi, float deadband)
{
    if (pi == NULL)
    {
        return;
    }
    
    pi->deadband = deadband;
    if (pi->deadband < 0.0f)
    {
        pi->deadband = 0.0f;
    }
    if (pi->deadband > 0.1f)
    {
        pi->deadband = 0.1f;  /* 最大0.1 MPa */
    }
}

/*==============================================================================
 * 自整定实现
 *============================================================================*/

/**
 * @brief  启动自整定
 */
void AutoTune_Start(AutoTuner *tuner, float setpoint)
{
    if (tuner == NULL)
    {
        return;
    }
    
    tuner->state = TUNE_WAIT_STABLE;
    tuner->setpoint = setpoint;
    
    /* 继电器参数 */
    tuner->relayHigh = TUNE_RELAY_HIGH;
    tuner->relayLow = TUNE_RELAY_LOW;
    tuner->hysteresis = TUNE_HYSTERESIS;
    
    /* 重置测量数据 */
    tuner->cycleCount = 0;
    tuner->periodSum = 0;
    tuner->peakHigh = setpoint;
    tuner->peakLow = setpoint;
    tuner->periodStart = 0;
    tuner->amplitudeSum = 0.0f;     /* Bug2修复: 初始化振幅累加 */
    tuner->halfCycleFlag = 0;       /* Bug3修复: 初始化半周期标志 */
    
    /* 配置 */
    tuner->requiredCycles = TUNE_CYCLES;
    tuner->timeout = TUNE_TIMEOUT;
    tuner->tuneTime = 0;
    tuner->stableCount = 0;
    
    /* 清空结果 */
    tuner->tuPeriod = 0.0f;
    tuner->amplitude = 0.0f;
    tuner->kuGain = 0.0f;
    tuner->calcKp = PI_KP_DEFAULT;
    tuner->calcKi = PI_KI_DEFAULT;
}

/**
 * @brief  自整定步进 (每秒调用)
 * @return 1=进行中, 0=完成, -1=失败
 */
int8_t AutoTune_Step(AutoTuner *tuner, float measurement, float *output)
{
    float error;
    float relayAmplitude;
    
    if (tuner == NULL || output == NULL)
    {
        return -1;
    }
    
    tuner->tuneTime++;
    
    /* 超时检测 */
    if (tuner->tuneTime > tuner->timeout)
    {
        tuner->state = TUNE_FAILED;
        *output = tuner->relayLow;  /* 安全功率 */
        return -1;
    }
    
    error = measurement - tuner->setpoint;
    
    switch (tuner->state)
    {
        case TUNE_WAIT_STABLE:
            /* 等待压力接近设定值 */
            *output = 50.0f;  /* 中等功率 */
            
            if (error > -0.05f && error < 0.05f)
            {
                tuner->stableCount++;
                if (tuner->stableCount > 10)  /* 稳定10秒 */
                {
                    tuner->state = TUNE_RELAY_HIGH_STATE;
                    tuner->periodStart = tuner->tuneTime;
                    tuner->peakHigh = tuner->setpoint;
                    tuner->peakLow = tuner->setpoint;
                }
            }
            else
            {
                tuner->stableCount = 0;
            }
            break;
            
        case TUNE_RELAY_HIGH_STATE:
            /* 高输出，等待压力超过设定值 */
            *output = tuner->relayHigh;
            
            /* 记录峰值 */
            if (measurement > tuner->peakHigh)
            {
                tuner->peakHigh = measurement;
            }
            
            /* 压力超过设定值+迟滞，切换到低输出 */
            if (error > tuner->hysteresis)
            {
                tuner->state = TUNE_RELAY_LOW_STATE;
            }
            break;
            
        case TUNE_RELAY_LOW_STATE:
            /* 低输出，等待压力低于设定值 */
            *output = tuner->relayLow;
            
            /* 记录谷值 */
            if (measurement < tuner->peakLow)
            {
                tuner->peakLow = measurement;
            }
            
            /* 压力低于设定值-迟滞，切换到高输出 */
            if (error < -tuner->hysteresis)
            {
                /* 完成一个周期 */
                tuner->cycleCount++;
                tuner->periodSum += (tuner->tuneTime - tuner->periodStart);
                tuner->periodStart = tuner->tuneTime;
                
                if (tuner->cycleCount >= tuner->requiredCycles)
                {
                    tuner->state = TUNE_CALC;
                }
                else
                {
                    /* 继续下一个周期，重置峰谷值 */
                    tuner->peakHigh = tuner->setpoint;
                    tuner->peakLow = tuner->setpoint;
                    tuner->state = TUNE_RELAY_HIGH_STATE;
                }
            }
            break;
            
        case TUNE_CALC:
            /* 计算PI参数 */
            relayAmplitude = (tuner->relayHigh - tuner->relayLow) / 2.0f;
            
            /* 计算振荡周期 (平均值) */
            tuner->tuPeriod = (float)tuner->periodSum / (float)tuner->cycleCount;
            
            /* 计算振幅 */
            tuner->amplitude = (tuner->peakHigh - tuner->peakLow) / 2.0f;
            
            /* 防止除零 */
            if (tuner->amplitude < 0.005f)
            {
                tuner->amplitude = 0.005f;
            }
            
            /* 临界增益 Ku = 4d / (π × a) */
            tuner->kuGain = (4.0f * relayAmplitude) / (3.14159f * tuner->amplitude);
            
            /* Ziegler-Nichols PI参数 */
            tuner->calcKp = 0.45f * tuner->kuGain;
            tuner->calcKi = 0.54f * tuner->kuGain / tuner->tuPeriod;
            
            /* 限制参数范围 */
            if (tuner->calcKp > PI_KP_MAX) tuner->calcKp = PI_KP_MAX;
            if (tuner->calcKp < PI_KP_MIN) tuner->calcKp = PI_KP_MIN;
            if (tuner->calcKi > PI_KI_MAX) tuner->calcKi = PI_KI_MAX;
            if (tuner->calcKi < PI_KI_MIN) tuner->calcKi = PI_KI_MIN;
            
            tuner->state = TUNE_DONE;
            *output = 50.0f;  /* 恢复中等功率 */
            break;
            
        case TUNE_DONE:
            *output = 50.0f;  /* 中等功率 */
            return 0;  /* 完成 */
            
        case TUNE_FAILED:
            *output = tuner->relayLow;  /* 安全功率 */
            return -1;  /* 失败 */
            
        case TUNE_IDLE:
        default:
            *output = 50.0f;
            return -1;
    }
    
    return 1;  /* 进行中 */
}

/**
 * @brief  获取自整定结果
 */
void AutoTune_GetResult(AutoTuner *tuner, float *kp, float *ki)
{
    if (tuner == NULL || kp == NULL || ki == NULL)
    {
        return;
    }
    
    *kp = tuner->calcKp;
    *ki = tuner->calcKi;
}

/**
 * @brief  获取自整定状态
 */
TuneState AutoTune_GetState(AutoTuner *tuner)
{
    if (tuner == NULL)
    {
        return TUNE_IDLE;
    }
    
    return tuner->state;
}

/**
 * @brief  取消自整定
 */
void AutoTune_Cancel(AutoTuner *tuner)
{
    if (tuner == NULL)
    {
        return;
    }
    
    tuner->state = TUNE_IDLE;
}

