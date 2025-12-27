/**
 * @file    sys_config.h
 * @brief   系统配置管理模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @details 本模块负责：
 *          - 系统参数配置与校验
 *          - 工作时间统计
 *          - 管理员锁机功能
 *          - WiFi锁定时间检测
 *          - 相变蒸汽追加控制
 */

#ifndef __SYS_CONFIG_H
#define __SYS_CONFIG_H

#include "main.h"

/*==============================================================================
 * 函数声明
 *============================================================================*/

/**
 * @brief  系统工作时间统计
 * @retval 0=正常
 * @note   统计系统累计运行时间，开炉运行时间
 */
uint8 sys_work_time_function(void);

/**
 * @brief  系统控制配置功能
 * @note   配置开机系统默认参数，从Flash读取或写入默认值
 */
void sys_control_config_function(void);

/**
 * @brief  配置数据检查
 * @note   校验各项配置参数，填充LCD10D数据结构
 */
void Check_Config_Data_Function(void);

/**
 * @brief  管理员工作时间功能
 * @retval 0=正常
 * @note   用于经销商管理，限制设备运行的时间
 */
uint8 Admin_Work_Time_Function(void);

/**
 * @brief  WiFi锁定时间功能
 * @retval 0=未锁定, OK=已锁定
 * @note   检测WiFi锁定日期，判断是否需要锁机
 */
uint8 Wifi_Lock_Time_Function(void);

/**
 * @brief  相变蒸汽追加功能
 * @retval 0=正常
 * @note   高压侧水位、压控保护逻辑
 */
uint8 XiangBian_Steam_AddFunction(void);

#endif /* __SYS_CONFIG_H */

