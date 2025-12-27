/**
 * @file    sys_config.c
 * @brief   系统配置管理模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @note    本文件包含6个配置管理函数，约650行代码
 *          从 system_control.c 中提取
 */

#include "sys_config.h"
#include "bsp_relays.h"
#include "in_flash.h"

/*==============================================================================
 * 函数实现 - 从 system_control.c 迁移
 *============================================================================*/

/**
 * @brief  系统工作时间统计
 */
uint8 sys_work_time_function(void)
{
	/* 系统累计运行时间，开炉运行时间 */
	uint16 data = 0;
	static uint8 Work_State = 0;
	static uint8 Main_secs = 0;

	data = sys_time_inf.All_Minutes / 60;

	lcd_data.Data_21H = data >> 8;
	lcd_data.Data_21L = data & 0x00FF;  /* 用于界面显示 */

	if(sys_data.Data_10H == 0)
	{
		/* 上个状态时运行状态 */
		if(Work_State == 2)
		{
			/* 说明刚刚进行关机操作，需将数据进行保存 */
		}
		Work_State = sys_data.Data_10H;
	}
	else
	{
		Work_State = sys_data.Data_10H;
	}

	/* 如果在1秒时间内处于待机状态，直接退出 */
	if(sys_flag.Work_1sec_Flag == FALSE || sys_data.Data_10H == 0)
		return 0;

	sys_flag.Work_1sec_Flag = FALSE;  /* 清除标志位 */

	if(sys_data.Data_10H == 2)
	{
		if(sys_flag.flame_state)
		{
			Main_secs++;
			if(Main_secs == 60)
			{
				Main_secs = 0;
				sys_time_inf.All_Minutes++;
			}
		}
	}

	return 0;
}

/**
 * @brief  系统控制配置功能
 */
void sys_control_config_function(void)
{
	/* 配置开机系统默认参数 */
	uint16 data_temp = 0;
	uint8 temp = 0;

	data_temp = *(uint32 *)(CHECK_FLASH_ADDRESS);
	if(data_temp != FLASH_BKP_DATA)
	{
		/* 首次上电，写入默认参数 */
		for(temp = 1; temp <= 4; temp++)
		{
			SlaveG[temp].Key_Power = OK;
			SlaveG[temp].Fire_Power = 30;
			SlaveG[temp].Max_Power = 85;
			SlaveG[temp].Smoke_Protect = 85;
			SlaveG[temp].Inside_WenDu_ProtectValue = 270;
		}
		LCD10D.DLCD.YunXu_Flag = SlaveG[1].Key_Power;

		sys_flag.Wifi_Lock_System = 0;
		sys_flag.wifi_Lock_Year = 0;
		sys_flag.wifi_Lock_Month = 0;
		sys_flag.Wifi_Lock_Day = 0;

		Sys_Admin.ChaYa_WaterHigh_Enabled = 0;
		Sys_Admin.ChaYa_WaterLow_Set = 100;
		Sys_Admin.ChaYa_WaterMid_Set = 180;
		Sys_Admin.ChaYa_WaterHigh_Set = 230;

		Sys_Admin.Device_Style = 0;

		Sys_Admin.LianXu_PaiWu_DelayTime = 10;
		Sys_Admin.LianXu_PaiWu_OpenSecs = 4;

		Sys_Admin.Water_BianPin_Enabled = 0;
		Sys_Admin.Water_Max_Percent = 45;

		Sys_Admin.YuRe_Enabled = 1;
		Sys_Admin.Inside_WenDu_ProtectValue = 270;
		Sys_Admin.Steam_WenDu_Protect = 173;
		Sys_Admin.Special_Secs = 18;

		sys_time_inf.UnPaiwuMinutes = 0;

		Sys_Admin.Balance_Big_Time = 90;
		Sys_Admin.Balance_Small_Time = 150;

		Sys_Admin.DeviceMaxPressureSet = 100;

		Sys_Admin.First_Blow_Time = 30 * 1000;
		Sys_Admin.Last_Blow_Time = 30 * 1000;
		Sys_Admin.Dian_Huo_Power = 30;
		Sys_Admin.Max_Work_Power = 85;
		Sys_Admin.Wen_Huo_Time = 6 * 1000;

		Sys_Admin.Fan_Speed_Check = 1;
		Sys_Admin.Fan_Speed_Value = 4800;
		Sys_Admin.Fan_Pulse_Rpm = 3;
		Sys_Admin.Fan_Fire_Value = 3000;

		Sys_Admin.Danger_Smoke_Value = 850;
		Sys_Admin.Supply_Max_Time = 320;
		Sys_Admin.ModBus_Address = 0;

		sys_config_data.Sys_Lock_Set = 0;
		sys_config_data.Auto_stop_pressure = 60;
		sys_config_data.Auto_start_pressure = 40;
		sys_config_data.zhuan_huan_temperture_value = 50;

		Sys_Admin.Admin_Work_Day = 0;
		Sys_Admin.Admin_Save_Day = 30;
		Sys_Admin.Admin_Save_Month = 12;
		Sys_Admin.Admin_Save_Year = 2025;

		sys_flag.Lcd_First_Connect = OK;
		sys_time_inf.All_Minutes = 1;

		Write_Internal_Flash();
		Write_Admin_Flash();
		Write_Second_Flash();
	}
	else
	{
		/* 已有数据，从Flash读取 */
		Sys_Admin.Fan_Pulse_Rpm = *(uint32 *)(FAN_PULSE_RPM_ADDRESS);
		Sys_Admin.ChaYa_WaterHigh_Enabled = *(uint32 *)(CHAYA_WATER_ENABLED);
		Sys_Admin.ChaYa_WaterLow_Set = *(uint32 *)(CHAYA_WATERLOW_SET);
		Sys_Admin.ChaYa_WaterMid_Set = *(uint32 *)(CHAYA_WATERMID_SET);
		Sys_Admin.ChaYa_WaterHigh_Set = *(uint32 *)(CHAYA_WATERHIGH_SET);

		sys_flag.Wifi_Lock_System = *(uint32 *)(WIFI_LOCK_SET_ADDRESS);
		sys_flag.wifi_Lock_Year = *(uint32 *)(WIFI_LOCK_YEAR_ADDRESS);
		sys_flag.wifi_Lock_Month = *(uint32 *)(WIFI_LOCK_MONTH_ADDRESS);
		sys_flag.Wifi_Lock_Day = *(uint32 *)(WIFI_LOCK_DAY_ADDRESS);

		Sys_Admin.Device_Style = *(uint32 *)(Device_Style_Choice_ADDRESS);

		Sys_Admin.LianXu_PaiWu_Enabled = *(uint32 *)(LianXu_PaiWu_Enabled_ADDRESS);
		Sys_Admin.LianXu_PaiWu_DelayTime = *(uint32 *)(LianXu_PaiWu_DelayTime_ADDRESS);
		Sys_Admin.LianXu_PaiWu_OpenSecs = *(uint32 *)(LianXu_PaiWu_OpenSecs_ADDRESS);

		Sys_Admin.Water_BianPin_Enabled = *(uint32 *)(WATER_BIANPIN_ADDRESS);
		Sys_Admin.Water_Max_Percent = *(uint32 *)(WATER_MAXPERCENT_ADDRESS);

		Sys_Admin.YuRe_Enabled = *(uint32 *)(WENDU_PROTECT_ADDRESS);
		Sys_Admin.Inside_WenDu_ProtectValue = *(uint32 *)(BENTI_WENDU_PROTECT_ADDRESS);
		Sys_Admin.Special_Secs = *(uint32 *)(SPECIAL_SECS_ADDRESS);

		Sys_Admin.Balance_Big_Power = *(uint32 *)(BALANCE_BIGPOWER_ADDRESS);
		Sys_Admin.Balance_Small_Power = *(uint32 *)(BALANCE_SMALLPOWER_ADDRESS);
		Sys_Admin.Balance_Big_Time = *(uint32 *)(BALANCE_BIGTIME_ADDRESS);
		Sys_Admin.Balance_Small_Time = *(uint32 *)(BALANCE_SMALLTIME_ADDRESS);

		Sys_Admin.Access_Data_Mode = *(uint32 *)(ACCESS_DATA_MODE_SET_ADDRESS);
		Sys_Admin.DeviceMaxPressureSet = *(uint32 *)(DEVICE_MAX_PRESSURE_SET_ADDRESS);

		sys_config_data.Sys_Lock_Set = *(uint32 *)(SYS_LOCK_SET_ADDRESS);
		Sys_Admin.Supply_Max_Time = *(uint32 *)(SUPPLY_MAX_TIME_ADDRESS);

		Sys_Admin.First_Blow_Time = *(uint32 *)(FIRST_BLOW_ADDRESS);
		Sys_Admin.Last_Blow_Time = *(uint32 *)(LAST_BLOW_ADDRESS);
		Sys_Admin.Dian_Huo_Power = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);

		Sys_Admin.Max_Work_Power = *(uint32 *)(MAX_WORK_POWER_ADDRESS);
		Sys_Admin.Wen_Huo_Time = *(uint32 *)(WEN_HUO_ADDRESS);

		Sys_Admin.Fan_Speed_Check = *(uint32 *)(FAN_SPEED_CHECK_ADDRESS);
		Sys_Admin.Fan_Speed_Value = *(uint32 *)(FAN_SPEED_VALUE_ADDRESS);
		Sys_Admin.Fan_Fire_Value = *(uint32 *)(FAN_FIRE_VALUE_ADDRESS);

		Sys_Admin.Danger_Smoke_Value = *(uint32 *)(DANGER_SMOKE_VALUE_ADDRESS);
		Sys_Admin.ModBus_Address = *(uint32 *)(MODBUS_ADDRESS_ADDRESS);

		sys_config_data.wifi_record = *(uint32 *)(CHECK_WIFI_ADDRESS);
		sys_config_data.zhuan_huan_temperture_value = *(uint32 *)(ZHUAN_HUAN_TEMPERATURE);
		sys_config_data.Auto_stop_pressure = *(uint32 *)(AUTO_STOP_PRESSURE_ADDRESS);
		sys_config_data.Auto_start_pressure = *(uint32 *)(AUTO_START_PRESSURE_ADDRESS);

		SlaveG[1].Key_Power = *(uint32 *)(A1_KEY_POWER_ADDRESS);

		SlaveG[2].Key_Power = *(uint32 *)(A2_KEY_POWER_ADDRESS);
		SlaveG[2].Fire_Power = *(uint32 *)(A2_FIRE_POWER_ADDRESS);
		SlaveG[2].Max_Power = *(uint32 *)(A2_MAX_POWER_ADDRESS);
		SlaveG[2].Smoke_Protect = *(uint32 *)(A2_SMOKE_PROTECT_ADDRESS);
		SlaveG[2].Inside_WenDu_ProtectValue = *(uint32 *)(A2_INSIDESMOKE_PROTECT_ADDRESS);

		SlaveG[3].Key_Power = *(uint32 *)(A3_KEY_POWER_ADDRESS);
		SlaveG[3].Fire_Power = *(uint32 *)(A3_FIRE_POWER_ADDRESS);
		SlaveG[3].Max_Power = *(uint32 *)(A3_MAX_POWER_ADDRESS);
		SlaveG[3].Smoke_Protect = *(uint32 *)(A3_SMOKE_PROTECT_ADDRESS);
		SlaveG[3].Inside_WenDu_ProtectValue = *(uint32 *)(A3_INSIDESMOKE_PROTECT_ADDRESS);
	}

	LCD10D.DLCD.YunXu_Flag = SlaveG[1].Key_Power;

	Sys_Admin.Admin_Work_Day = *(uint32 *)(ADMIN_WORK_DAY_ADDRESS);
	Sys_Admin.Admin_Save_Day = *(uint32 *)(ADMIN_SAVE_DAY_ADDRESS);
	Sys_Admin.Admin_Save_Month = *(uint32 *)(ADMIN_SAVE_MONTH_ADDRESS);
	Sys_Admin.Admin_Save_Year = *(uint32 *)(ADMIN_SAVE_YEAR_ADDRESS);

	sys_time_inf.All_Minutes = *(uint32 *)(SYS_WORK_TIME_ADDRESS);
}

/**
 * @brief  配置数据检查
 */
void Check_Config_Data_Function(void)
{
	float ResData = 0;

	/* 1. 前吹扫: 30--120s */
	Sys_Admin.First_Blow_Time = *(uint32 *)(FIRST_BLOW_ADDRESS);
	if(Sys_Admin.First_Blow_Time > 300000 || Sys_Admin.First_Blow_Time < 30000)
		Sys_Admin.First_Blow_Time = 30000;

	/* 2. 后吹扫: 30--120s */
	Sys_Admin.Last_Blow_Time = *(uint32 *)(LAST_BLOW_ADDRESS);
	if(Sys_Admin.Last_Blow_Time > 300000 || Sys_Admin.Last_Blow_Time < 30000)
		Sys_Admin.Last_Blow_Time = 30000;

	/* 3. 点火率: 20--35% */
	Sys_Admin.Dian_Huo_Power = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);
	if(Sys_Admin.Dian_Huo_Power > Max_Dian_Huo_Power || Sys_Admin.Dian_Huo_Power < Min_Dian_Huo_Power)
		Sys_Admin.Dian_Huo_Power = 25;

	/* 4. 最大运行功率: 30--100% */
	if(Sys_Admin.Max_Work_Power > 100 || Sys_Admin.Max_Work_Power < 20)
		Sys_Admin.Max_Work_Power = 100;

	if(Sys_Admin.Max_Work_Power < Sys_Admin.Dian_Huo_Power)
		Sys_Admin.Max_Work_Power = Sys_Admin.Dian_Huo_Power;

	Sys_Admin.Fan_Speed_Check = *(uint32 *)(FAN_SPEED_CHECK_ADDRESS);
	if(Sys_Admin.Fan_Speed_Check > 1)
		Sys_Admin.Fan_Speed_Check = 1;

	Sys_Admin.Danger_Smoke_Value = *(uint32 *)(DANGER_SMOKE_VALUE_ADDRESS);
	if(Sys_Admin.Danger_Smoke_Value > 2000 && Sys_Admin.Danger_Smoke_Value < 600)
		Sys_Admin.Danger_Smoke_Value = 800;

	sys_config_data.zhuan_huan_temperture_value = *(uint32 *)(ZHUAN_HUAN_TEMPERATURE);
	if(sys_config_data.zhuan_huan_temperture_value < 10 || sys_config_data.zhuan_huan_temperture_value >= Sys_Admin.DeviceMaxPressureSet)
		sys_config_data.zhuan_huan_temperture_value = 55;

	if(sys_config_data.Auto_stop_pressure >= Sys_Admin.DeviceMaxPressureSet)
		sys_config_data.Auto_stop_pressure = Sys_Admin.DeviceMaxPressureSet - 5;

	Sys_Admin.DeviceMaxPressureSet = *(uint32 *)(DEVICE_MAX_PRESSURE_SET_ADDRESS);
	if(Sys_Admin.DeviceMaxPressureSet > 250)
		Sys_Admin.DeviceMaxPressureSet = 80;

	/* 设备状态同步 */
	switch(Sys_Admin.Device_Style)
	{
		case 0:
		case 1:
			if(sys_data.Data_10H == 0)
				LCD10D.DLCD.Start_Close_Flag = 0;
			if(sys_data.Data_10H == 2)
				LCD10D.DLCD.Start_Close_Flag = 1;
			break;
		case 2:
		case 3:
			LCD10D.DLCD.Start_Close_Flag = UnionD.UnionStartFlag;
			if(UnionD.UnionStartFlag == 3)
				LCD10D.DLCD.Start_Close_Flag = 0;
			break;
	}

	/* 填充LCD10D数据 */
	LCD10D.DLCD.Zhu_WaterHigh = sys_flag.ChaYaWater_Value;
	LCD10D.DLCD.Cong_WaterHigh = sys_flag.Cong_ChaYaWater_Value;
	LCD10D.DLCD.Input_Data = sys_flag.Inputs_State;
	LCD10D.DLCD.System_Lock = sys_config_data.Sys_Lock_Set;
	LCD10D.DLCD.YunXu_Flag = SlaveG[1].Key_Power;
	LCD10D.DLCD.Pump_State = Switch_Inf.Water_Valve_Flag;
	LCD10D.DLCD.Paiwu_State = Switch_Inf.pai_wu_flag;
	LCD10D.DLCD.lianxuFa_State = Switch_Inf.LianXu_PaiWu_flag;
	LCD10D.DLCD.Flame_State = sys_flag.flame_state;
	LCD10D.DLCD.Air_Speed = sys_flag.Fan_Rpm;
	LCD10D.DLCD.Air_Power = sys_data.Data_1FH;

	LCD10D.DLCD.Target_Value = (float)sys_config_data.zhuan_huan_temperture_value / 100;
	LCD10D.DLCD.Stop_Value = (float)sys_config_data.Auto_stop_pressure / 100;
	LCD10D.DLCD.Start_Value = (float)sys_config_data.Auto_start_pressure / 100;
	LCD10D.DLCD.Max_Pressure = (float)Sys_Admin.DeviceMaxPressureSet / 100;

	LCD10D.DLCD.First_Blow_Time = Sys_Admin.First_Blow_Time / 1000;
	LCD10D.DLCD.Last_Blow_Time = Sys_Admin.Last_Blow_Time / 1000;
	LCD10D.DLCD.Dian_Huo_Power = Sys_Admin.Dian_Huo_Power;
	LCD10D.DLCD.Max_Work_Power = Sys_Admin.Max_Work_Power;
	LCD10D.DLCD.Danger_Smoke_Value = Sys_Admin.Danger_Smoke_Value / 10;

	LCD10D.DLCD.Fan_Speed_Check = Sys_Admin.Fan_Speed_Check;
	LCD10D.DLCD.Fan_Fire_Value = Sys_Admin.Fan_Fire_Value;
	LCD10D.DLCD.Fan_Pulse_Rpm = Sys_Admin.Fan_Pulse_Rpm;

	LCD10D.DLCD.Error_Code = sys_flag.Error_Code;
	LCD10D.DLCD.Paiwu_Flag = sys_flag.Paiwu_Flag;

	LCD10D.DLCD.Air_State = Switch_Inf.air_on_flag;
	LCD10D.DLCD.lianxuFa_State = Switch_Inf.LianXu_PaiWu_flag;

	LCD10D.DLCD.Water_BianPin_Enabled = Sys_Admin.Water_BianPin_Enabled;
	LCD10D.DLCD.Water_Max_Percent = Sys_Admin.Water_Max_Percent;

	LCD10D.DLCD.YuRe_Enabled = Sys_Admin.YuRe_Enabled;
	LCD10D.DLCD.Inside_WenDu_ProtectValue = Sys_Admin.Inside_WenDu_ProtectValue;

	LCD10D.DLCD.LianXu_PaiWu_DelayTime = Sys_Admin.LianXu_PaiWu_DelayTime;
	LCD10D.DLCD.LianXu_PaiWu_OpenSecs = Sys_Admin.LianXu_PaiWu_OpenSecs;
	LCD10D.DLCD.ModBus_Address = Sys_Admin.ModBus_Address;

	LCD10D.DLCD.Balance_Big_Time = Sys_Admin.Balance_Big_Time;
	LCD10D.DLCD.Balance_Small_Time = Sys_Admin.Balance_Small_Time;

	LCD10D.DLCD.System_Version = Soft_Version;
	LCD10D.DLCD.Device_Style = Sys_Admin.Device_Style;

	ResData = Sys_Admin.DeviceMaxPressureSet;
	LCD10D.DLCD.Max_Pressure = ResData / 100;

	LCD10JZ[2].DLCD.YunXu_Flag = SlaveG[2].Key_Power;
	LCD10JZ[1].DLCD.YunXu_Flag = SlaveG[1].Key_Power;
}

/**
 * @brief  管理员工作时间功能
 */
uint8 Admin_Work_Time_Function(void)
{
	uint16 Now_Year = 0;
	uint16 Now_Month = 0;
	uint16 Now_Day = 0;

	uint16 Set_Year = 0;
	uint16 Set_Month = 0;
	uint16 Set_Day = 0;

	uint8 Set_Function = 0;

	Set_Function = *(uint32 *)(ADMIN_WORK_DAY_ADDRESS);

	sys_flag.Lock_System = 0;
	if(Set_Function == FALSE)
		return 0;

	Now_Year = systmtime.tm_year;
	Now_Month = systmtime.tm_mon;
	Now_Day = systmtime.tm_mday;

	Set_Year = *(uint32 *)(ADMIN_SAVE_YEAR_ADDRESS);
	Set_Month = *(uint32 *)(ADMIN_SAVE_MONTH_ADDRESS);
	Set_Day = *(uint32 *)(ADMIN_SAVE_DAY_ADDRESS);

	if(Now_Year > Set_Year)
	{
		sys_flag.Lock_System = OK;
		return 0;
	}

	if(Now_Year == Set_Year)
	{
		if(Now_Month > Set_Month)
		{
			sys_flag.Lock_System = OK;
			return 0;
		}

		if(Now_Month == Set_Month)
		{
			if(Now_Day >= Set_Day)
				sys_flag.Lock_System = OK;
		}
	}

	return 0;
}

/**
 * @brief  WiFi锁定时间功能
 */
uint8 Wifi_Lock_Time_Function(void)
{
	Date Now;
	Date Set;

	Now.iYear = LCD10D.DLCD.Year;
	Now.iMonth = LCD10D.DLCD.Month;
	Now.iDay = LCD10D.DLCD.Day;

	Set.iYear = *(uint32 *)(WIFI_LOCK_YEAR_ADDRESS);
	Set.iMonth = *(uint32 *)(WIFI_LOCK_MONTH_ADDRESS);
	Set.iDay = *(uint32 *)(WIFI_LOCK_DAY_ADDRESS);

	if(Set.iYear == 0 || Set.iMonth == 0 || Set.iDay == 0)
		return 0;

	if(Now.iYear > Set.iYear)
		return OK;

	if(Now.iYear == Set.iYear)
	{
		if(Now.iMonth > Set.iMonth)
			return OK;

		if(Now.iMonth == Set.iMonth)
		{
			if(Now.iDay >= Set.iDay)
				return OK;
		}
	}

	return 0;
}

/**
 * @brief  相变蒸汽追加功能
 */
uint8 XiangBian_Steam_AddFunction(void)
{
	uint16 Protect_Pressure = 150;  /* 1.5Mpa */

	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
	{
		/* 运行状态压控保护 */
		if(sys_data.Data_10H == 2)
		{
			if(Temperature_Data.Inside_High_Pressure >= Protect_Pressure)
			{
				if(sys_flag.Error_Code == 0)
					sys_flag.Error_Code = Error20_XB_HighPressureYabian_Bad;
			}
		}

		/* 状态机处理 */
		switch(sys_data.Data_10H)
		{
			case 0:  /* 待机状态 */
				if(IO_Status.Target.XB_WaterLow == FALSE)
				{
					if(sys_flag.XB_WaterLow_Flag == 0)
					{
						sys_flag.XB_WaterLow_Flag = OK;
						sys_flag.XB_WaterLow_Count = 0;
					}
					if(sys_flag.XB_WaterLow_Count > 15)
					{
						/* 可选报警 */
					}
				}
				else
				{
					sys_flag.XB_WaterLow_Flag = 0;
					sys_flag.XB_WaterLow_Count = 0;
				}
				break;

			case 2:  /* 运行状态 */
				if(sys_flag.flame_state == OK)
				{
					if(IO_Status.Target.XB_WaterLow == FALSE && sys_flag.Protect_WenDu >= 200)
					{
						if(sys_flag.XB_WaterLow_Flag == 0)
						{
							sys_flag.XB_WaterLow_Flag = OK;
							sys_flag.XB_WaterLow_Count = 0;
						}

						if(sys_flag.XB_WaterLow_Count > 10)
						{
							if(sys_data.Data_12H == 0)
							{
								sys_flag.XB_WaterLowAB_Count++;
							}

							if(sys_flag.XB_WaterLowAB_Count >= 4)
							{
								sys_flag.Error_Code = Error22_XB_HighPressureWater_Low;
							}
							else
							{
								sys_data.Data_12H = 5;
								Abnormal_Events.target_complete_event = 1;
							}
						}
					}
					else
					{
						sys_flag.XB_WaterLow_Flag = 0;
						sys_flag.XB_WaterLow_Count = 0;

						if(sys_flag.XB_WaterLowAB_Count)
						{
							if(sys_flag.XB_WaterLowAB_RecoverTime >= 1800)
								sys_flag.XB_WaterLowAB_Count = 0;
						}
					}
				}
				else
				{
					if(sys_data.Data_12H == 0)
					{
						if(IO_Status.Target.XB_WaterLow == FALSE)
						{
							if(sys_flag.XB_WaterLow_Flag == 0)
							{
								sys_flag.XB_WaterLow_Flag = OK;
								sys_flag.XB_WaterLow_Count = 0;
							}

							if(sys_flag.XB_WaterLow_Count > 10)
							{
								sys_flag.Error_Code = Error22_XB_HighPressureWater_Low;
							}
						}
						else
						{
							sys_flag.XB_WaterLow_Flag = 0;
							sys_flag.XB_WaterLow_Count = 0;
						}
					}
					else
					{
						sys_flag.XB_WaterLow_Flag = 0;
						sys_flag.XB_WaterLow_Count = 0;
					}
				}
				break;

			default:
				break;
		}

		/* 压控信号检测 */
		if(IO_Status.Target.XB_Hpress_Ykong == PRESSURE_ERROR)
		{
			if(sys_flag.Error_Code == 0)
				sys_flag.Error_Code = Error21_XB_HighPressureYAKONG_Bad;
		}

		/* 内部有压力时关闭连续排污 */
		if(Temperature_Data.Inside_High_Pressure >= 2)
		{
			LianXu_Paiwu_Close();
		}
	}

	return 0;
}

