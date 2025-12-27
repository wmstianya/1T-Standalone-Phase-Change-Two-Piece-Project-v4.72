/**
 * @file    water_control.c
 * @brief   水位控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @note    本文件包含7个水位控制函数，约1800行代码
 *          从 system_control.c 中提取
 */

#include "water_control.h"
#include "bsp_relays.h"
#include "delay.h"

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern Logic_Water Water_State;

/*==============================================================================
 * 函数实现 - 从 system_control.c 迁移
 * 注意: 以下函数实现保持原有逻辑不变
 *============================================================================*/

/**
 * @brief  单机水位平衡控制
 */
uint8 Water_Balance_Function(void)
{
	uint8 buffer = 0;
	
	lcd_data.Data_15H = 0;
	
	/* 读取水位状态 */
	if (IO_Status.Target.water_protect == WATER_OK)
		buffer |= 0x01;
	else
		buffer &= 0x0E;
	
	if (IO_Status.Target.water_mid == WATER_OK)
		buffer |= 0x02;
	else
		buffer &= 0x0D;
	
	if (IO_Status.Target.water_high == WATER_OK)
		buffer |= 0x04;
	else
		buffer &= 0x0B;
	
	if (IO_Status.Target.water_shigh == WATER_OK)
		buffer |= 0x08;
	else
		buffer &= 0x07;

	/* 运行中高水位探针异常处理 */
	if(sys_data.Data_10H == 2)
	{
		if (IO_Status.Target.water_high == WATER_LOSE)
		{
			if (IO_Status.Target.water_shigh == WATER_OK)
				buffer &= 0x07;
		}
	}

	lcd_data.Data_15L = buffer;
	LCD10D.DLCD.Water_State = buffer;

	/* 故障状态不补水 */
	if(sys_flag.Error_Code)
	{
		Feed_Main_Pump_OFF();
		Second_Water_Valve_Close();
		return 0;
	}

	/* 手动模式不自动补水 */
	if(sys_data.Data_10H == SYS_MANUAL)
		return 0;

	/* 待机模式补水逻辑 */
	if(sys_data.Data_10H == SYS_IDLE)
	{
		if(sys_flag.last_blow_flag)
		{
			if(IO_Status.Target.water_mid == WATER_LOSE)
				sys_flag.Force_Supple_Water_Flag = OK;
			if(IO_Status.Target.water_mid == WATER_OK)
				sys_flag.Force_Supple_Water_Flag = FALSE;
		}
		else
		{
			sys_flag.Force_Supple_Water_Flag = FALSE;
		}
		
		if(sys_flag.Force_Supple_Water_Flag)
		{
			Feed_Main_Pump_ON();
			Second_Water_Valve_Open();
		}
		else
		{
			Feed_Main_Pump_OFF();
			Second_Water_Valve_Close();
		}
		return 0;
	}
	
	/* 强制补水 */
	if(sys_flag.Force_Supple_Water_Flag)
	{
		Feed_Main_Pump_ON();
		Second_Water_Valve_Open();
		return 0;
	}

	/* 正常运行补水逻辑 */
	if(sys_flag.Error_Code == 0)
	{
		/* 根据水位控制补水 */
		if(IO_Status.Target.water_mid == WATER_LOSE)
		{
			Feed_Main_Pump_ON();
			Second_Water_Valve_Open();
		}
		
		if(IO_Status.Target.water_high == WATER_OK)
		{
			Feed_Main_Pump_OFF();
			Second_Water_Valve_Close();
		}
	}
	
	return 0;
}

/**
 * @brief  特殊补水功能
 */
uint8 Special_Water_Supply_Function(void)
{
	/* 参数限制 */
	if(Sys_Admin.Special_Secs > 50)
		Sys_Admin.Special_Secs = 20;
	
	/* 故障时关闭 */
	if(sys_flag.Error_Code)
		Special_Water_OFF();

	/* 停机或异常时关闭 */
	if(sys_data.Data_10H == 0 || sys_data.Data_12H)
		Special_Water_OFF();

	/* 高水位达到时关闭 */
	if (IO_Status.Target.water_high == WATER_OK)
	{
		Special_Water_OFF();
		sys_flag.High_Lose_Flag = 0;
		sys_flag.High_Lose_Count = 0;
	}

	/* 高水位丢失延时开启 */
	if (IO_Status.Target.water_high == WATER_LOSE)
	{
		if(sys_flag.High_Lose_Flag == 0)
		{
			sys_flag.High_Lose_Flag = OK;
			sys_flag.High_Lose_Count = 0;
		}

		if(sys_flag.High_Lose_Count >= Sys_Admin.Special_Secs)
		{
			sys_flag.High_Lose_Count = Sys_Admin.Special_Secs;
			Special_Water_Open();
		}
	}

	return 0;
}

/**
 * @brief  水位变化检测
 */
uint8 WaterLevel_Unchange_Check(void)
{
	static uint8 LastState = 0;
	uint8 Water_Buffer = 0;

	/* 默认关闭检测 */
	if(Sys_Admin.WaterUnchangeMaxTime >= 250)
		return 0;

	/* 取三个水位状态 */
	Water_Buffer = lcd_data.Data_15L & 0x07;

	if(LastState != Water_Buffer)
	{
		LastState = Water_Buffer;
		sys_flag.WaterUnsupply_Count = 0;
	}

	/* 超时报警（已注释） */
	/*
	if(sys_flag.WaterUnsupply_Count >= Sys_Admin.WaterUnchangeMaxTime)
	{
		sys_flag.Error_Code = Error19_NotSupplyWater;
	}
	*/

	return 0;
}

/**
 * @brief  单机变频水泵控制
 */
uint8 Water_BianPin_Function(void)
{
	uint8 buffer = 0;
	static uint8 Old_State = 0;
	static uint8 New_Percent = 18;
	uint8 Max_Percent = 0;
	uint8 Min_Percent = 18;
	uint8 Jump_Index = 0;

	/* 参数限制 */
	if(Sys_Admin.Water_Max_Percent > 99)
		Sys_Admin.Water_Max_Percent = 99;
	if(Sys_Admin.Water_Max_Percent < 25)
		Sys_Admin.Water_Max_Percent = 25;
	
	Max_Percent = Sys_Admin.Water_Max_Percent;
	
	/* 读取水位状态 */
	lcd_data.Data_15H = 0;
	if (IO_Status.Target.water_protect == WATER_OK)
		buffer |= 0x01;
	else
		buffer &= 0x0E;
	
	if (IO_Status.Target.water_mid == WATER_OK)
		buffer |= 0x02;
	else
		buffer &= 0x0D;
	
	if (IO_Status.Target.water_high == WATER_OK)
		buffer |= 0x04;
	else
		buffer &= 0x0B;
	
	if (IO_Status.Target.water_shigh == WATER_OK)
		buffer |= 0x08;
	else
		buffer &= 0x07;

	/* 高水位异常处理 */
	if(sys_data.Data_10H == 2)
	{
		if (IO_Status.Target.water_high == WATER_LOSE)
		{
			if (IO_Status.Target.water_shigh == WATER_OK)
				buffer &= 0x07;
		}
	}

	lcd_data.Data_15L = buffer;
	LCD10D.DLCD.Water_State = buffer;

	/* 水泵控制 */
	if(sys_flag.Water_Percent > 0)
	{
		Feed_Main_Pump_ON();
		Second_Water_Valve_Open();
	}
	
	if(sys_flag.Water_Percent == 0)
	{
		Feed_Main_Pump_OFF();
		Second_Water_Valve_Close();
	}

	/* 手动模式返回 */
	if(sys_data.Data_10H == SYS_MANUAL)
		return 0;

	/* 故障状态 */
	if(sys_flag.Error_Code)
	{
		sys_flag.Water_Percent = 0;
		return 0;
	}

	/* 强制补水 */
	if(sys_flag.Force_Supple_Water_Flag)
	{
		sys_flag.Water_Percent = Max_Percent;
		return 0;
	}

	/* 待机模式 */
	if(sys_data.Data_10H == 0)
	{
		sys_flag.Water_Percent = 0;
		return 0;
	}

	/* 运行模式变频控制 */
	if(IO_Status.Target.water_protect == WATER_OK && IO_Status.Target.water_mid == WATER_LOSE)
		Jump_Index = 1;
	if(IO_Status.Target.water_mid == WATER_OK && IO_Status.Target.water_high == WATER_LOSE)
		Jump_Index = 2;
	if(IO_Status.Target.water_high == WATER_OK)
		Jump_Index = 3;

	switch(Jump_Index)
	{
		case 1: /* 缺水严重 */
			sys_flag.Water_Percent = Max_Percent;
			break;
		case 2: /* 中等缺水 */
			sys_flag.Water_Percent = (Max_Percent + Min_Percent) / 2;
			break;
		case 3: /* 水位正常 */
			sys_flag.Water_Percent = 0;
			break;
		default:
			break;
	}

	return 0;
}

/**
 * @brief  双拼水位平衡控制
 */
uint8 ShuangPin_Water_Balance_Function(void)
{
	uint8 buffer = 0;

	lcd_data.Data_15H = 0;
	
	/* 读取水位状态 */
	if (IO_Status.Target.water_protect == WATER_OK)
		buffer |= 0x01;
	else
		buffer &= 0x0E;
	
	if (IO_Status.Target.water_mid == WATER_OK)
		buffer |= 0x02;
	else
		buffer &= 0x0D;
	
	if (IO_Status.Target.water_high == WATER_OK)
		buffer |= 0x04;
	else
		buffer &= 0x0B;
	
	if (IO_Status.Target.water_shigh == WATER_OK)
		buffer |= 0x08;
	else
		buffer &= 0x07;

	/* 高水位异常处理 */
	if(sys_data.Data_10H == 2)
	{
		if (IO_Status.Target.water_high == WATER_LOSE)
		{
			if (IO_Status.Target.water_shigh == WATER_OK)
				buffer &= 0x07;
		}
	}

	lcd_data.Data_15L = buffer;
	LCD10D.DLCD.Water_State = buffer;

	/* 手动模式返回 */
	if(sys_data.Data_10H == SYS_MANUAL)
		return 0;

	/* 主机控制从机水阀 */
	if(sys_flag.Address_Number == 0)
	{
		if(Water_State.ZCommand)
			Second_Water_Valve_Open();
		else
			Second_Water_Valve_Close();
	}

	/* 故障状态 */
	if(sys_flag.Error_Code)
	{
		Water_State.ZSignal = FALSE;
		return 0;
	}

	/* 待机模式 */
	if(sys_data.Data_10H == SYS_IDLE)
	{
		if(sys_flag.last_blow_flag)
		{
			if(IO_Status.Target.water_mid == WATER_LOSE)
				sys_flag.Force_Supple_Water_Flag = OK;
			if(IO_Status.Target.water_mid == WATER_OK)
				sys_flag.Force_Supple_Water_Flag = FALSE;
		}
		else
		{
			Water_State.ZSignal = FALSE;
			sys_flag.Force_Supple_Water_Flag = FALSE;
			return 0;
		}
	}

	/* 强制补水 */
	if(sys_flag.Force_Supple_Water_Flag)
	{
		Water_State.ZSignal = OK;
		return 0;
	}

	/* 运行模式补水控制 */
	if(sys_flag.Error_Code == 0)
	{
		if(IO_Status.Target.water_mid == WATER_LOSE)
			Water_State.ZSignal = OK;
		
		if(IO_Status.Target.water_high == WATER_OK)
			Water_State.ZSignal = FALSE;
	}
	
	return 0;
}

/**
 * @brief  双水泵逻辑控制
 */
uint8 Double_WaterPump_LogicFunction(void)
{
	uint8 State_Index = 0;
	static uint16 Time_Value = 900;

	/* 手动模式 */
	if(sys_data.Data_10H == SYS_MANUAL)
	{
		if(Water_State.Zstate_Flag || Water_State.Cstate_Flag)
			Logic_Pump_On();
		else
			Logic_Pump_OFF();
		return 0;
	}

	/* 计算信号状态 */
	if(Water_State.ZSignal == FALSE && Water_State.CSignal == FALSE)
		State_Index = 0;
	if(Water_State.ZSignal == OK && Water_State.CSignal == FALSE)
		State_Index = 1;
	if(Water_State.ZSignal == FALSE && Water_State.CSignal == OK)
		State_Index = 2;
	if(Water_State.ZSignal == OK && Water_State.CSignal == OK)
		State_Index = 3;

	switch (State_Index)
	{
		case 0: /* 都不需要补水 */
			if(Switch_Inf.water_switch_flag)
			{
				Water_State.Pump_Signal = FALSE;
				Water_State.PUMP_Close_Time = 0;
			}
			Water_State.Pump_Signal = FALSE;
			
			if(Water_State.PUMP_Close_Time >= Time_Value)
			{
				Water_State.ZCommand = FALSE;
				Water_State.CCommand = FALSE;
			}
			break;
			
		case 1: /* 只有主机需要补水 */
			if(Water_State.Cstate_Flag == OK)
			{
				if(Switch_Inf.water_switch_flag)
					Water_State.PUMP_Close_Time = 0;
				Water_State.Pump_Signal = FALSE;
				if(Water_State.PUMP_Close_Time >= Time_Value)
				{
					Water_State.CCommand = FALSE;
					Water_State.ZC_Open_Time = 0;
				}
			}
			else
			{
				if(Water_State.Zstate_Flag == OK)
				{
					if(Water_State.ZC_Open_Time >= Time_Value)
						Water_State.Pump_Signal = OK;
				}
				else
				{
					Water_State.ZCommand = OK;
				}
			}
			break;
			
		case 2: /* 只有从机需要补水 */
			if(Water_State.Zstate_Flag == OK)
			{
				if(Switch_Inf.water_switch_flag)
					Water_State.PUMP_Close_Time = 0;
				Water_State.Pump_Signal = FALSE;
				if(Water_State.PUMP_Close_Time >= Time_Value)
				{
					Water_State.ZCommand = FALSE;
					Water_State.ZC_Open_Time = 0;
				}
			}
			else
			{
				if(Water_State.Cstate_Flag == OK)
				{
					if(Water_State.ZC_Open_Time >= Time_Value)
						Water_State.Pump_Signal = OK;
				}
				else
				{
					Water_State.CCommand = OK;
				}
			}
			break;
			
		case 3: /* 都需要补水 */
			Water_State.ZCommand = OK;
			Water_State.CCommand = OK;
			if(Water_State.Zstate_Flag == OK && Water_State.Cstate_Flag == OK)
			{
				if(Water_State.ZC_Open_Time >= Time_Value)
					Water_State.Pump_Signal = OK;
			}
			break;
	}

	/* 控制水泵 */
	if(Water_State.Pump_Signal)
		Logic_Pump_On();
	else
		Logic_Pump_OFF();

	return 0;
}

/**
 * @brief  双拼变频水泵控制
 */
uint8 Double_Water_BianPin_Function(void)
{
	uint8 buffer = 0;
	static uint8 Old_State = 0;
	static uint8 New_Percent = 18;
	uint8 Max_Percent = 40;
	uint8 Min_Percent = 18;
	uint8 Jump_Index = 0;

	/* 参数限制 */
	if(Sys_Admin.Water_Max_Percent > 99)
		Sys_Admin.Water_Max_Percent = 99;
	if(Sys_Admin.Water_Max_Percent < 25)
		Sys_Admin.Water_Max_Percent = 25;
	
	Max_Percent = Sys_Admin.Water_Max_Percent;
	
	/* 读取水位状态 */
	lcd_data.Data_15H = 0;
	if (IO_Status.Target.water_protect == WATER_OK)
		buffer |= 0x01;
	else
		buffer &= 0x0E;
	
	if (IO_Status.Target.water_mid == WATER_OK)
		buffer |= 0x02;
	else
		buffer &= 0x0D;
	
	if (IO_Status.Target.water_high == WATER_OK)
		buffer |= 0x04;
	else
		buffer &= 0x0B;
	
	if (IO_Status.Target.water_shigh == WATER_OK)
		buffer |= 0x08;
	else
		buffer &= 0x07;

	/* 高水位异常处理 */
	if(sys_data.Data_10H == 2)
	{
		if (IO_Status.Target.water_high == WATER_LOSE)
		{
			if (IO_Status.Target.water_shigh == WATER_OK)
				buffer &= 0x07;
		}
	}

	lcd_data.Data_15L = buffer;
	LCD10D.DLCD.Water_State = buffer;

	/* 水泵控制 */
	if(sys_flag.Water_Percent > 0)
	{
		Water_State.ZSignal = OK;
		sys_flag.WaterOut_Time = 3;
		Feed_Main_Pump_ON();
		Second_Water_Valve_Open();
	}
	
	if(sys_flag.Water_Percent == 0)
	{
		Water_State.ZSignal = FALSE;
		Feed_Main_Pump_OFF();
		Second_Water_Valve_Close();
	}

	/* 手动模式返回 */
	if(sys_data.Data_10H == SYS_MANUAL)
		return 0;

	/* 故障状态 */
	if(sys_flag.Error_Code)
	{
		Water_State.ZSignal = FALSE;
		sys_flag.Water_Percent = 0;
		return 0;
	}

	/* 待机模式 */
	if(sys_data.Data_10H == SYS_IDLE)
	{
		if(sys_flag.last_blow_flag)
		{
			if(IO_Status.Target.water_mid == WATER_LOSE)
				sys_flag.Water_Percent = 30;
			if(IO_Status.Target.water_mid == WATER_OK)
				sys_flag.Water_Percent = 0;
		}
		else
		{
			Water_State.ZSignal = FALSE;
			sys_flag.Water_Percent = 0;
		}
		return 0;
	}

	/* 强制补水 */
	if(sys_flag.Force_Supple_Water_Flag)
	{
		Water_State.ZSignal = OK;
		sys_flag.Water_Percent = Max_Percent;
		return 0;
	}

	/* 运行模式变频控制 */
	if(IO_Status.Target.water_protect == WATER_OK && IO_Status.Target.water_mid == WATER_LOSE)
		Jump_Index = 1;
	if(IO_Status.Target.water_mid == WATER_OK && IO_Status.Target.water_high == WATER_LOSE)
		Jump_Index = 2;
	if(IO_Status.Target.water_high == WATER_OK)
		Jump_Index = 3;

	switch(Jump_Index)
	{
		case 1:
			sys_flag.Water_Percent = Max_Percent;
			break;
		case 2:
			sys_flag.Water_Percent = (Max_Percent + Min_Percent) / 2;
			break;
		case 3:
			sys_flag.Water_Percent = 0;
			break;
		default:
			break;
	}

	return 0;
}

