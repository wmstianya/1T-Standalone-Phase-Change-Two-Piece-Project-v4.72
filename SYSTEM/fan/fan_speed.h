/**
 * @file    fan_speed.h
 * @brief   风机转速检测模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.1
 * 
 * @details 本模块负责：
 *          - 风机脉冲计数（带软件消抖）
 *          - 转速计算与滤波
 *          - 转速异常检测
 * 
 * @note    优化方案：
 *          1. 中断消抖：过滤间隔过短的脉冲（杂波）
 *          2. 滑动平均：多次采样取平均，平滑转速值
 *          3. 异常值剔除：超出合理范围的值不计入
 */

#ifndef __FAN_SPEED_H
#define __FAN_SPEED_H

#include "main.h"

/*==============================================================================
 * 配置参数
 *============================================================================*/

/**
 * @brief  最小脉冲间隔 (微秒)
 * @note   用于中断消抖，过滤杂波
 *         计算依据：最高转速6000rpm，每转3脉冲
 *         最小间隔 = 60s / 6000rpm / 3脉冲 = 3.33ms = 3333us
 *         设置为 2000us (2ms) 留有余量
 */
#define FAN_MIN_PULSE_INTERVAL_US   2000

/**
 * @brief  滑动平均窗口大小
 * @note   用于平滑转速值，建议3-5
 */
#define FAN_FILTER_WINDOW_SIZE      4

/**
 * @brief  转速合理范围 (RPM)
 */
#define FAN_RPM_MIN                 100
#define FAN_RPM_MAX                 8000

/*==============================================================================
 * 数据结构
 *============================================================================*/

/**
 * @brief  风机转速滤波器结构
 */
typedef struct {
    uint16_t buffer[FAN_FILTER_WINDOW_SIZE];  /* 历史转速缓冲 */
    uint8_t  index;                            /* 当前写入位置 */
    uint8_t  count;                            /* 有效数据个数 */
    uint32_t lastPulseTime;                    /* 上次脉冲时间(us) */
} FanSpeedFilter_t;

/*==============================================================================
 * 外部变量
 *============================================================================*/
extern FanSpeedFilter_t fanFilter;

/*==============================================================================
 * 函数声明
 *============================================================================*/

/**
 * @brief  风机转速检测初始化
 */
void Fan_Speed_Init(void);

/**
 * @brief  风机转速检测功能（每秒调用一次）
 * @note   从 system_control.c 迁移，增加滑动平均滤波
 */
void Fan_Speed_Check_Function(void);

/**
 * @brief  风机脉冲中断处理（在EXTI0中调用）
 * @note   带消抖功能，过滤杂波
 */
void Fan_Pulse_ISR_Handler(void);

/**
 * @brief  获取滤波后的转速值
 * @retval 滤波后的RPM值
 */
uint16_t Fan_Get_Filtered_Rpm(void);

/**
 * @brief  设置消抖使能
 * @param  enable: 1=使能消抖, 0=禁用消抖（兼容原逻辑）
 */
void Fan_Set_Debounce(uint8_t enable);

#endif /* __FAN_SPEED_H */

