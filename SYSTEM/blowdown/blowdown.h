/**
 * @file    blowdown.h
 * @brief   排污控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @details 本模块负责：
 *          - 自动排污控制
 *          - 连续排污控制
 *          - 排污警告计时
 *          - 待机排污功能
 */

#ifndef __BLOWDOWN_H
#define __BLOWDOWN_H

#include "main.h"

/*==============================================================================
 * 函数声明
 *============================================================================*/

/**
 * @brief  系统待机功能
 * @note   待机时关闭风门、点火、燃气阀，执行自动排污
 */
void System_Idel_Function(void);

/**
 * @brief  待机自动排污
 * @retval 0=正常
 * @note   空函数，待实现
 */
uint8 IDLE_Auto_Pai_Wu_Function(void);

/**
 * @brief  自动排污功能
 * @retval 0=进行中, OK=完成
 * @note   状态机控制：开阀→检测水位→关阀→补水
 */
uint8 Auto_Pai_Wu_Function(void);

/**
 * @brief  运行中可调排污
 * @retval 0=正常
 * @note   空函数，待实现
 */
uint8 YunXingZhong_TimeAdjustable_PaiWu_Function(void);

/**
 * @brief  排污警告功能
 * @retval 0=正常
 * @note   未排污时间计时，超时报警
 */
uint8 PaiWu_Warnning_Function(void);

/**
 * @brief  连续排污控制
 * @retval 0=正常
 * @note   根据工作时间定时开启排污阀
 */
uint8 LianXu_Paiwu_Control_Function(void);

#endif /* __BLOWDOWN_H */

