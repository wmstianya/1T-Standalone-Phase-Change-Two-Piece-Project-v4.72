/**
 * @file    pressure_ctrl.c
 * @brief   压力控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 2.0 - 增加PI控制支持
 * 
 * @note    本文件包含压力控制函数
 *          支持增量式PI控制和原步进控制两种模式
 *          通过 USE_PI_CONTROL 宏切换
 */

#include "pressure_ctrl.h"
#include "pwm_output.h"

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern uint8 now_percent;

/*==============================================================================
 * PI控制器实例 (USE_PI_CONTROL=1时有效)
 *============================================================================*/
#if USE_PI_CONTROL

PiController gPressurePi;       /* PI控制器实例 */
AutoTuner gAutoTuner;           /* 自整定器实例 */
uint8_t gAutoTuneActive = 0;    /* 自整定进行中标志 */
static uint8_t gPiInitialized = 0;  /* PI初始化标志 */

#endif /* USE_PI_CONTROL */

/*==============================================================================
 * PI控制辅助函数
 *============================================================================*/
#if USE_PI_CONTROL

/**
 * @brief  初始化压力PI控制器
 * @note   Bug4修复: 添加airMin有效性检查
 */
void Pressure_Pi_Init(void)
{
    uint8_t airMin, airMax;
    
    /* 获取功率范围 */
    airMin = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);
    airMax = Sys_Admin.Max_Work_Power;
    
    /* Bug4修复: 功率范围有效性校验 */
    if (airMax > 100) airMax = 100;
    if (airMax < 20) airMax = 25;
    if (airMin > 100) airMin = 20;  /* Bug4: 防止Flash数据损坏 */
    if (airMin < 10) airMin = 15;   /* Bug4: 最小不低于10% */
    if (airMin > airMax) airMin = 20;
    
    /* 初始化PI控制器 */
    Pi_Init(&gPressurePi, 
            PI_PRESET_STANDARD.kp, 
            PI_PRESET_STANDARD.ki,
            (float)airMin, 
            (float)airMax);
    
    Pi_SetDeadband(&gPressurePi, PI_PRESET_STANDARD.deadband);
    
    gPiInitialized = 1;
    gAutoTuneActive = 0;
}

/**
 * @brief  启动PI参数自整定
 */
void Pressure_StartAutoTune(void)
{
    float setpoint;
    
    /* 获取当前设定压力 (MPa) */
    setpoint = (float)sys_config_data.zhuan_huan_temperture_value / 100.0f;
    
    /* 相变设备需要加压差补偿 */
    if (Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
    {
        setpoint += 0.65f;
    }
    
    AutoTune_Start(&gAutoTuner, setpoint);
    gAutoTuneActive = 1;
}

/**
 * @brief  检查自整定是否完成
 * @retval 0=进行中, 1=完成, -1=失败
 */
int8_t Pressure_CheckAutoTune(void)
{
    TuneState state = AutoTune_GetState(&gAutoTuner);
    
    if (state == TUNE_DONE)
    {
        return 1;
    }
    else if (state == TUNE_FAILED)
    {
        return -1;
    }
    
    return 0;  /* 进行中 */
}

/**
 * @brief  应用自整定结果到PI控制器
 * @note   Bug6修复: 自整定后重置PI状态
 */
void Pressure_ApplyAutoTuneResult(void)
{
    float kp, ki;
    
    AutoTune_GetResult(&gAutoTuner, &kp, &ki);
    Pi_SetParams(&gPressurePi, kp, ki);
    
    /* Bug6修复: 自整定后重置PI状态，避免旧的lastError和output影响 */
    Pi_Reset(&gPressurePi);
    
    gAutoTuneActive = 0;
}

/**
 * @brief  取消自整定
 */
void Pressure_CancelAutoTune(void)
{
    AutoTune_Cancel(&gAutoTuner);
    gAutoTuneActive = 0;
}

/**
 * @brief  获取当前PI参数
 */
void Pressure_GetPiParams(float *kp, float *ki)
{
    if (kp != NULL)
    {
        *kp = gPressurePi.kp;
    }
    if (ki != NULL)
    {
        *ki = gPressurePi.ki;
    }
}

/**
 * @brief  设置PI参数
 */
void Pressure_SetPiParams(float kp, float ki)
{
    Pi_SetParams(&gPressurePi, kp, ki);
}

#endif /* USE_PI_CONTROL */

/*==============================================================================
 * 压力控制函数实现
 *============================================================================*/

/**
 * @brief  单机压力平衡控制
 */
uint8 System_Pressure_Balance_Function(void)
{
#if USE_PI_CONTROL
    /* ========== PI控制模式 ========== */
    float setpoint;         /* 设定压力 MPa */
    float measurement;      /* 实际压力 MPa */
    float outputPercent;    /* 输出功率 % */
    uint16_t stopPressure;  /* 停炉压力 */
    
    /* 首次调用初始化PI */
    if (!gPiInitialized)
    {
        Pressure_Pi_Init();
    }
    
    /* 获取设定值 (转换为MPa) */
    setpoint = (float)sys_config_data.zhuan_huan_temperture_value / 100.0f;
    stopPressure = sys_config_data.Auto_stop_pressure;
    
    /* 获取实际压力 (转换为MPa) */
    if (Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
    {
        /* 相变设备使用高压侧压力 + 压差补偿 */
        measurement = (float)Temperature_Data.Inside_High_Pressure / 100.0f;
        setpoint += 0.65f;  /* 压差补偿 0.65 MPa */
        stopPressure += 65;
    }
    else
    {
        measurement = (float)Temperature_Data.Pressure_Value / 100.0f;
    }
    
    /* 自整定模式 */
    if (gAutoTuneActive)
    {
        int8_t tuneResult = AutoTune_Step(&gAutoTuner, measurement, &outputPercent);
        
        if (tuneResult == 0)  /* 完成 */
        {
            Pressure_ApplyAutoTuneResult();
        }
        else if (tuneResult == -1)  /* 失败 */
        {
            gAutoTuneActive = 0;
        }
        
        now_percent = (uint8_t)outputPercent;
    }
    else
    {
        /* 正常PI控制 - 每秒计算一次 */
        if (sys_flag.Power_1_Sec)
        {
            sys_flag.Power_1_Sec = 0;
            outputPercent = Pi_Compute(&gPressurePi, setpoint, measurement);
            now_percent = (uint8_t)outputPercent;
        }
    }
    
    /* 输出PWM */
    PWM_Adjust(now_percent);
    
    /* 超压保护 */
    if (Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
    {
        sys_data.Data_12H |= Set_Bit_4;
        Abnormal_Events.target_complete_event = 1;
    }
    
    /* 相变设备额外检查高压侧 */
    if (Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
    {
        if (Temperature_Data.Inside_High_Pressure >= stopPressure)
        {
            sys_data.Data_12H |= Set_Bit_4;
            Abnormal_Events.target_complete_event = 1;
        }
    }
    
    return OK;
    
#else
    /* ========== 原步进控制模式 ========== */
    static uint16 Man_Set_Pressure = 0;
    static uint8 air_min = 0;
    static uint8 air_max = 0;
    static uint16 stop_wait_pressure = 0;
    uint8 Tp_value = 0;

    uint16 Real_Pressure = 0;
    static uint8 Yacha_Value = 65;
    uint16 Max_Pressure = 150;

    /* 根据设备类型选择压力源 */
    if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
    {
        Yacha_Value = 65;
        Real_Pressure = Temperature_Data.Inside_High_Pressure;
    }
    else
    {
        Yacha_Value = 0;
        Real_Pressure = Temperature_Data.Pressure_Value;
    }

    /* 获取功率范围 */
    air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);
    air_max = Sys_Admin.Max_Work_Power;
    
    if(air_max >= 100)
        air_max = 100;
    if(air_max < 20)
        air_max = 25;

    /* 计算目标压力 */
    Man_Set_Pressure = sys_config_data.zhuan_huan_temperture_value + Yacha_Value;
    stop_wait_pressure = sys_config_data.Auto_stop_pressure + Yacha_Value;
    
    Tp_value = now_percent;

    /* 压力低于设定值 - 升功率 */
    if(Temperature_Data.Pressure_Value < sys_config_data.zhuan_huan_temperture_value)
    {
        if(Real_Pressure < Man_Set_Pressure)
        {
            if(sys_flag.Pressure_ChangeTime > 6)
            {
                sys_flag.get_60_percent_flag = OK;
            }
            if(sys_flag.Pressure_ChangeTime <= 5)
            {
                sys_flag.get_60_percent_flag = 0;
            }
            
            if(sys_flag.get_60_percent_flag)
            {
                if(sys_flag.Power_1_Sec)
                {
                    sys_flag.Power_1_Sec = 0;
                    Tp_value = Tp_value + 1;
                }
            }
            else
            {
                if(sys_flag.Power_5_Sec)
                {
                    sys_flag.Power_5_Sec = 0;
                    Tp_value = Tp_value + 1;
                }
            }
        }
    }

    /* 压力等于设定值 - 保持 */
    if(Real_Pressure == Man_Set_Pressure)
    {
        if(now_percent > 80)
        {
            Tp_value = 80;
        }
        sys_flag.get_60_percent_flag = 1;
    }

    /* 压力高于设定值 - 降功率 */
    if(Real_Pressure > Man_Set_Pressure || Temperature_Data.Pressure_Value > sys_config_data.zhuan_huan_temperture_value)
    {
        if(Temperature_Data.Pressure_Value > (sys_config_data.zhuan_huan_temperture_value))
        {
            if(Real_Pressure > Man_Set_Pressure)
            {
                if(now_percent > 80)
                {
                    Tp_value = 70;
                }
                if(sys_flag.Power_1_Sec)
                {
                    sys_flag.Power_1_Sec = 0;
                    Tp_value = Tp_value - 1;
                }
            }
            else
            {
                if(Real_Pressure <= (Man_Set_Pressure - 5))
                {
                    if(Temperature_Data.Pressure_Value < (sys_config_data.zhuan_huan_temperture_value + 1))
                    {
                        if(sys_flag.Power_1_Sec)
                        {
                            sys_flag.Power_1_Sec = 0;
                            Tp_value = Tp_value + 1;
                        }
                    }
                    else
                    {
                        if(sys_flag.Power_1_Sec)
                        {
                            sys_flag.Power_1_Sec = 0;
                            Tp_value = Tp_value - 1;
                        }
                    }
                }
                else
                {
                    if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value + 1))
                    {
                        if(sys_flag.Power_1_Sec)
                        {
                            sys_flag.Power_1_Sec = 0;
                            Tp_value = Tp_value - 1;
                        }
                    }
                }
            }
        }
        else
        {
            if(Real_Pressure > Man_Set_Pressure)
            {
                if(sys_flag.Power_1_Sec)
                {
                    sys_flag.Power_1_Sec = 0;
                    Tp_value = Tp_value - 1;
                }
            }
        }
    }

    /* 限制功率范围 */
    now_percent = Tp_value;
    if(now_percent > air_max)
        now_percent = air_max;
    if(now_percent < air_min)
        now_percent = air_min;

    if(now_percent >= 70)
        sys_flag.get_60_percent_flag = 1;

    /* 输出PWM */
    PWM_Adjust(now_percent);

    /* 超压停炉 */
    if(Real_Pressure >= stop_wait_pressure || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
    {
        sys_data.Data_12H |= Set_Bit_4;
        Abnormal_Events.target_complete_event = 1;
    }

    return OK;
#endif /* USE_PI_CONTROL */
}

/**
 * @brief  相变压力平衡控制
 */
uint8 XB_System_Pressure_Balance_Function(void)
{
#if USE_PI_CONTROL
    /* ========== PI控制模式 ========== */
    /* 相变模式与单机模式使用相同的PI控制器 */
    /* 区别在于压力源和压差补偿已在 System_Pressure_Balance_Function 中处理 */
    return System_Pressure_Balance_Function();
    
#else
    /* ========== 原步进控制模式 ========== */
    static uint16 Man_Set_Pressure = 0;
    static uint8 air_min = 0;
    static uint8 air_max = 0;
    static uint16 stop_wait_pressure = 0;
    uint8 Tp_value = 0;

    uint16 Real_Pressure = 0;
    static uint8 Yacha_Value = 65;
    uint16 Max_Pressure = 150;

    /* 根据设备类型选择压力源 */
    if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
    {
        Yacha_Value = 65;
        Real_Pressure = Temperature_Data.Inside_High_Pressure;
    }
    else
    {
        Yacha_Value = 0;
        Real_Pressure = Temperature_Data.Pressure_Value;
    }

    /* 获取功率范围 */
    air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);
    air_max = Sys_Admin.Max_Work_Power;
    
    if(air_max >= 100)
        air_max = 100;
    if(air_max < 20)
        air_max = 25;

    /* 计算目标压力 */
    Man_Set_Pressure = sys_config_data.zhuan_huan_temperture_value + Yacha_Value;
    stop_wait_pressure = sys_config_data.Auto_stop_pressure + Yacha_Value;
    
    Tp_value = now_percent;

    /* 压力低于设定值 - 升功率 */
    if(Temperature_Data.Pressure_Value < sys_config_data.zhuan_huan_temperture_value)
    {
        if(Real_Pressure < Man_Set_Pressure)
        {
            if(sys_flag.Pressure_ChangeTime > 6)
            {
                sys_flag.get_60_percent_flag = OK;
            }
            if(sys_flag.Pressure_ChangeTime <= 5)
            {
                sys_flag.get_60_percent_flag = 0;
            }
            
            if(sys_flag.get_60_percent_flag)
            {
                if(sys_flag.Power_1_Sec)
                {
                    sys_flag.Power_1_Sec = 0;
                    Tp_value = Tp_value + 1;
                }
            }
            else
            {
                if(sys_flag.Power_5_Sec)
                {
                    sys_flag.Power_5_Sec = 0;
                    Tp_value = Tp_value + 1;
                }
            }
        }
    }

    /* 压力等于设定值 */
    if(Real_Pressure == Man_Set_Pressure)
    {
        if(now_percent > 80)
        {
            Tp_value = 80;
        }
        sys_flag.get_60_percent_flag = 1;
    }

    /* 压力高于设定值 - 降功率 */
    if(Real_Pressure > Man_Set_Pressure || Temperature_Data.Pressure_Value >= sys_config_data.zhuan_huan_temperture_value)
    {
        if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value))
        {
            if(Real_Pressure > Man_Set_Pressure)
            {
                if(now_percent > 80)
                {
                    Tp_value = 70;
                }
                if(sys_flag.Power_1_Sec)
                {
                    sys_flag.Power_1_Sec = 0;
                    Tp_value = Tp_value - 1;
                }
            }
            else
            {
                if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value + 2))
                {
                    if(sys_flag.Power_1_Sec)
                    {
                        sys_flag.Power_1_Sec = 0;
                        Tp_value = Tp_value - 1;
                    }
                }
                else
                {
                    if(Real_Pressure <= (Man_Set_Pressure - 10))
                    {
                        if(sys_flag.Power_1_Sec)
                        {
                            sys_flag.Power_1_Sec = 0;
                            Tp_value = Tp_value + 1;
                        }
                    }
                }
            }
        }
        else
        {
            if(Real_Pressure > Man_Set_Pressure)
            {
                if(sys_flag.Power_1_Sec)
                {
                    sys_flag.Power_1_Sec = 0;
                    Tp_value = Tp_value - 1;
                }
            }
        }
    }

    /* 限制功率范围 */
    now_percent = Tp_value;
    if(now_percent > air_max)
        now_percent = air_max;
    if(now_percent < air_min)
        now_percent = air_min;

    if(now_percent >= 70)
        sys_flag.get_60_percent_flag = 1;

    /* 输出PWM */
    PWM_Adjust(now_percent);

    /* 超压停炉 */
    if(Real_Pressure >= stop_wait_pressure || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
    {
        sys_data.Data_12H |= Set_Bit_4;
        Abnormal_Events.target_complete_event = 1;
    }

    return OK;
#endif /* USE_PI_CONTROL */
}

/**
 * @brief  压力变化速度检测
 * @note   PI控制模式下此函数仍然有效，用于其他模块参考
 */
uint8 Speed_Pressure_Function(void)
{
    static uint16 Old_Pressure = 0;
    uint16 New_Pressure = 0;
    static uint16 TimeCount = 0;
    uint8 Chazhi = 0;

    /* 根据设备类型选择压力源 */
    if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
    {
        New_Pressure = Temperature_Data.Inside_High_Pressure;
    }
    else
    {
        New_Pressure = Temperature_Data.Pressure_Value;
    }

    /* 每秒计算一次压力变化 */
    if(sys_flag.Pressure_1sFlag)
    {
        sys_flag.Pressure_1sFlag = 0;
        
        if(sys_flag.flame_state)
        {
            TimeCount++;
            
            /* 压力上升 */
            if(New_Pressure > Old_Pressure)
            {
                Chazhi = New_Pressure - Old_Pressure;
                Old_Pressure = New_Pressure;
                sys_flag.Pressure_ChangeTime = TimeCount;
                if (Chazhi > 0)
                {
                    sys_flag.Pressure_ChangeTime = sys_flag.Pressure_ChangeTime / Chazhi;
                }
                TimeCount = 0;
            }

            /* 压力下降 */
            if(New_Pressure < Old_Pressure)
            {
                Chazhi = Old_Pressure - New_Pressure;
                Old_Pressure = New_Pressure;
                sys_flag.Pressure_ChangeTime = TimeCount;
                if (Chazhi > 0)
                {
                    sys_flag.Pressure_ChangeTime = sys_flag.Pressure_ChangeTime / Chazhi;
                }
                TimeCount = 0;
            }
        }
        else
        {
            /* 没有火焰时状态清零 */
            Old_Pressure = New_Pressure;
            TimeCount = 0;
            sys_flag.Pressure_ChangeTime = 0;
        }
    }

    return 0;
}
