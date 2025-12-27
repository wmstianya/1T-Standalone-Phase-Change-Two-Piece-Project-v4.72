/**
 * @file    water_control.h
 * @brief   水位控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @details 本模块负责：
 *          - 水位平衡控制（单机/双拼模式）
 *          - 变频水泵控制
 *          - 特殊补水功能
 *          - 水位状态检测
 */

#ifndef __WATER_CONTROL_H
#define __WATER_CONTROL_H

#include "main.h"

/*==============================================================================
 * 水位状态定义
 *============================================================================*/
/* 水位信号位定义 (lcd_data.Data_15L):
 * bit0 - water_protect (保护水位)
 * bit1 - water_mid (中水位)
 * bit2 - water_high (高水位)
 * bit3 - water_shigh (超高水位)
 */

/*==============================================================================
 * 函数声明
 *============================================================================*/

/**
 * @brief  单机水位平衡控制
 * @retval 0=正常
 * @note   适用于单机模式(Device_Style=1)
 *         控制主水泵和二次水阀
 */
uint8 Water_Balance_Function(void);

/**
 * @brief  特殊补水功能
 * @retval 0=正常
 * @note   高水位丢失时延时开启副补水阀
 */
uint8 Special_Water_Supply_Function(void);

/**
 * @brief  水位变化检测
 * @retval 0=正常
 * @note   检测水位长时间无变化报警
 */
uint8 WaterLevel_Unchange_Check(void);

/**
 * @brief  单机变频水泵控制
 * @retval 0=正常
 * @note   根据水位状态调节水泵频率
 */
uint8 Water_BianPin_Function(void);

/**
 * @brief  双拼水位平衡控制
 * @retval 0=正常
 * @note   适用于双拼模式(Device_Style=2,3)
 *         主从机协调补水
 */
uint8 ShuangPin_Water_Balance_Function(void);

/**
 * @brief  双水泵逻辑控制
 * @retval 0=正常
 * @note   主从机水泵信号协调
 */
uint8 Double_WaterPump_LogicFunction(void);

/**
 * @brief  双拼变频水泵控制
 * @retval 0=正常
 * @note   双拼模式下的变频补水控制
 */
uint8 Double_Water_BianPin_Function(void);

#endif /* __WATER_CONTROL_H */

