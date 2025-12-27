/**
 * @file    fan_speed.c
 * @brief   风机转速检测模块实现
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.1
 * 
 * @note    优化内容：
 *          1. 中断消抖：使用最小脉冲间隔过滤杂波
 *          2. 滑动平均滤波：平滑转速波动
 *          3. 异常值剔除：超出合理范围的值不计入
 */

#include "fan_speed.h"

/*==============================================================================
 * 全局变量
 *============================================================================*/
FanSpeedFilter_t fanFilter = {0};

/* 消抖用的脉冲计数（替代原 sys_flag.Fan_count） */
static volatile uint16_t fanPulseCount = 0;

/* 上次脉冲时间（毫秒级，使用 sys_flag.test_Ms 近似） */
static volatile uint8_t lastPulseMs = 0;

/*==============================================================================
 * 内部函数
 *============================================================================*/

/**
 * @brief  添加转速值到滑动平均滤波器
 * @param  rpm: 原始转速值
 */
static void Filter_Add_Sample(uint16_t rpm)
{
    /* 异常值剔除 */
    if(rpm > FAN_RPM_MAX || (rpm > 0 && rpm < FAN_RPM_MIN))
    {
        return;  /* 不计入明显异常的值 */
    }
    
    fanFilter.buffer[fanFilter.index] = rpm;
    fanFilter.index = (fanFilter.index + 1) % FAN_FILTER_WINDOW_SIZE;
    
    if(fanFilter.count < FAN_FILTER_WINDOW_SIZE)
    {
        fanFilter.count++;
    }
}

/**
 * @brief  计算滑动平均值
 * @retval 滤波后的RPM
 */
static uint16_t Filter_Get_Average(void)
{
    uint32_t sum = 0;
    uint8_t i;
    
    if(fanFilter.count == 0)
    {
        return 0;
    }
    
    for(i = 0; i < fanFilter.count; i++)
    {
        sum += fanFilter.buffer[i];
    }
    
    return (uint16_t)(sum / fanFilter.count);
}

/*==============================================================================
 * 公共函数
 *============================================================================*/

/**
 * @brief  风机转速检测初始化
 */
void Fan_Speed_Init(void)
{
    uint8_t i;
    
    fanPulseCount = 0;
    lastPulseMs = 0;
    
    fanFilter.index = 0;
    fanFilter.count = 0;
    fanFilter.lastPulseTime = 0;
    
    for(i = 0; i < FAN_FILTER_WINDOW_SIZE; i++)
    {
        fanFilter.buffer[i] = 0;
    }
}

/**
 * @brief  风机脉冲中断处理（在EXTI0中调用）
 * @note   带消抖功能
 * 
 * 消抖原理：
 *   - 记录上次有效脉冲的时间
 *   - 如果两次脉冲间隔太短（<2ms），认为是杂波，忽略
 *   - 正常脉冲间隔：6000rpm@3脉冲/转 = 3.33ms
 */
void Fan_Pulse_ISR_Handler(void)
{
    uint8_t currentMs = sys_flag.test_Ms;
    uint8_t deltaMs;
    
    /* 计算时间差（处理溢出） */
    if(currentMs >= lastPulseMs)
    {
        deltaMs = currentMs - lastPulseMs;
    }
    else
    {
        deltaMs = (255 - lastPulseMs) + currentMs + 1;
    }
    
    /* 消抖：间隔小于2ms认为是杂波，忽略 */
    /* FAN_MIN_PULSE_INTERVAL_US = 2000us = 2ms */
    if(deltaMs >= 2)
    {
        fanPulseCount++;
        lastPulseMs = currentMs;
    }
    /* else: 杂波，忽略 */
}

/**
 * @brief  风机转速检测功能（每秒调用一次）
 * @note   从 system_control.c 迁移，增加滑动平均滤波
 */
void Fan_Speed_Check_Function(void)
{
    uint16_t rawRpm = 0;
    
    if(sys_flag.Rpm_1_Sec)
    {
        sys_flag.Rpm_1_Sec = FALSE;
        
        /* 参数校验 */
        if(Sys_Admin.Fan_Pulse_Rpm >= 10 || Sys_Admin.Fan_Pulse_Rpm == 0)
        {
            Sys_Admin.Fan_Pulse_Rpm = 3;  /* 默认每转3脉冲 */
        }
        
        /* 计算原始转速 */
        if(fanPulseCount > 0)
        {
            /* RPM = (脉冲数 / 每转脉冲数) * 60 */
            rawRpm = (uint16_t)(fanPulseCount * 60 / Sys_Admin.Fan_Pulse_Rpm);
            fanPulseCount = 0;
        }
        else
        {
            rawRpm = 0;
            fanPulseCount = 0;
        }
        
        /* 添加到滤波器 */
        Filter_Add_Sample(rawRpm);
        
        /* 获取滤波后的值 */
        sys_flag.Fan_Rpm = Filter_Get_Average();
    }
}

/**
 * @brief  获取滤波后的转速值
 * @retval 滤波后的RPM值
 */
uint16_t Fan_Get_Filtered_Rpm(void)
{
    return Filter_Get_Average();
}

