/**
 * @file    pressure_ctrl.h
 * @brief   压力控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @details 本模块负责：
 *          - 单机压力平衡控制
 *          - 相变压力平衡控制
 *          - 压力变化速度检测
 */

#ifndef __PRESSURE_CTRL_H
#define __PRESSURE_CTRL_H

#include "main.h"

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern uint8 now_percent;  /* 当前功率百分比 */

/*==============================================================================
 * 函数声明
 *============================================================================*/

/**
 * @brief  单机压力平衡控制
 * @retval OK=正常
 * @note   根据压力与设定值比较，动态调节PWM输出功率
 *         适用于单机模式(Device_Style=0,2)
 */
uint8 System_Pressure_Balance_Function(void);

/**
 * @brief  相变压力平衡控制
 * @retval OK=正常
 * @note   根据内外压差动态调节功率
 *         适用于相变模式(Device_Style=1,3)
 */
uint8 XB_System_Pressure_Balance_Function(void);

/**
 * @brief  压力变化速度检测
 * @retval 0=正常
 * @note   计算压力变化时间，用于判断是否需要加速/减速功率调节
 */
uint8 Speed_Pressure_Function(void);

#endif /* __PRESSURE_CTRL_H */

