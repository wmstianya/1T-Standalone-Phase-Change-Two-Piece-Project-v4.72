/**
 * @file    error_handler.c
 * @brief   故障检测与响应模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 */

#include "error_handler.h"

/*==============================================================================
 * 外部变量声明（来自 system_control.c）
 *============================================================================*/
extern uint8 sys_time_up;
extern uint8 sys_time_start;
extern uint8 ab_index;
extern uint8 Sys_Staus;
extern uint8 Sys_Launch_Index;
extern uint8 Ignition_Index;
extern uint8 IDLE_INDEX;
extern uint8 now_percent;

/*==============================================================================
 * 函数实现
 *============================================================================*/

/**
 * @brief  获取IO信息，检测水位逻辑、热保护、压控信号异常
 */
void Get_IO_Inf(void)
{
	uint8  Error16_Time = 8;
	uint8  Error_Buffer = 0;
	
	/* 检测水位逻辑异常 */
	Error_Buffer = FALSE;
	if (IO_Status.Target.water_high == WATER_OK)
	{
		if(IO_Status.Target.water_mid == WATER_LOSE || IO_Status.Target.water_protect == WATER_LOSE)
			Error_Buffer = OK;
	}

	if (IO_Status.Target.water_mid == WATER_OK)
	{
		if(IO_Status.Target.water_protect == WATER_LOSE)
			Error_Buffer = OK;
	}
	
	if(Error_Buffer)
	{
		if(sys_flag.flame_state)
		{
			sys_flag.Force_Supple_Water_Flag = OK;
			sys_flag.Force_Flag = OK;
		}
		else
		{
			sys_flag.Force_Supple_Water_Flag = 0;
		}
		sys_flag.Error16_Flag = OK;
	}
	else
	{
		sys_flag.Force_Flag = FALSE;
		sys_flag.Error16_Flag = 0;
		sys_flag.Error16_Count = 0;
	}
	
	/* 强制补水超时处理 */
	if(sys_flag.Force_Count >= 5)
	{
		sys_flag.Force_Supple_Water_Flag = 0;
		sys_flag.Force_Flag = FALSE;
		sys_flag.Force_Count = 0;
	}
		
	/* 设置水位逻辑异常检测时间 */
	if(sys_data.Data_10H == 0)
		Error16_Time = 5; 
	if(sys_flag.flame_state && sys_data.Data_10H == 2)
	{
		if(IO_Status.Target.water_protect)
			Error16_Time = 8;
		else
			Error16_Time = 5;
	}
	else
	{
		Error16_Time = 10;
	}
	
	/* 水位逻辑异常报警 */
	if(sys_flag.Error16_Count >= Error16_Time)
	{
		if(sys_flag.Error_Code == 0)
			sys_flag.Error_Code = Error8_WaterLogic;
		
		sys_flag.Error16_Flag = FALSE;
		sys_flag.Error16_Count = 0;
	}

	/* 热保护信号检测 */
	if(IO_Status.Target.hot_protect == THERMAL_BAD)
	{
		if(sys_flag.Error15_Flag == 0)
			sys_flag.Error15_Count = 0;
		
		sys_flag.Error15_Flag = OK;

		if(sys_flag.Error15_Count > 1)
		{
			if(sys_flag.Error_Code == 0)
				sys_flag.Error_Code = Error15_RebaoBad;
		}
	}
	else
	{
		sys_flag.Error15_Flag = 0;
		sys_flag.Error15_Count = 0;
	}

	/* 压控保护信号检测 */
	if(IO_Status.Target.hpressure_signal == PRESSURE_ERROR)
	{
		if(sys_flag.Error1_Flag == 0)
			sys_flag.Error1_Count = 0;
		
		sys_flag.Error1_Flag = OK;
		
		if(sys_flag.Error1_Count > 1)
		{
			if(sys_flag.Error_Code == 0)
				sys_flag.Error_Code = Error1_YakongProtect;
		}
	}
	else
	{
		sys_flag.Error1_Flag = 0;
		sys_flag.Error1_Count = 0;
	}
}

/**
 * @brief  系统自检功能
 */
void Self_Check_Function(void)
{
	Get_IO_Inf();

	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
	{
		sys_flag.Error_Code = Error16_SmokeValueHigh;
	}
}

/**
 * @brief  运行中自检功能
 */
void Auto_Check_Fun(void)
{
	uint8 Error_Buffer = 0;
	
	Get_IO_Inf();

	/* 风门检测 */
	if(IO_Status.Target.Air_Door == AIR_CLOSE) 
	{
		if(sys_flag.Error_Code == 0)
			sys_flag.Error_Code = Error9_AirPressureBad;
	}
	
	/* 燃气压力检测 */
	if(IO_Status.Target.gas_low_pressure == GAS_OUT)
	{
		if(sys_flag.Error_Code == 0)
			sys_flag.Error_Code = Error3_LowGas;
	}
	
	/* 极限水位检测 */
	if (IO_Status.Target.water_protect == WATER_LOSE)
	{
		Error_Buffer = OK;
	}

	if(Error_Buffer)
		sys_flag.Error5_Flag = OK;
	else
	{
		sys_flag.Error5_Flag = 0;
		sys_flag.Error5_Count = 0;
	}
		
	if(sys_flag.Error5_Count >= 5)
	{
		sys_data.Data_12H = 6;
		Abnormal_Events.target_complete_event = 1;
		sys_flag.Error5_Flag = 0;
		sys_flag.Error5_Count = 0;
	}
							
	/* 火焰检测 */
	if(sys_flag.flame_state == FLAME_OUT)
	{
		Send_Gas_Close();
		sys_flag.FlameOut_Count++;
		
		if(sys_flag.FlameOut_Count >= 3)
		{
			sys_flag.Error_Code = Error12_FlameLose;
			/* 调试日志输出 */
			#ifdef DEBUG_ENABLED
			U5_Printf("{\"sessionId\":\"debug-session\",\"runId\":\"run1-pre\",\"hypothesisId\":\"H1\",\"location\":\"error_handler.c:Auto_Check_Fun\",\"message\":\"Error12_FlameLose set\",\"data\":{\"boardAddr\":%d,\"deviceStyle\":%d,\"workState\":%d,\"flameSignal\":%d,\"flameState\":%d,\"flameFilter\":%d,\"flameOutCount\":%d},\"timestamp\":%lu}\r\n",
			          sys_flag.Address_Number,
			          (int)Sys_Admin.Device_Style,
			          (int)sys_data.Data_10H,
			          (int)IO_Status.Target.Flame_Signal,
			          (int)sys_flag.flame_state,
			          (int)sys_flag.FlameFilter,
			          (int)sys_flag.FlameOut_Count,
			          (unsigned long)sys_time_inf.sec * 1000UL);
			#endif
		}
		else
		{
			sys_data.Data_12H |= Set_Bit_5;
			Abnormal_Events.target_complete_event = 1;
		}
	}
	
	/* 火焰恢复时间计数 */
	if(sys_flag.FlameOut_Count)
	{
		if(sys_flag.FlameRecover_Time >= 1800)
			sys_flag.FlameOut_Count = 0;
	}

	/* 烟温检测 */
	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
	{
		sys_flag.Error_Code = Error16_SmokeValueHigh;
	}
	
	/* 压力检测 */
	if(Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 1))
	{
		sys_flag.Error_Code = Error2_YaBianProtect;
	}
}

/**
 * @brief  待机状态检测
 */
uint8 Idel_Check_Fun(void)
{
	if(sys_flag.Error_Code)
		return 0;

	Get_IO_Inf();

	if (IDLE_INDEX == 0)
	{
		if(sys_flag.flame_state == FLAME_OK)
		{
			if(sys_flag.Error_Code == 0)
				sys_flag.Error_Code = Error7_FlameZiJian;
		}
	}

	return 0;
}

/**
 * @brief  异常状态检测
 */
void Abnormal_Check_Fun(void)
{
	Get_IO_Inf();
	
	/* 燃气压力检测 */
	if(IO_Status.Target.gas_low_pressure == GAS_OUT)
	{
		if(sys_flag.Error_Code == 0)
			sys_flag.Error_Code = Error3_LowGas;
	}

	/* 烟温检测 */
	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
	{
		sys_flag.Error_Code = Error16_SmokeValueHigh;
	}
	
	/* 压力检测 */
	if(Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 1))
	{
		sys_flag.Error_Code = Error2_YaBianProtect;
	}
}

/**
 * @brief  异常事件响应处理
 */
void Abnormal_Events_Response(void)
{
	if (sys_data.Data_12H)
	{
		switch (ab_index)
		{
			case 0:
				Dian_Huo_OFF();
				Send_Gas_Close();
				Feed_First_Level();

				if(sys_flag.LianxuWorkTime < 600)
				{
					sys_flag.get_60_percent_flag = 0;
				}
				
				delay_sys_sec(1000);
				ab_index = 1;
				break;
				
			case 1:
				Send_Gas_Close();
				Dian_Huo_OFF();
				Feed_First_Level();

				if(sys_time_start == 0)
				{
					sys_time_up = 1;
				}
					
				if(sys_time_up)
				{
					sys_time_up = 0;
					ab_index = 2;
					delay_sys_sec(Sys_Admin.Last_Blow_Time);
				}
				break;
				
			case 2:
				Send_Gas_Close();
				Dian_Huo_OFF();
				Feed_First_Level();
		
				if(IO_Status.Target.water_shigh == WATER_LOSE)
				{
					sys_flag.Force_Supple_Water_Flag = OK;
				}
				
				if(IO_Status.Target.water_shigh == OK)
				{
					sys_flag.Force_Supple_Water_Flag = 0;
				}

				if(sys_time_start == 0)
				{
					sys_time_up = 1;
				}
					
				if(sys_time_up)
				{
					sys_time_up = 0;
					ab_index = 3;
					delay_sys_sec(1000);
					sys_flag.Force_Supple_Water_Flag = 0;
					
					if (IO_Status.Target.water_protect == WATER_LOSE)
					{
						sys_flag.Error_Code = Error5_LowWater;
					}
				}
				break;
			
			case 3:
				Dian_Huo_OFF();
				Send_Gas_Close();
				Feed_First_Level();
			
				if(sys_flag.flame_state == FLAME_OK)
				{
					sys_flag.Error_Code = Error7_FlameZiJian;
				}
				
				if(sys_time_start == 0)
				{
					sys_time_up = 1;
				}
					
				if(sys_time_up)
				{
					sys_time_up = 0;
					
					if(sys_data.Data_12H == 3)
					{
						ab_index = 10;
					}
					else
					{
						if(Temperature_Data.Pressure_Value <= sys_config_data.Auto_start_pressure || 
						   sys_data.Data_12H == 5 || sys_data.Data_12H == 6)
						{
							sys_data.Data_12H = 0;
							Abnormal_Events.target_complete_event = 0;
							memset(&Abnormal_Events, 0, sizeof(Abnormal_Events));
					 
							ab_index = 0;
							sys_data.Data_10H = SYS_WORK;
							Sys_Staus = 2;
							Sys_Launch_Index = 1;
							Ignition_Index = 0;
							Send_Air_Open();
							delay_sys_sec(1000);
						}
						else
						{
							ab_index = 4;
						}
					}
				}
				break;

			case 10:
				Send_Gas_Close();
				Send_Air_Close();
				if(Auto_Pai_Wu_Function())
				{
					ab_index = 4; 
					Abnormal_Events.target_complete_event = OK;
					sys_flag.Paiwu_Flag = 0;
				}
				break;
				
			case 4:
				Abnormal_Events.target_complete_event = OK;
				if (Abnormal_Events.target_complete_event)
				{
					Dian_Huo_OFF();
					Send_Gas_Close();
					
					if(Temperature_Data.Pressure_Value <= sys_config_data.Auto_start_pressure ||
					   sys_data.Data_12H == 5 || sys_data.Data_12H == 6)
					{
						sys_data.Data_12H = 0;
						Abnormal_Events.target_complete_event = 0;
						memset(&Abnormal_Events, 0, sizeof(Abnormal_Events));
				 
						ab_index = 0;
						sys_data.Data_10H = SYS_WORK;
						Sys_Staus = 2;
						Sys_Launch_Index = 1;
						Ignition_Index = 0;
						Send_Air_Open();
						delay_sys_sec(1000);
					}
					else
					{
						Send_Air_Close();
					}
				}
				break;
				
			default:
				sys_close_cmd();
				break;
		}
	}
	else
	{
		ab_index = 0;
	}
}

/**
 * @brief  运行中故障响应
 */
void Err_Response(void)
{
	static uint8 Old_Error = 0;
	
	/* 故障清除处理 */
	if(sys_flag.Error_Code == 0)
	{
		if(sys_flag.Lock_Error)
			sys_flag.tx_hurry_flag = 1;
			
		sys_flag.Error_Code = 0;
		sys_flag.Lock_Error = 0;
		Beep_Data.beep_start_flag = 0;
	}

	/* 故障记录超时清除 */
	if(sys_flag.Old_Error_Count >= 1800)
	{
		Old_Error = 0;
		sys_flag.Old_Error_Count = 0;
	}

	/* 首次故障处理 */
	if(sys_flag.Lock_Error == 0)
	{
		if(sys_flag.Error_Code)
		{
			sys_close_cmd();
			sys_flag.Lock_Error = 1;
			sys_flag.Alarm_Out = OK;
			Beep_Data.beep_start_flag = 1;
			
			if(sys_flag.Error_Code != Old_Error)
			{
				Old_Error = sys_flag.Error_Code;
			}

			sys_flag.Old_Error_Count = 0;
		}
	}
	else
	{
		/* 已锁定状态下的故障处理 */
		if(sys_flag.Error_Code)
		{
			if(sys_data.Data_10H == 2)
			{
				sys_close_cmd();
				sys_flag.Alarm_Out = OK;
				Beep_Data.beep_start_flag = 1;
			}
		}
	}
}

/**
 * @brief  待机故障响应
 */
void IDLE_Err_Response(void)
{
	static uint8 Old_Error = 0;
	
	/* 故障清除处理 */
	if(sys_flag.Error_Code == 0)
	{
		if(sys_flag.Lock_Error)
			sys_flag.tx_hurry_flag = 1;
		sys_flag.Error_Code = 0;
		sys_flag.Lock_Error = 0;
		Beep_Data.beep_start_flag = 0;
	}

	/* 故障记录超时清除 (30分钟) */
	if(sys_flag.Old_Error_Count >= 1800)
	{
		Old_Error = 0;
		sys_flag.Old_Error_Count = 0;
	}

	/* 首次故障处理 */
	if (sys_flag.Lock_Error == 0)
	{
		if(sys_flag.Error_Code && sys_flag.Error_Code != 0xFF)
		{
			Sys_Staus = 0;
			if(sys_data.Data_10H == 2)
				sys_close_cmd();
			
			Beep_Data.beep_start_flag = 1;
			sys_flag.Lock_Error = 1;
			sys_flag.Alarm_Out = OK;
			sys_flag.tx_hurry_flag = 1;
			
			if(sys_flag.Error_Code != Old_Error)
				Old_Error = sys_flag.Error_Code;
			
			sys_flag.Old_Error_Count = 0;
		}
	}
}

/**
 * @brief  LCD故障刷新（预留接口）
 */
void Lcd_Err_Refresh(void)
{
	/* 预留接口 */
}

/**
 * @brief  LCD故障读取（预留接口）
 */
void Lcd_Err_Read(void)
{
	/* 预留接口 */
}

