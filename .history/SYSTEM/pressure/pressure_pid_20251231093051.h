/**
 * @file    pressure_pid.h
 * @brief   增量式PI压力控制器 + 自整定功能
 * @author  AI Assistant
 * @date    2024-12-29
 * @version 1.0
 * 
 * @note    专为锅炉压力控制设计
 *          - 增量式PI：无积分饱和，适合大惯性系统
 *          - 自整定：继电器反馈法自动计算参数
 */

#ifndef __PRESSURE_PID_H
#define __PRESSURE_PID_H

#include "main.h"

/*==============================================================================
 * 宏定义
 *============================================================================*/

/* PI参数限幅 */
#define PI_KP_MIN           0.1f
#define PI_KP_MAX           5.0f
#define PI_KI_MIN           0.01f
#define PI_KI_MAX           0.5f

/* 默认参数 */
#define PI_KP_DEFAULT       1.0f
#define PI_KI_DEFAULT       0.05f
#define PI_DEADBAND_DEFAULT 0.02f   /* 0.02 MPa */

/* 自整定参数 */
#define TUNE_RELAY_HIGH     80.0f   /* 高功率 80% */
#define TUNE_RELAY_LOW      30.0f   /* 低功率 30% */
#define TUNE_HYSTERESIS     0.01f   /* 迟滞 0.01 MPa */
#define TUNE_CYCLES         3       /* 需要3个振荡周期 */
#define TUNE_TIMEOUT        300     /* 5分钟超时 */

/*==============================================================================
 * 类型定义
 *============================================================================*/

/**
 * @brief PI控制器结构体
 */
typedef struct {
    /* PI参数 */
    float kp;               /**< 比例系数 */
    float ki;               /**< 积分系数 */
    
    /* 状态变量 */
    float lastError;        /**< 上一次误差 e(k-1) */
    float output;           /**< 当前输出 u(k) */
    
    /* 输出限幅 */
    float outMin;           /**< 最小输出 (%) */
    float outMax;           /**< 最大输出 (%) */
    
    /* 死区设置 */
    float deadband;         /**< 死区范围 (MPa) */
    
    /* 积分分离阈值 */
    float integralSepThreshold;  /**< 误差超过此值时禁用积分 (MPa) */
    
} PiController;

/**
 * @brief 自整定状态枚举
 */
typedef enum {
    TUNE_IDLE = 0,          /**< 空闲 */
    TUNE_WAIT_STABLE,       /**< 等待压力稳定 */
    TUNE_RELAY_HIGH_STATE,  /**< 继电器高输出 */
    TUNE_RELAY_LOW_STATE,   /**< 继电器低输出 */
    TUNE_CALC,              /**< 计算参数 */
    TUNE_DONE,              /**< 完成 */
    TUNE_FAILED             /**< 失败 */
} TuneState;

/**
 * @brief 自整定控制器结构体
 */
typedef struct {
    TuneState state;        /**< 当前状态 */
    
    /* 继电器参数 */
    float relayHigh;        /**< 高输出 (%) */
    float relayLow;         /**< 低输出 (%) */
    float hysteresis;       /**< 迟滞带 (MPa) */
    
    /* 测量数据 */
    float peakHigh;         /**< 压力峰值 */
    float peakLow;          /**< 压力谷值 */
    float setpoint;         /**< 设定压力 */
    uint32_t periodStart;   /**< 周期开始时间 */
    uint32_t periodSum;     /**< 周期累加 */
    uint8_t cycleCount;     /**< 振荡次数 */
    
    /* 计算结果 */
    float tuPeriod;         /**< 振荡周期 (秒) */
    float amplitude;        /**< 振幅 (MPa) */
    float kuGain;           /**< 临界增益 */
    float calcKp;           /**< 计算的Kp */
    float calcKi;           /**< 计算的Ki */
    
    /* 配置 */
    uint8_t requiredCycles; /**< 需要的振荡次数 */
    uint32_t timeout;       /**< 超时时间 (秒) */
    uint32_t tuneTime;      /**< 已运行时间 */
    uint32_t stableCount;   /**< 稳定计数 */
    
} AutoTuner;

/**
 * @brief 预设参数结构体
 */
typedef struct {
    float kp;
    float ki;
    float deadband;
} PiPreset;

/*==============================================================================
 * 预设参数常量
 *============================================================================*/

extern const PiPreset PI_PRESET_STANDARD;   /**< 标准模式 */
extern const PiPreset PI_PRESET_FAST;       /**< 快速响应 */
extern const PiPreset PI_PRESET_STABLE;     /**< 稳定优先 */

/*==============================================================================
 * PI控制器函数声明
 *============================================================================*/

/**
 * @brief  初始化PI控制器
 * @param  pi      PI控制器指针
 * @param  kp      比例系数
 * @param  ki      积分系数
 * @param  outMin  最小输出 (%)
 * @param  outMax  最大输出 (%)
 */
void Pi_Init(PiController *pi, float kp, float ki, float outMin, float outMax);

/**
 * @brief  PI计算 (每秒调用一次)
 * @param  pi          PI控制器指针
 * @param  setpoint    设定压力 (MPa)
 * @param  measurement 实际压力 (MPa)
 * @return 输出功率百分比
 */
float Pi_Compute(PiController *pi, float setpoint, float measurement);

/**
 * @brief  复位PI控制器状态
 * @param  pi  PI控制器指针
 */
void Pi_Reset(PiController *pi);

/**
 * @brief  运行时修改PI参数
 * @param  pi  PI控制器指针
 * @param  kp  新的比例系数
 * @param  ki  新的积分系数
 */
void Pi_SetParams(PiController *pi, float kp, float ki);

/**
 * @brief  设置死区
 * @param  pi       PI控制器指针
 * @param  deadband 死区范围 (MPa)
 */
void Pi_SetDeadband(PiController *pi, float deadband);

/*==============================================================================
 * 自整定函数声明
 *============================================================================*/

/**
 * @brief  启动自整定
 * @param  tuner    自整定器指针
 * @param  setpoint 目标压力 (MPa)
 */
void AutoTune_Start(AutoTuner *tuner, float setpoint);

/**
 * @brief  自整定步进 (每秒调用)
 * @param  tuner       自整定器指针
 * @param  measurement 实际压力 (MPa)
 * @param  output      输出功率指针
 * @return 1=进行中, 0=完成, -1=失败
 */
int8_t AutoTune_Step(AutoTuner *tuner, float measurement, float *output);

/**
 * @brief  获取自整定结果
 * @param  tuner  自整定器指针
 * @param  kp     Kp输出指针
 * @param  ki     Ki输出指针
 */
void AutoTune_GetResult(AutoTuner *tuner, float *kp, float *ki);

/**
 * @brief  获取自整定状态
 * @param  tuner  自整定器指针
 * @return 当前状态
 */
TuneState AutoTune_GetState(AutoTuner *tuner);

/**
 * @brief  取消自整定
 * @param  tuner  自整定器指针
 */
void AutoTune_Cancel(AutoTuner *tuner);

#endif /* __PRESSURE_PID_H */

