/**
 * @file    blowdown.c
 * @brief   排污控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @note    本文件包含6个排污控制函数
 *          从 system_control.c 中提取
 */

#include "blowdown.h"
#include "bsp_relays.h"
#include "delay.h"

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern uint8 sys_time_up;
extern uint8 sys_time_start;

/*==============================================================================
 * 函数实现 - 从 system_control.c 迁移
 *============================================================================*/

/**
 * @brief  系统待机功能
 */
void System_Idel_Function(void)
{
	/* 关闭全部输出 */
	Send_Air_Close();
	Dian_Huo_OFF();      /* 点火继电器关闭 */
	Send_Gas_Close();    /* 燃气电磁阀关闭 */
	WTS_Gas_One_Close();
	
	/* 执行自动排污 */
	Auto_Pai_Wu_Function();
}

/**
 * @brief  待机自动排污
 */
uint8 IDLE_Auto_Pai_Wu_Function(void)
{
	/* 空函数，待实现 */
	return 0;
}

/**
 * @brief  自动排污功能
 */
uint8 Auto_Pai_Wu_Function(void)
{
	static uint8 OK_Pressure = 5;
	static uint8 PaiWu_Count = 0;
	uint8 Paiwu_Times = 3;   /* 4次降压排污 */
	uint8 Time = 15;         /* 低压力排水30秒 */
	uint8 Ok_Value = 0;
	
	/* 排污状态机 */
	if(sys_flag.Paiwu_Flag)
	{
		switch (sys_flag.Pai_Wu_Idle_Index)
		{
			case 0:
				/* 打开排污门 */
				Pai_Wu_Door_Open();
				if(Temperature_Data.Pressure_Value > OK_Pressure)
				{
					delay_sys_sec(25000);
				}
				else
				{
					delay_sys_sec(35000); /* 低压延长时间 */
				}
				sys_flag.Pai_Wu_Idle_Index = 2;
				break;
			
			case 2:
				/* 检测极低水位判断是否结束 */
				if(sys_time_start == 0)
				{
					sys_time_up = 1;
				}
				
				if (IO_Status.Target.water_protect == WATER_LOSE)
				{
					sys_flag.Pai_Wu_Idle_Index = 3;
					delay_sys_sec(60000); /* 极低水位补水时间 */
					Pai_Wu_Door_Close();
				}
				
				if(sys_time_up)
				{
					sys_time_up = 0;
					sys_flag.Force_Supple_Water_Flag = FALSE;
					Pai_Wu_Door_Close();
					delay_sys_sec(60000); /* 极低水位补水时间 */
					sys_flag.Pai_Wu_Idle_Index = 3;
				}
				break;
				
			case 3:
				Pai_Wu_Door_Close();
				if(sys_time_start == 0)
				{
					sys_time_up = 1;
				}
				
				if (IO_Status.Target.water_mid == WATER_OK)
				{
					sys_flag.Pai_Wu_Idle_Index = 4;
				}
				
				if(sys_time_up)
				{
					sys_time_up = 0;
					sys_flag.Force_Supple_Water_Flag = FALSE;
					Pai_Wu_Door_Close();
					sys_flag.Pai_Wu_Idle_Index = 4;
				}
				break;
			
			case 4:
				Pai_Wu_Door_Close();
				sys_flag.Force_Supple_Water_Flag = 0;
				sys_flag.Paiwu_Flag = FALSE;
				sys_flag.Pai_Wu_Idle_Index = 0;
				Ok_Value = OK;  /* 完成自动排污程序 */
				break;
			
			default:
				sys_flag.Paiwu_Flag = FALSE;
				sys_flag.Pai_Wu_Idle_Index = 0;
				Ok_Value = OK;
				break;
		}
	}
	else
	{
		sys_flag.Pai_Wu_Idle_Index = 0;
		sys_flag.Force_Supple_Water_Flag = FALSE;
		Ok_Value = OK;
		Pai_Wu_Door_Close();
	}
	
	return Ok_Value;
}

/**
 * @brief  运行中可调排污
 */
uint8 YunXingZhong_TimeAdjustable_PaiWu_Function(void)
{
	/* 设备运行过程中使用该功能 */
	uint8 set_flag = 0;
	
	return set_flag;
}

/**
 * @brief  排污警告功能
 */
uint8 PaiWu_Warnning_Function(void)
{
	/* 排污计时：2E, 2F, 30寄存器 */
	static uint16 Max_Time = 480;    /* 最大超时8小时 */
	static uint16 Max_Value = 1439;  /* 最大显示时间为23:59 */
	static uint8 Low_Flag = 0;

	/* 运行中计时 */
	if(sys_data.Data_10H == SYS_WORK)
	{
		if(sys_flag.Paiwu_Secs >= 60)
		{
			sys_flag.Paiwu_Secs = 0;
			sys_time_inf.UnPaiwuMinutes++;
		}
	}
	else
	{
		sys_flag.Paiwu_Secs = 0;
	}
	
	/* 限制最大值 */
	if(sys_time_inf.UnPaiwuMinutes > Max_Value)
		sys_time_inf.UnPaiwuMinutes = Max_Value;

	/* 超时报警 */
	if(sys_time_inf.UnPaiwuMinutes > Max_Time)
	{
		lcd_data.Data_2EH = 0;
		lcd_data.Data_2EL = OK; /* 报警图标颜色 */
	}
	else
	{
		lcd_data.Data_2EH = 0;
		lcd_data.Data_2EL = 0;
	}

	/* 低水位复位计时 */
	if(Low_Flag == 0)
		sys_flag.Low_Count = 0;
		
	if(sys_time_inf.UnPaiwuMinutes > 1)
	{
		if (IO_Status.Target.water_protect == WATER_LOSE)
		{
			Low_Flag = OK;
			if(sys_flag.Low_Count >= 3)
			{
				Low_Flag = 0;
				sys_time_inf.UnPaiwuMinutes = 0;
				Write_Second_Flash();
			}
		}
	}

	/* 更新LCD显示 */
	lcd_data.Data_2FH = 0;
	lcd_data.Data_2FL = sys_time_inf.UnPaiwuMinutes / 60;  /* 未排污时间：小时 */
	lcd_data.Data_30H = 0;
	lcd_data.Data_30L = sys_time_inf.UnPaiwuMinutes % 60;  /* 未排污时间：分钟 */

	return 0;
}

/**
 * @brief  连续排污控制
 */
uint8 LianXu_Paiwu_Control_Function(void)
{
	uint32 Dealy_Time = 0;
	uint16 Open_Time = 0;       /* 连续排污阀实际开启时间设定 */
	uint16 Cong_Work_Time = 0;
	static uint8 Time_Ok = 0;   /* 工作时间到的标志 */
	
	/* 计算延时时间 */
	Dealy_Time = Sys_Admin.LianXu_PaiWu_DelayTime * 1 * 60; /* 0.1h * min * 60sec/min */
	Open_Time = Sys_Admin.LianXu_PaiWu_OpenSecs * 10;       /* 精确到100ms */

	/* 单机型不支持连续排污 */
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
	{
		return 0;
	}
	
	/* 手动模式不执行 */
	if(sys_data.Data_10H == 3)
		return 0;

	/* 运行状态计时 */
	if(sys_data.Data_10H == 2)
	{
		if(sys_flag.flame_state)
		{
			if(sys_flag.LianXu_1sFlag)
			{
				sys_flag.LianxuWorkTime++;
				sys_flag.LianXu_1sFlag = 0;
			}
		}
	}

	/* 检查工作时间是否达到设定值 */
	if(sys_flag.LianxuWorkTime >= Dealy_Time)
	{
		sys_flag.LianxuWorkTime = 0;
		sys_flag.Lianxu_OpenTime = 0;
		Time_Ok = OK; /* 设置连续排污标志 */
	}

	/* 执行连续排污 */
	if(Time_Ok)
	{
		if(sys_flag.Lianxu_OpenTime < Open_Time)
		{
			/* 补水泵运行时才开启排污阀 */
			if(Switch_Inf.water_switch_flag)
			{
				LianXu_Paiwu_Open();
				if(sys_flag.LianXu_100msFlag)
				{
					sys_flag.LianXu_100msFlag = 0;
					sys_flag.Lianxu_OpenTime++;
				}
			}
			else
			{
				LianXu_Paiwu_Close();
			}
		}
		else
		{
			Time_Ok = FALSE; /* 时间到标志清除 */
		}
	}
	else
	{
		sys_flag.Lianxu_OpenTime = 0;
		LianXu_Paiwu_Close();
	}

	return 0;
}

