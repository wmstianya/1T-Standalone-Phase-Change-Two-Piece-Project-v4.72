/**
 * @file    sys_utils.c
 * @brief   系统工具函数模块实现
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 */

#include "sys_utils.h"
#include "bsp_relays.h"
#include "delay.h"
#include <string.h>

/*==============================================================================
 * 初始化相关函数
 *============================================================================*/

/**
 * @brief  清除结构体内存
 */
void clear_struct_memory(void)
{
    /* 对结构体变量初始化 */
    memset(&sys_data, 0, sizeof(sys_data));
    memset(&lcd_data, 0, sizeof(lcd_data));
    memset(&sys_time_inf, 0, sizeof(sys_time_inf));
    memset(&sys_config_data, 0, sizeof(sys_config_data));
    memset(&Switch_Inf, 0, sizeof(Switch_Inf));
    memset(&Abnormal_Events, 0, sizeof(Abnormal_Events));
    memset(&sys_flag, 0, sizeof(sys_flag));
    memset(&Flash_Data, 0, sizeof(Flash_Data));
    memset(&Temperature_Data, 0, sizeof(Temperature_Data));
}

/**
 * @brief  JTAG禁用
 */
void JTAG_Diable(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

/**
 * @brief  上电检查功能
 */
uint8 Power_ON_Begin_Check_Function(void)
{
    return 0;
}

/**
 * @brief  硬件保护继电器功能
 */
void HardWare_Protect_Relays_Function(void)
{
    /* 保留接口 */
}

/*==============================================================================
 * 定时检测函数
 *============================================================================*/

/**
 * @brief  1秒周期检测
 */
void One_Sec_Check(void)
{
    /* 继电器有效性验证 */
    if(sys_flag.Relays_3Secs_Flag)
    {
        sys_flag.Relays_3Secs_Flag = 0;
        /* 调试输出（已注释）*/
    }

    /* 打印调试信息 */
    if(sys_flag.two_sec_flag)
    {
        sys_flag.two_sec_flag = 0;
        /* 调试输出（已注释）*/
    }
}

/*==============================================================================
 * 后吹扫控制函数
 *============================================================================*/

/**
 * @brief  后吹扫开始
 */
void Last_Blow_Start_Fun(void)
{
    /* 确保风机已开 */
    Send_Air_Open();

    sys_flag.last_blow_flag = 1;  /* 后吹状态开始标志 */
    sys_flag.tx_hurry_flag = 1;   /* 立即发送数据给控制屏 */

    Feed_First_Level();  /* 90%的风量进行后吹 */
    
    if(sys_flag.Already_Work_On_Flag)
    {
        delay_sys_sec(Sys_Admin.Last_Blow_Time);  /* 执行后吹计时 */
    }
    else
    {
        delay_sys_sec(15000);  /* 如果没成功点火，则吹15秒 */
    }
}

/**
 * @brief  后吹扫结束
 */
void Last_Blow_End_Fun(void)
{
    /* 确保风机关闭 */
    Send_Air_Close();

    sys_flag.tx_hurry_flag = 1;   /* 立即发送数据给控制屏 */
    sys_flag.last_blow_flag = 0;  /* 后吹状态结束标志 */
}

/*==============================================================================
 * 手动模式控制函数
 *============================================================================*/

/**
 * @brief  手动继电器功能
 */
uint8 Manual_Realys_Function(void)
{
    /* 保留接口：补水超时提示，防止水泵空转等 */
    return 0;
}

/**
 * @brief  退出手动模式
 */
uint8 GetOut_Mannual_Function(void)
{
    Feed_Main_Pump_OFF();
    sys_flag.WaterClose_Time = 2;
    Second_Water_Valve_Close();
    Pai_Wu_Door_Close();
    Send_Air_Close();
    LianXu_Paiwu_Close();
    WTS_Gas_One_Close();

    return 0;
}

/*==============================================================================
 * 空函数（保留接口）
 *============================================================================*/

/**
 * @brief  字节转位
 */
uint8 byte_to_bit(void)
{
    return 0;
}

/**
 * @brief  加载LCD数据
 */
void Load_LCD_Data(void)
{
    /* 保留接口 */
}

/**
 * @brief  拷贝到LCD
 */
void copy_to_lcd(void)
{
    /* 保留接口 */
}

