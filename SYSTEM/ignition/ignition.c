/**
 * @file    ignition.c
 * @brief   点火控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 */

#include "ignition.h"
#include "error_handler.h"
#include "bsp_relays.h"
#include "delay.h"
#include "pwm_output.h"

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern uint8 sys_time_up;
extern uint8 sys_time_start;
extern uint8 ab_index;
extern uint8 Sys_Staus;
extern uint8 Sys_Launch_Index;
extern uint8 Ignition_Index;
extern uint8 IDLE_INDEX;
extern uint8 Self_Index;
extern uint8 Air_Door_Index;

/*==============================================================================
 * 外部函数声明
 *============================================================================*/
extern void sys_close_cmd(void);
extern uint8 System_Pressure_Balance_Function(void);

/*==============================================================================
 * 函数实现
 *============================================================================*/

/**
 * @brief  点火前准备
 * @retval 1=准备完成, 0=准备中
 */
uint8 Before_Ignition_Prepare(void)
{
	uint8 func_state = 0;

	switch (sys_flag.before_ignition_index)
	{
		case 0:
			/* 开风门前扫 */
			Send_Air_Open();
			PWM_Adjust(0);
			Pai_Wu_Door_Close();
			delay_sys_sec(12000);
			
			/* 单机或双拼主机模式：检查内压 */
			if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
			{
				if(Temperature_Data.Inside_High_Pressure <= 1)
				{
					LianXu_Paiwu_Open();
				}
			}
			
			sys_flag.before_ignition_index = 1;
			sys_flag.FlameOut_Count = 0;
			sys_flag.XB_WaterLowAB_Count = 0;
			break;

		case 1:
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				sys_flag.Wts_Gas_Index = 0;
				sys_flag.before_ignition_index = 2;
				Feed_First_Level();
				
				if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
				{
					LianXu_Paiwu_Close();
				}
			}
			break;

		case 2:
			/* 单机/双拼主机：确保排污门关闭 */
			if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
			{
				LianXu_Paiwu_Close();
			}
			
			/* 压力保护检测 */
			if(Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 1))
			{
				sys_flag.Error_Code = Error2_YaBianProtect;
			}

			sys_flag.before_ignition_index = 0;
			func_state = SUCCESS;
			break;

		default:
			sys_flag.before_ignition_index = 0;
			sys_close_cmd();
			break;
	}

	return func_state;
}

/**
 * @brief  点火主流程状态机
 * @retval 1=点火成功, 0=点火中
 */
uint8 Sys_Ignition_Fun(void)
{
	sys_data.Data_12H = 0x00;
	Abnormal_Events.target_complete_event = 0;
	
	switch(Ignition_Index)
	{
		case 0: /* 初始化 */
			sys_flag.Ignition_Count = 0;
			sys_flag.FlameRecover_Time = 0;
			sys_flag.LianxuWorkTime = 0;
			WTS_Gas_One_Close();
			Send_Air_Open();
			delay_sys_sec(10000);
			Ignition_Index = 10;
			break;

		case 10: /* 前扫等待 */
			Send_Air_Open();
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				delay_sys_sec(500);
				Ignition_Index = 1;
			}
			break;

		case 1: /* 水位检查 */
			Feed_First_Level();
			
			/* 根据设备类型检查水位 */
			if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
			{
				if(IO_Status.Target.water_mid == WATER_LOSE)
					sys_flag.Force_Supple_Water_Flag = OK;
				if(IO_Status.Target.water_mid == WATER_OK)
					sys_flag.Force_Supple_Water_Flag = FALSE;
			}
			else
			{
				if(IO_Status.Target.water_shigh == WATER_LOSE)
					sys_flag.Force_Supple_Water_Flag = OK;
				if(IO_Status.Target.water_shigh == WATER_OK)
					sys_flag.Force_Supple_Water_Flag = FALSE;
			}
			
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				delay_sys_sec(Sys_Admin.First_Blow_Time);
				Ignition_Index = 20;
			}
			break;

		case 20: /* 正式前扫 */
			Send_Air_Open();
			
			/* 检查水位，关闭强制补水 */
			if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
			{
				if(IO_Status.Target.water_mid == WATER_OK)
					sys_flag.Force_Supple_Water_Flag = FALSE;
			}
			else
			{
				if(IO_Status.Target.water_shigh == WATER_OK)
					sys_flag.Force_Supple_Water_Flag = FALSE;
			}
			
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				delay_sys_sec(500);
				
				/* 水位低则延长等待 */
				if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
				{
					if(IO_Status.Target.water_mid == WATER_LOSE)
					{
						sys_flag.Force_Supple_Water_Flag = OK;
						delay_sys_sec(90000);
					}
				}
				else
				{
					if(IO_Status.Target.water_shigh == WATER_LOSE)
					{
						sys_flag.Force_Supple_Water_Flag = OK;
						delay_sys_sec(90000);
					}
				}
				
				Ignition_Index = 2;
			}
			break;

		case 2: /* 风门切换，功率调整 */
			Send_Air_Open();
			Send_Gas_Close();
			Feed_First_Level();
			
			/* 水位满则取消强制补水 */
			if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
			{
				if(IO_Status.Target.water_mid == WATER_OK)
				{
					if(sys_flag.Force_Supple_Water_Flag)
					{
						sys_flag.Force_Supple_Water_Flag = FALSE;
						sys_time_up = 1;
					}
				}
			}
			else
			{
				if(IO_Status.Target.water_shigh == WATER_OK)
				{
					sys_flag.Force_Supple_Water_Flag = FALSE;
					sys_time_up = 1;
				}
			}

			/* 压力保护 */
			if(Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 2))
			{
				sys_flag.Error_Code = Error2_YaBianProtect;
			}
	
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				
				/* 风门检测 */
				if(IO_Status.Target.Air_Door == AIR_CLOSE)
					sys_flag.Error_Code = Error9_AirPressureBad;
				
				/* 远程锁定检测 */
				if(sys_config_data.Sys_Lock_Set)
					sys_flag.Error_Code = Error9_AirPressureBad;
				
				PWM_Adjust(30);
				
				if(Sys_Admin.Fan_Speed_Check)
					delay_sys_sec(20000);
				else
					delay_sys_sec(3000);
				
				Ignition_Index = 3;
			}
			break;
			
		case 3: /* 点火准备 */
			Send_Air_Open();
			Send_Gas_Close();
			PWM_Adjust(30);
			Dian_Huo_OFF();
			sys_flag.Force_Supple_Water_Flag = FALSE;
			
			/* 缺水保护 */
			if(IO_Status.Target.water_protect == WATER_LOSE)
			{
				sys_flag.Error_Code = Error5_LowWater;
			}
			
			if(Sys_Admin.Fan_Speed_Check)
			{
				/* 风速检测模式 */
				if(sys_flag.Fan_Rpm > 200 && sys_flag.Fan_Rpm < Sys_Admin.Fan_Fire_Value)
				{
					delay_sys_sec(8000);
					Dian_Huo_Air_Level();
					Ignition_Index = 4;
				}
				
				if(sys_time_start == 0)
					sys_time_up = 1;
				
				if(sys_time_up)
				{
					sys_time_up = 0;
					sys_flag.Error_Code = Error13_AirControlFail;
				}
			}
			else
			{
				/* 无风速检测 */
				if(sys_time_start == 0)
					sys_time_up = 1;
				
				if(sys_time_up)
				{
					Dian_Huo_Air_Level();
					delay_sys_sec(8000);
					Ignition_Index = 4;
				}
			}
			break;

		case 4: /* 点火器预热 */
			Send_Air_Open();
			Send_Gas_Close();
			Dian_Huo_Air_Level();
			Dian_Huo_OFF();
	
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				Dian_Huo_Start();
				delay_sys_sec(1500);
				Ignition_Index = 5;
			}
			break;

		case 5: /* 开燃气 */
			Send_Air_Open();
			Dian_Huo_Air_Level();
		
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				WTS_Gas_One_Open();
				delay_sys_sec(3500);
				Ignition_Index = 6;
			}
			break;
				 
		case 6: /* 等待火焰 */
			Send_Air_Open();
			Dian_Huo_Air_Level();
				 
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				Send_Gas_Open();
				delay_sys_sec(4800);
				Ignition_Index = 7;
			}
			break;

		case 7: /* 火焰检测 */
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				Dian_Huo_OFF();
				WTS_Gas_One_Close();
				 
				if(sys_flag.flame_state == FLAME_OK)
				{
					/* 点火成功 */
					delay_sys_sec(Sys_Admin.Wen_Huo_Time);
					Ignition_Index = 8;
				}
				else
				{
					/* 点火失败，尝试重试 */
					sys_flag.Ignition_Count++;
					Send_Gas_Close();
					WTS_Gas_One_Close();
					Dian_Huo_OFF();
					
					if(sys_flag.Ignition_Count < Max_Ignition_Times)
					{
						Ignition_Index = 9;
						Feed_First_Level();
						delay_sys_sec(Sys_Admin.First_Blow_Time);
					}
					else
					{
						sys_flag.Error_Code = Error11_DianHuo_Bad;
						Ignition_Index = 0;
					}
				}
			}
			break;

		case 8: /* 稳火等待 */
			Dian_Huo_OFF();
			sys_flag.Force_UnSupply_Water_Flag = FALSE;
			
			/* 稳火期间火焰熄灭检测 */
			if(sys_flag.flame_state == FLAME_OUT)
			{
				sys_flag.Ignition_Count++;
				Send_Gas_Close();
				WTS_Gas_One_Close();
				Dian_Huo_OFF();
				
				if(sys_flag.Ignition_Count < Max_Ignition_Times)
				{
					Ignition_Index = 9;
					Feed_First_Level();
					delay_sys_sec(Sys_Admin.First_Blow_Time);
				}
				else
				{
					sys_flag.Error_Code = Error11_DianHuo_Bad;
					Ignition_Index = 0;
				}
			}

			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				sys_flag.Ignition_Count = 0;
				return 1; /* 点火成功 */
			}
			break;

		case 9: /* 点火重试 */
			Send_Gas_Close();
			Dian_Huo_OFF();
			WTS_Gas_One_Close();
			Feed_First_Level();
			
			if(sys_time_start == 0)
				sys_time_up = 1;
			
			if(sys_time_up)
			{
				sys_time_up = 0;
				Dian_Huo_Air_Level();
				delay_sys_sec(6000);
				Ignition_Index = 4;
			}
			break;

		default:
			sys_close_cmd();
			break;
	}

	return 0;
}

/**
 * @brief  点火过程检测
 */
void Ignition_Check_Fun(void)
{
	Get_IO_Inf();

	/* 烟温检测 */
	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
	{
		sys_flag.Error_Code = Error16_SmokeValueHigh;
	}

	/* 燃气压力检测 */
	if(IO_Status.Target.gas_low_pressure == GAS_OUT)
	{
		sys_flag.Error_Code = Error3_LowGas;
	}
}

/**
 * @brief  系统启动流程
 */
void Sys_Launch_Function(void)
{
	switch(Sys_Launch_Index)
	{
		case 0: /* 系统自检 */
			Self_Check_Function();
			
			if(Before_Ignition_Prepare())
			{
				Ignition_Index = 0;
				Sys_Launch_Index = 1;
			}
			break;
		
		case 1: /* 系统点火 */
			Ignition_Check_Fun();
			
			if(Sys_Ignition_Fun())
			{
				Sys_Launch_Index = 2;
				Ignition_Index = 0;
				delay_sys_sec(2000);
				
				/* 清除异常记录 */
				sys_data.Data_12H = 0;
				Abnormal_Events.airdoor_event = 0;
				Abnormal_Events.burner_heat_protect_count = 0;
				Abnormal_Events.flameout_event = 0;
				Abnormal_Events.overheat_event = 0;
				sys_flag.WaterUnsupply_Count = 0;
			}
			
			Self_Index = 0;
			ab_index = 0;
			Air_Door_Index = 0;
			break;
		
		case 2: /* 系统运行 */
			sys_flag.Force_Supple_Water_Flag = FALSE;
			sys_flag.Already_Work_On_Flag = OK;
					
			if(sys_data.Data_12H == 0)
			{
				Auto_Check_Fun();
				System_Pressure_Balance_Function();
				
				if(sys_flag.Paiwu_Flag)
					sys_data.Data_12H = 3;
			}
			else
			{
				Abnormal_Check_Fun();
			}

			Abnormal_Events_Response();
			break;
		
		default:
			sys_close_cmd();
			Sys_Launch_Index = 0;
			break;
	}
}

