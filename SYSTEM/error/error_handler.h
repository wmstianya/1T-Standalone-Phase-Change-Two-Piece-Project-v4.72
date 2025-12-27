/**
 * @file    error_handler.h
 * @brief   故障检测与响应模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @details 本模块负责：
 *          - IO信息获取与异常检测
 *          - 系统自检功能
 *          - 运行中/待机故障检测
 *          - 异常事件响应处理
 *          - 故障响应与报警
 */

#ifndef __ERROR_HANDLER_H
#define __ERROR_HANDLER_H

#include "main.h"

/*==============================================================================
 * 错误码定义
 *============================================================================*/
/* 注意：错误码已在 system_control.h 中定义，此处仅作参考
 * Error1_YakongProtect     - 压控保护
 * Error2_YaBianProtect     - 压变保护
 * Error3_LowGas            - 燃气压力低
 * Error5_LowWater          - 缺水
 * Error7_FlameZiJian       - 火焰自检异常
 * Error8_WaterLogic        - 水位逻辑异常
 * Error9_AirPressureBad    - 风压异常
 * Error11_DianHuo_Bad      - 点火失败
 * Error12_FlameLose        - 运行中火焰熄灭
 * Error13_AirControlFail   - 风控失败
 * Error15_RebaoBad         - 热保护触发
 * Error16_SmokeValueHigh   - 烟温过高
 * Error19_NotSupplyWater   - 未补水
 * Error20_XB_HighPressureYabian_Bad - 相变高压压变异常
 * Error21_XB_HighPressureYAKONG_Bad - 相变高压压控异常
 * Error22_XB_HighPressureWater_Low  - 相变高压缺水
 */

/*==============================================================================
 * 函数声明
 *============================================================================*/

/**
 * @brief  获取IO信息，检测水位逻辑、热保护、压控信号异常
 * @param  无
 * @retval 无
 * @note   检测项目：
 *         - 水位逻辑异常(Error8)
 *         - 热保护触发(Error15)
 *         - 压控保护(Error1)
 */
void Get_IO_Inf(void);

/**
 * @brief  系统自检功能
 * @param  无
 * @retval 无
 * @note   调用Get_IO_Inf，检测烟温
 */
void Self_Check_Function(void);

/**
 * @brief  运行中自检功能
 * @param  无
 * @retval 无
 * @note   检测项目：
 *         - 风门状态(Error9)
 *         - 燃气压力(Error3)
 *         - 极限水位(Error5)
 *         - 火焰状态(Error12)
 *         - 烟温(Error16)
 *         - 压力(Error2)
 */
void Auto_Check_Fun(void);

/**
 * @brief  待机状态检测
 * @param  无
 * @retval 0
 * @note   检测待机时火焰自检异常(Error7)
 */
uint8 Idel_Check_Fun(void);

/**
 * @brief  异常状态检测
 * @param  无
 * @retval 无
 * @note   检测燃气压力、烟温、压力
 */
void Abnormal_Check_Fun(void);

/**
 * @brief  异常事件响应处理
 * @param  无
 * @retval 无
 * @note   处理流程：
 *         - 关闭燃气、点火
 *         - 执行后吹
 *         - 补水
 *         - 自动排污(如需要)
 *         - 等待压力下降后重启
 */
void Abnormal_Events_Response(void);

/**
 * @brief  运行中故障响应
 * @param  无
 * @retval 无
 * @note   故障时停机、报警、锁定
 */
void Err_Response(void);

/**
 * @brief  待机故障响应
 * @param  无
 * @retval 无
 * @note   故障时报警、锁定
 */
void IDLE_Err_Response(void);

/**
 * @brief  LCD故障刷新（预留接口）
 * @param  无
 * @retval 无
 */
void Lcd_Err_Refresh(void);

/**
 * @brief  LCD故障读取（预留接口）
 * @param  无
 * @retval 无
 */
void Lcd_Err_Read(void);

#endif /* __ERROR_HANDLER_H */

