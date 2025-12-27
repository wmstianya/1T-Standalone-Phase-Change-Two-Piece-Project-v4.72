/**
 * @file    sys_cmd.h
 * @brief   系统启停命令模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @details 本模块负责：
 *          - 系统启动命令处理
 *          - 系统关闭命令处理
 *          - 自动启停过程控制
 */

#ifndef __SYS_CMD_H
#define __SYS_CMD_H

#include "main.h"

/*==============================================================================
 * 函数声明
 *============================================================================*/

/**
 * @brief  系统启动命令
 * @retval 0=正常
 * @note   处理系统从待机到运行的状态转换
 */
uint8 sys_start_cmd(void);

/**
 * @brief  系统关闭命令
 * @note   处理系统从运行到待机的状态转换，包含后吹扫启动
 */
void sys_close_cmd(void);

/**
 * @brief  自动启停过程功能
 * @retval 0=正常
 * @note   自动启停控制逻辑
 */
uint8 Auto_StartOrClose_Process_Function(void);

#endif /* __SYS_CMD_H */

