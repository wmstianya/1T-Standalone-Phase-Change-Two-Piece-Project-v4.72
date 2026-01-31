# 变更提案: 压力控制算法优化 - 增量式PI

**Date**: 2024-12-29  
**Author**: AI Assistant  
**Status**: ✅ Approved - 已实施完成

---

## 1. 变更概述

| 项目 | 内容 |
|------|------|
| **目标** | 将压力控制从步进式开关控制升级为增量式PI控制 |
| **影响范围** | `SYSTEM/pressure/pressure_ctrl.c` |
| **预计工作量** | 1-2天 |
| **风险等级** | 中 (核心控制逻辑变更) |

---

## 2. 为什么选择PI而非PID？

| 因素 | 说明 |
|------|------|
| **系统惯性大** | 锅炉热容量大，压力响应时间常数10-60秒 |
| **自阻尼特性** | 蒸汽系统本身具有自平衡能力 |
| **传感器噪声** | 压力传感器有波动，D项会放大噪声 |
| **安全优先** | PI控制更平稳，避免功率剧烈波动 |

**结论**: PI控制 = 稳定 + 够用，D项 = 增加复杂度 + 风险

---

## 3. 增量式PI算法

### 3.1 数学公式

```
位置式PI (不推荐):
u(k) = Kp × e(k) + Ki × Σe(i)
       ↓
问题: 积分项累积，易饱和，需要初始值

增量式PI (推荐):
Δu(k) = Kp × [e(k) - e(k-1)] + Ki × e(k)
u(k) = u(k-1) + Δu(k)
       ↓
优势: 无积分累积，无需初始值，天然抗饱和
```

### 3.2 控制框图

```
              ┌─────────────┐
设定压力 ──┬─►│  误差计算   │──► e(k)
           │  └─────────────┘      │
           │         ▲             ▼
           │         │      ┌──────────────┐
           │    实际压力    │  增量式PI    │
           │         │      │  Δu = Kp×Δe  │
           │         │      │     + Ki×e   │
           │         │      └──────────────┘
           │         │             │
           │         │             ▼ Δu(k)
           │         │      ┌──────────────┐
           │         │      │  积分限幅    │
           │         │      │  u = u + Δu  │
           │         │      │  限制在      │
           │         │      │  [min, max]  │
           │         │      └──────────────┘
           │         │             │
           │         │             ▼ u(k)
           │  ┌──────┴──────┐     │
           │  │   锅炉系统   │◄────┘ 功率%
           │  │  (大惯性)   │
           │  └─────────────┘
           │         │
           └─────────┘ 压力反馈
```

---

## 4. 代码设计

### 4.1 新增文件

```
SYSTEM/pressure/
├── pressure_ctrl.h      # 现有 (需修改)
├── pressure_ctrl.c      # 现有 (需修改)
├── pressure_pid.h       # 新增: PI控制器定义
└── pressure_pid.c       # 新增: PI控制器实现
```

### 4.2 数据结构

```c
/**
 * @file    pressure_pid.h
 * @brief   增量式PI压力控制器
 */

#ifndef __PRESSURE_PID_H
#define __PRESSURE_PID_H

#include "main.h"

/* PI控制器状态 */
typedef struct {
    /* PI参数 */
    float kp;               /* 比例系数 (典型值: 0.5-2.0) */
    float ki;               /* 积分系数 (典型值: 0.01-0.1) */
    
    /* 状态变量 */
    float lastError;        /* 上一次误差 e(k-1) */
    float output;           /* 当前输出 u(k) */
    
    /* 输出限幅 */
    float outMin;           /* 最小输出 (如: 20%) */
    float outMax;           /* 最大输出 (如: 100%) */
    
    /* 死区设置 */
    float deadband;         /* 死区范围 (如: ±0.02 MPa) */
    
    /* 积分分离阈值 */
    float integralSepThreshold;  /* 误差超过此值时禁用积分 */
    
} PiController;

/* 预设参数配置 */
typedef struct {
    float kp;
    float ki;
    float deadband;
} PiPreset;

/* API函数 */
void Pi_Init(PiController *pi, float kp, float ki, float outMin, float outMax);
float Pi_Compute(PiController *pi, float setpoint, float measurement);
void Pi_Reset(PiController *pi);
void Pi_SetParams(PiController *pi, float kp, float ki);
void Pi_SetDeadband(PiController *pi, float deadband);

/* 预设参数 */
extern const PiPreset PI_PRESET_STANDARD;   /* 标准锅炉 */
extern const PiPreset PI_PRESET_FAST;       /* 快速响应 */
extern const PiPreset PI_PRESET_STABLE;     /* 稳定优先 */

#endif /* __PRESSURE_PID_H */
```

### 4.3 核心算法实现

```c
/**
 * @file    pressure_pid.c
 * @brief   增量式PI压力控制器实现
 */

#include "pressure_pid.h"

/* 预设参数 */
const PiPreset PI_PRESET_STANDARD = { .kp = 1.0f,  .ki = 0.05f, .deadband = 0.02f };
const PiPreset PI_PRESET_FAST     = { .kp = 1.5f,  .ki = 0.08f, .deadband = 0.01f };
const PiPreset PI_PRESET_STABLE   = { .kp = 0.6f,  .ki = 0.03f, .deadband = 0.03f };

/**
 * @brief  初始化PI控制器
 */
void Pi_Init(PiController *pi, float kp, float ki, float outMin, float outMax)
{
    pi->kp = kp;
    pi->ki = ki;
    pi->lastError = 0.0f;
    pi->output = outMin;  /* 从最小功率开始 */
    pi->outMin = outMin;
    pi->outMax = outMax;
    pi->deadband = 0.02f;  /* 默认死区 0.02 MPa */
    pi->integralSepThreshold = 0.1f;  /* 误差>0.1MPa时禁用积分 */
}

/**
 * @brief  PI计算 (每秒调用一次)
 * @param  pi          PI控制器指针
 * @param  setpoint    设定压力 (MPa, 如 0.7)
 * @param  measurement 实际压力 (MPa, 如 0.65)
 * @return 输出功率百分比 (20-100)
 */
float Pi_Compute(PiController *pi, float setpoint, float measurement)
{
    float error;
    float deltaError;
    float pTerm, iTerm;
    float deltaOutput;
    
    /* 计算误差: 正值表示需要升压 */
    error = setpoint - measurement;
    
    /* 死区处理: 误差在死区内不调节 */
    if (error > -pi->deadband && error < pi->deadband)
    {
        error = 0.0f;
    }
    
    /* 计算误差变化量 */
    deltaError = error - pi->lastError;
    
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
    
    /* 保存历史误差 */
    pi->lastError = error;
    
    return pi->output;
}

/**
 * @brief  复位PI控制器状态
 */
void Pi_Reset(PiController *pi)
{
    pi->lastError = 0.0f;
    pi->output = pi->outMin;
}

/**
 * @brief  运行时修改PI参数
 */
void Pi_SetParams(PiController *pi, float kp, float ki)
{
    pi->kp = kp;
    pi->ki = ki;
}

/**
 * @brief  设置死区
 */
void Pi_SetDeadband(PiController *pi, float deadband)
{
    pi->deadband = deadband;
}
```

### 4.4 集成到现有控制函数

```c
/* pressure_ctrl.c 修改 */

#include "pressure_pid.h"

/* PI控制器实例 */
static PiController pressurePi;
static uint8_t piInitialized = 0;

/**
 * @brief  单机压力平衡控制 (PI版本)
 */
uint8 System_Pressure_Balance_Function(void)
{
    float setpoint;         /* 设定压力 MPa */
    float measurement;      /* 实际压力 MPa */
    float outputPercent;    /* 输出功率 % */
    uint8_t airMin, airMax;
    
    /* 首次调用初始化PI */
    if (!piInitialized)
    {
        airMin = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);
        airMax = Sys_Admin.Max_Work_Power;
        if (airMax > 100) airMax = 100;
        if (airMax < 20) airMax = 25;
        
        Pi_Init(&pressurePi, 
                PI_PRESET_STANDARD.kp, 
                PI_PRESET_STANDARD.ki,
                (float)airMin, 
                (float)airMax);
        Pi_SetDeadband(&pressurePi, PI_PRESET_STANDARD.deadband);
        piInitialized = 1;
    }
    
    /* 获取设定值 (转换为MPa) */
    setpoint = (float)sys_config_data.zhuan_huan_temperture_value / 100.0f;
    
    /* 获取实际压力 (转换为MPa) */
    if (Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
    {
        /* 相变设备使用高压侧压力 + 压差补偿 */
        measurement = (float)Temperature_Data.Inside_High_Pressure / 100.0f;
        setpoint += 0.65f;  /* 压差补偿 0.65 MPa */
    }
    else
    {
        measurement = (float)Temperature_Data.Pressure_Value / 100.0f;
    }
    
    /* 每秒计算一次PI */
    if (sys_flag.Power_1_Sec)
    {
        sys_flag.Power_1_Sec = 0;
        outputPercent = Pi_Compute(&pressurePi, setpoint, measurement);
        now_percent = (uint8_t)outputPercent;
    }
    
    /* 输出PWM */
    PWM_Adjust(now_percent);
    
    /* 超压保护 (保留原逻辑) */
    if (Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
    {
        sys_data.Data_12H |= Set_Bit_4;
        Abnormal_Events.target_complete_event = 1;
    }
    
    return OK;
}
```

---

## 5. 自整定功能

### 5.1 继电器反馈自整定原理

```
┌────────────────────────────────────────────────────────────┐
│  自整定过程 (用户触发，自动完成)                             │
│                                                            │
│  功率输出:                                                  │
│        Max ──┐    ┌────┐    ┌────┐                         │
│              │    │    │    │    │                         │
│        Min ──┴────┴────┴────┴────┴──                       │
│                                                            │
│  压力响应:                                                  │
│              ╱╲      ╱╲      ╱╲                            │
│        SP ──────────────────────── 设定值                   │
│              ╲╱      ╲╱      ╲╱                            │
│              ◄──Tu──►                                      │
│              ◄─a─► 振幅                                    │
│                                                            │
│  测量: 振荡周期 Tu, 振幅 a, 继电器幅度 d                      │
│  计算: 临界增益 Ku = 4d / (π × a)                           │
│  PI参数 (Ziegler-Nichols):                                 │
│         Kp = 0.45 × Ku                                     │
│         Ki = 0.54 × Ku / Tu                                │
└────────────────────────────────────────────────────────────┘
```

### 5.2 自整定数据结构

```c
/* 自整定状态机 */
typedef enum {
    TUNE_IDLE = 0,      /* 空闲 */
    TUNE_WAIT_STABLE,   /* 等待压力稳定 */
    TUNE_RELAY_HIGH,    /* 继电器高输出 */
    TUNE_RELAY_LOW,     /* 继电器低输出 */
    TUNE_CALC,          /* 计算参数 */
    TUNE_DONE,          /* 完成 */
    TUNE_FAILED         /* 失败 */
} TuneState;

/* 自整定控制器 */
typedef struct {
    TuneState state;        /* 当前状态 */
    
    /* 继电器参数 */
    float relayHigh;        /* 高输出 (如 80%) */
    float relayLow;         /* 低输出 (如 30%) */
    float hysteresis;       /* 迟滞带 (如 0.01 MPa) */
    
    /* 测量数据 */
    float peakHigh;         /* 压力峰值 */
    float peakLow;          /* 压力谷值 */
    uint32_t periodStart;   /* 周期开始时间 */
    uint32_t periodSum;     /* 周期累加 */
    uint8_t cycleCount;     /* 振荡次数 */
    
    /* 计算结果 */
    float Tu;               /* 振荡周期 (秒) */
    float amplitude;        /* 振幅 */
    float Ku;               /* 临界增益 */
    float calcKp;           /* 计算的Kp */
    float calcKi;           /* 计算的Ki */
    
    /* 配置 */
    uint8_t requiredCycles; /* 需要的振荡次数 (默认3) */
    uint32_t timeout;       /* 超时时间 (秒) */
    
} AutoTuner;
```

### 5.3 自整定算法实现

```c
/**
 * @brief  启动自整定
 * @param  tuner    自整定器指针
 * @param  setpoint 目标压力 MPa
 */
void AutoTune_Start(AutoTuner *tuner, float setpoint)
{
    tuner->state = TUNE_WAIT_STABLE;
    tuner->relayHigh = 80.0f;       /* 高功率 80% */
    tuner->relayLow = 30.0f;        /* 低功率 30% */
    tuner->hysteresis = 0.01f;      /* 迟滞 0.01 MPa */
    tuner->cycleCount = 0;
    tuner->requiredCycles = 3;
    tuner->periodSum = 0;
    tuner->peakHigh = setpoint;
    tuner->peakLow = setpoint;
    tuner->timeout = 300;           /* 5分钟超时 */
}

/**
 * @brief  自整定步进 (每秒调用)
 * @param  tuner      自整定器指针
 * @param  setpoint   设定压力 MPa
 * @param  measurement 实际压力 MPa
 * @param  output     输出功率指针
 * @return 1=进行中, 0=完成, -1=失败
 */
int8_t AutoTune_Step(AutoTuner *tuner, float setpoint, float measurement, float *output)
{
    float error = measurement - setpoint;
    static uint32_t stableCount = 0;
    static uint32_t tuneTime = 0;
    
    tuneTime++;
    
    /* 超时检测 */
    if (tuneTime > tuner->timeout)
    {
        tuner->state = TUNE_FAILED;
        return -1;
    }
    
    switch (tuner->state)
    {
        case TUNE_WAIT_STABLE:
            /* 等待压力接近设定值 */
            *output = 50.0f;  /* 中等功率 */
            if (error > -0.05f && error < 0.05f)
            {
                stableCount++;
                if (stableCount > 10)  /* 稳定10秒 */
                {
                    tuner->state = TUNE_RELAY_HIGH;
                    tuner->periodStart = tuneTime;
                }
            }
            else
            {
                stableCount = 0;
            }
            break;
            
        case TUNE_RELAY_HIGH:
            /* 高输出，等待压力超过设定值 */
            *output = tuner->relayHigh;
            
            /* 记录峰值 */
            if (measurement > tuner->peakHigh)
                tuner->peakHigh = measurement;
            
            /* 压力超过设定值+迟滞，切换到低输出 */
            if (error > tuner->hysteresis)
            {
                tuner->state = TUNE_RELAY_LOW;
            }
            break;
            
        case TUNE_RELAY_LOW:
            /* 低输出，等待压力低于设定值 */
            *output = tuner->relayLow;
            
            /* 记录谷值 */
            if (measurement < tuner->peakLow)
                tuner->peakLow = measurement;
            
            /* 压力低于设定值-迟滞，切换到高输出 */
            if (error < -tuner->hysteresis)
            {
                /* 完成一个周期 */
                tuner->cycleCount++;
                tuner->periodSum += (tuneTime - tuner->periodStart);
                tuner->periodStart = tuneTime;
                
                /* 重置峰谷值 */
                tuner->peakHigh = setpoint;
                tuner->peakLow = setpoint;
                
                if (tuner->cycleCount >= tuner->requiredCycles)
                {
                    tuner->state = TUNE_CALC;
                }
                else
                {
                    tuner->state = TUNE_RELAY_HIGH;
                }
            }
            break;
            
        case TUNE_CALC:
            /* 计算PI参数 */
            {
                float d = (tuner->relayHigh - tuner->relayLow) / 2.0f;
                
                tuner->Tu = (float)tuner->periodSum / (float)tuner->cycleCount;
                tuner->amplitude = (tuner->peakHigh - tuner->peakLow) / 2.0f;
                
                /* 防止除零 */
                if (tuner->amplitude < 0.005f)
                    tuner->amplitude = 0.005f;
                
                /* Ku = 4d / (π × a) */
                tuner->Ku = (4.0f * d) / (3.14159f * tuner->amplitude);
                
                /* Ziegler-Nichols PI参数 */
                tuner->calcKp = 0.45f * tuner->Ku;
                tuner->calcKi = 0.54f * tuner->Ku / tuner->Tu;
                
                /* 限制参数范围 */
                if (tuner->calcKp > 5.0f) tuner->calcKp = 5.0f;
                if (tuner->calcKp < 0.1f) tuner->calcKp = 0.1f;
                if (tuner->calcKi > 0.5f) tuner->calcKi = 0.5f;
                if (tuner->calcKi < 0.01f) tuner->calcKi = 0.01f;
                
                tuner->state = TUNE_DONE;
            }
            break;
            
        case TUNE_DONE:
            *output = 50.0f;  /* 恢复中等功率 */
            return 0;  /* 完成 */
            
        case TUNE_FAILED:
            *output = 30.0f;  /* 安全功率 */
            return -1;  /* 失败 */
            
        default:
            break;
    }
    
    return 1;  /* 进行中 */
}

/**
 * @brief  获取自整定结果
 */
void AutoTune_GetResult(AutoTuner *tuner, float *kp, float *ki)
{
    *kp = tuner->calcKp;
    *ki = tuner->calcKi;
}
```

### 5.4 用户接口

```c
/* 在LCD菜单中添加自整定选项 */

/* 触发自整定 */
void Pressure_StartAutoTune(void)
{
    AutoTune_Start(&autoTuner, currentSetpoint);
    autoTuneActive = 1;
    /* LCD显示: "自整定中..." */
}

/* 在主循环中调用 */
void Pressure_Control_Task(void)
{
    if (autoTuneActive)
    {
        int8_t result = AutoTune_Step(&autoTuner, setpoint, measurement, &output);
        
        if (result == 0)  /* 完成 */
        {
            float newKp, newKi;
            AutoTune_GetResult(&autoTuner, &newKp, &newKi);
            
            /* 应用新参数 */
            Pi_SetParams(&pressurePi, newKp, newKi);
            
            /* 保存到Flash */
            SavePiParams(newKp, newKi);
            
            autoTuneActive = 0;
            /* LCD显示: "自整定完成 Kp=x.xx Ki=x.xx" */
        }
        else if (result == -1)  /* 失败 */
        {
            autoTuneActive = 0;
            /* LCD显示: "自整定失败" */
        }
    }
    else
    {
        /* 正常PI控制 */
        output = Pi_Compute(&pressurePi, setpoint, measurement);
    }
    
    PWM_Adjust((uint8_t)output);
}
```

### 5.5 自整定流程图

```
用户触发
    │
    ▼
┌─────────────┐
│ 等待压力稳定 │ ◄── 中等功率50%
└──────┬──────┘
       │ 稳定10秒
       ▼
┌─────────────┐
│ 继电器高输出 │ ◄── 功率80%
│  等待压力升  │
└──────┬──────┘
       │ 压力 > 设定值
       ▼
┌─────────────┐
│ 继电器低输出 │ ◄── 功率30%
│  等待压力降  │
└──────┬──────┘
       │ 压力 < 设定值
       ▼
   已完成3个周期?
       │
   ┌───┴───┐
   │ 否    │ 是
   ▼       ▼
 返回高输出  计算参数
             │
             ▼
        ┌─────────────┐
        │ Tu = 平均周期 │
        │ a = 振幅     │
        │ Ku = 4d/πa  │
        │ Kp = 0.45Ku │
        │ Ki = 0.54Ku/Tu│
        └──────┬──────┘
               │
               ▼
         保存到Flash
               │
               ▼
           完成 ✓
```

---

## 6. 参数整定指南 (手动备选)

### 6.1 初始参数推荐

| 参数 | 标准模式 | 快速模式 | 稳定模式 |
|------|----------|----------|----------|
| Kp | 1.0 | 1.5 | 0.6 |
| Ki | 0.05 | 0.08 | 0.03 |
| 死区 | 0.02 MPa | 0.01 MPa | 0.03 MPa |
| 适用 | 通用 | 负载波动大 | 精密控制 |

### 5.2 手动整定步骤

```
1. 先设 Ki=0，只用P控制
2. 逐渐增大Kp，直到出现小幅振荡
3. 将Kp降低到振荡时的60%
4. 逐渐增大Ki，直到稳态误差消除
5. 如果超调，降低Ki
6. 微调死区减少频繁动作
```

### 5.3 典型响应曲线

```
压力
 ↑
 │     ┌──────────────────── 设定值
 │    ╱
 │   ╱   ← PI控制 (无超调)
 │  ╱
 │ ╱    ╱╲  ← 原步进控制 (振荡)
 │╱    ╱  ╲
 └──────────────────────────► 时间
     启动    稳态
```

---

## 6. 实施计划

| 阶段 | 内容 | 时间 |
|------|------|------|
| Phase 1 | 创建 `pressure_pid.h/c` | 2小时 |
| Phase 2 | 修改 `System_Pressure_Balance_Function()` | 2小时 |
| Phase 3 | 修改 `XB_System_Pressure_Balance_Function()` | 1小时 |
| Phase 4 | 仿真测试 (无硬件) | 1小时 |
| Phase 5 | 硬件调试整定 | 4-8小时 |

---

## 7. 回退方案

```c
/* 编译开关: 如需回退到原算法 */
#define USE_PI_CONTROL  1

#if USE_PI_CONTROL
    /* 使用PI控制 */
    outputPercent = Pi_Compute(&pressurePi, setpoint, measurement);
#else
    /* 回退到原步进控制 */
    // ... 原代码 ...
#endif
```

---

## 8. 验收标准

- [ ] 压力稳态误差 < ±0.02 MPa
- [ ] 无持续振荡 (功率波动 < ±3%)
- [ ] 升压时间 ≤ 原算法
- [ ] 超压保护正常工作
- [ ] 参数可通过LCD调整

---

**请审批此方案，批准后开始实施 Phase 1。**

