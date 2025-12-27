/**
 * @file    sys_cmd.c
 * @brief   系统启停命令模块实现
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 */

#include "sys_cmd.h"
#include "bsp_relays.h"
#include "delay.h"
#include "internal_flash.h"
#include "usart5.h"

/* 外部变量声明 */
extern sys_flags sys_flag;
extern SYS_DATA sys_data;
extern LCD_DATA lcd_data;
extern SYS_ADMIN Sys_Admin;
extern AB_EVENTS Abnormal_Events;
extern IO_STATE IO_Status;
extern SYS_TIME_INF sys_time_inf;

extern uint8 Sys_Staus;
extern uint8 Sys_Launch_Index;
extern uint8 Ignition_Index;
extern uint8 IDLE_INDEX;
extern uint16 sys_time_up;
extern uint16 sys_time_start;

/* 外部函数声明 */
extern void Last_Blow_Start_Fun(void);

/*==============================================================================
 * 函数实现
 *============================================================================*/

/**
 * @brief  系统启动命令
 */
uint8 sys_start_cmd(void)
{
	if(sys_flag.Lock_System)
	{
		/* 锁定状态，禁止启动 */
		return 0;
	}

	if(sys_flag.Error_Code)
	{
		Sys_Staus = 0;
		sys_data.Data_10H = 0x00;  /* 系统停止状态 */
		sys_data.Data_12H = 0x00;  /* 对方设备异常标志清除 */

		delay_sys_sec(100);

		IDLE_INDEX = 1;

		sys_flag.Lock_Error = 1;   /* 自故障进入锁定 */
		sys_flag.tx_hurry_flag = 1; /* 立即发送数据给控制屏 */
		Beep_Data.beep_start_flag = 1;
	}
	else
	{
		if(sys_data.Data_10H == 0)
		{
			IDLE_INDEX = 0;  /* 防止在后吹时启动 */
			Sys_Staus = 2;
			Sys_Launch_Index = 0;
			sys_flag.before_ignition_index = 0;
			Ignition_Index = 0;
			sys_time_up = 0;

			sys_data.Data_10H = 0x02;  /* 系统运行状态 */

			sys_flag.Paiwu_Flag = 0;

			sys_time_start = 0;  /* 在待机状态下，不能存在倒计时等待，防止影响系统 */

			sys_flag.Already_Work_On_Flag = FALSE;

			sys_flag.get_60_percent_flag = 0;
			sys_flag.Pai_Wu_Idle_Index = 0;

			sys_flag.before_ignition_index = 0;
			sys_flag.tx_hurry_flag = 1;  /* 立即发送数据给控制屏 */
			Dian_Huo_OFF();  /* 点火电磁阀关闭 */
		}
	}

	return 0;
}

/**
 * @brief  系统关闭命令
 */
void sys_close_cmd(void)
{
	/* 调试日志：若因火焰熄灭(12)进入停机流程，记录一次 */
	if(sys_flag.Error_Code == Error12_FlameLose)
	{
		U5_Printf("{\"sessionId\":\"debug-session\",\"runId\":\"run1-pre\",\"hypothesisId\":\"H1\",\"location\":\"sys_cmd.c:sys_close_cmd\",\"message\":\"sys_close_cmd with Error12\",\"data\":{\"boardAddr\":%d,\"deviceStyle\":%d,\"workState\":%d,\"errorCode\":%d,\"flameSignal\":%d,\"flameState\":%d},\"timestamp\":%lu}\r\n",
		          sys_flag.Address_Number,
		          (int)Sys_Admin.Device_Style,
		          (int)sys_data.Data_10H,
		          (int)sys_flag.Error_Code,
		          (int)IO_Status.Target.Flame_Signal,
		          (int)sys_flag.flame_state,
		          (unsigned long)sys_time_inf.sec * 1000UL);
	}

	sys_data.Data_10H = 0x00;  /* 系统停止状态 */

	lcd_data.Data_16H = 0X00;
	lcd_data.Data_16L = 0x00;  /* 系统刷新开关图标，显示START */

	/* 系统停止，自关键数据进行存储 */
	WTS_Gas_One_Close();

	Abnormal_Events.target_complete_event = 0;
	Dian_Huo_OFF();     /* 关闭点火电磁阀 */
	Send_Gas_Close();   /* 关闭燃气电磁阀 */

	sys_flag.get_60_percent_flag = 0;

	sys_flag.tx_hurry_flag = 1;  /* 立即发送数据给控制屏 */

	Write_Second_Flash();  /* 保存运行时间等值 */

	/* 清除上次运行中可能存在的异常状态 */
	memset(&Abnormal_Events, 0, sizeof(Abnormal_Events));

	/* 进行后吹扫计时 */
	sys_data.Data_10H = SYS_IDLE;
	Sys_Staus = 0;
	Sys_Launch_Index = 0;
	sys_flag.before_ignition_index = 0;
	Ignition_Index = 0;
	IDLE_INDEX = 1;
	Last_Blow_Start_Fun();
}

/**
 * @brief  自动启停过程功能
 */
uint8 Auto_StartOrClose_Process_Function(void)
{
	return 0;
}

