/**
 * @file    pressure_ctrl.h
 * @brief   压力控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 2.0 - 增加PI控制支持
 * 
 * @details 本模块负责：
 *          - 单机压力平衡控制 (支持PI/原步进算法切换)
 *          - 相变压力平衡控制 (支持PI/原步进算法切换)
 *          - 压力变化速度检测
 *          - PI参数自整定
 */

#ifndef __PRESSURE_CTRL_H
#define __PRESSURE_CTRL_H

#include "main.h"
#include "pressure_pid.h"

/*==============================================================================
 * 编译开关
 *============================================================================*/

/**
 * @brief 启用PI控制算法
 *        1 = 使用增量式PI控制 (推荐)
 *        0 = 使用原步进控制 (回退)
 */
#define USE_PI_CONTROL  1

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern uint8 now_percent;  /* 当前功率百分比 */

/*==============================================================================
 * PI控制器实例 (USE_PI_CONTROL=1时有效)
 *============================================================================*/
#if USE_PI_CONTROL
extern PiController gPressurePi;       /* PI控制器实例 */
extern AutoTuner gAutoTuner;           /* 自整定器实例 */
extern uint8_t gAutoTuneActive;        /* 自整定进行中标志 */
#endif

/*==============================================================================
 * 函数声明 - 压力控制
 *============================================================================*/

/**
 * @brief  单机压力平衡控制
 * @retval OK=正常
 * @note   USE_PI_CONTROL=1: 使用增量式PI控制
 *         USE_PI_CONTROL=0: 使用原步进控制
 */
uint8 System_Pressure_Balance_Function(void);

/**
 * @brief  相变压力平衡控制
 * @retval OK=正常
 * @note   USE_PI_CONTROL=1: 使用增量式PI控制
 *         USE_PI_CONTROL=0: 使用原步进控制
 */
uint8 XB_System_Pressure_Balance_Function(void);

/**
 * @brief  压力变化速度检测
 * @retval 0=正常
 * @note   计算压力变化时间，用于判断是否需要加速/减速功率调节
 */
uint8 Speed_Pressure_Function(void);

/*==============================================================================
 * 函数声明 - PI控制与自整定
 *============================================================================*/

#if USE_PI_CONTROL
/**
 * @brief  初始化压力PI控制器
 * @note   在系统初始化时调用一次
 */
void Pressure_Pi_Init(void);

/**
 * @brief  启动PI参数自整定
 * @note   调用后系统自动执行继电器反馈法整定
 *         整定过程约2-3分钟
 */
void Pressure_StartAutoTune(void);

/**
 * @brief  检查自整定是否完成
 * @retval 0=进行中, 1=完成, -1=失败
 */
int8_t Pressure_CheckAutoTune(void);

/**
 * @brief  应用自整定结果到PI控制器
 */
void Pressure_ApplyAutoTuneResult(void);

/**
 * @brief  取消自整定
 */
void Pressure_CancelAutoTune(void);

/**
 * @brief  获取当前PI参数
 * @param  kp  Kp输出指针
 * @param  ki  Ki输出指针
 */
void Pressure_GetPiParams(float *kp, float *ki);

/**
 * @brief  设置PI参数
 * @param  kp  新的Kp值
 * @param  ki  新的Ki值
 */
void Pressure_SetPiParams(float kp, float ki);
#endif /* USE_PI_CONTROL */

#endif /* __PRESSURE_CTRL_H */
