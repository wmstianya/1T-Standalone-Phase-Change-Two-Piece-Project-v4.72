/**
 * @file    pressure_ctrl.c
 * @brief   压力控制模块
 * @author  Refactored from system_control.c
 * @date    2024-12-27
 * @version 1.0
 * 
 * @note    本文件包含3个压力控制函数，约500行代码
 *          从 system_control.c 中提取
 */

#include "pressure_ctrl.h"
#include "pwm_output.h"

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern uint8 now_percent;

/*==============================================================================
 * 函数实现 - 从 system_control.c 迁移
 *============================================================================*/

/**
 * @brief  单机压力平衡控制
 */
uint8 System_Pressure_Balance_Function(void)
{
	static uint16 Man_Set_Pressure = 0;     /* 目标压力设定值 */
	static uint8 air_min = 0;               /* 最小功率 */
	static uint8 air_max = 0;               /* 最大功率 */
	static uint16 stop_wait_pressure = 0;   /* 停炉压力阈值 */
	uint8 Tp_value = 0;                     /* 临时功率值 */

	uint16 Real_Pressure = 0;
	static uint8 Yacha_Value = 65;          /* 固定压差0.45Mpa */
	uint16 Max_Pressure = 150;              /* 最大压力1.50Mpa */

	/* 根据设备类型选择压力源 */
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
	{
		Yacha_Value = 65;
		Real_Pressure = Temperature_Data.Inside_High_Pressure;
	}
	else
	{
		Yacha_Value = 0;
		Real_Pressure = Temperature_Data.Pressure_Value;
	}

	/* 获取功率范围 */
	air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);
	air_max = Sys_Admin.Max_Work_Power;
	
	if(air_max >= 100)
		air_max = 100;
	if(air_max < 20)
		air_max = 25;

	/* 计算目标压力 */
	Man_Set_Pressure = sys_config_data.zhuan_huan_temperture_value + Yacha_Value;
	stop_wait_pressure = sys_config_data.Auto_stop_pressure + Yacha_Value;
	
	Tp_value = now_percent;

	/* 压力低于设定值 - 升功率 */
	if(Temperature_Data.Pressure_Value < sys_config_data.zhuan_huan_temperture_value)
	{
		if(Real_Pressure < Man_Set_Pressure)
		{
			/* 根据压力变化速度决定升功率速度 */
			if(sys_flag.Pressure_ChangeTime > 6)
			{
				sys_flag.get_60_percent_flag = OK;
			}
			if(sys_flag.Pressure_ChangeTime <= 5)
			{
				sys_flag.get_60_percent_flag = 0;
			}
			
			if(sys_flag.get_60_percent_flag)
			{
				if(sys_flag.Power_1_Sec)
				{
					sys_flag.Power_1_Sec = 0;
					Tp_value = Tp_value + 1;
				}
			}
			else
			{
				if(sys_flag.Power_5_Sec)
				{
					sys_flag.Power_5_Sec = 0;
					Tp_value = Tp_value + 1;
				}
			}
		}
	}

	/* 压力等于设定值 - 保持 */
	if(Real_Pressure == Man_Set_Pressure)
	{
		if(now_percent > 80)
		{
			Tp_value = 80;
		}
		sys_flag.get_60_percent_flag = 1;
	}

	/* 压力高于设定值 - 降功率 */
	if(Real_Pressure > Man_Set_Pressure || Temperature_Data.Pressure_Value > sys_config_data.zhuan_huan_temperture_value)
	{
		if(Temperature_Data.Pressure_Value > (sys_config_data.zhuan_huan_temperture_value))
		{
			if(Real_Pressure > Man_Set_Pressure)
			{
				if(now_percent > 80)
				{
					Tp_value = 70;
				}
				if(sys_flag.Power_1_Sec)
				{
					sys_flag.Power_1_Sec = 0;
					Tp_value = Tp_value - 1;
				}
			}
			else
			{
				if(Real_Pressure <= (Man_Set_Pressure - 5))
				{
					if(Temperature_Data.Pressure_Value < (sys_config_data.zhuan_huan_temperture_value + 1))
					{
						if(sys_flag.Power_1_Sec)
						{
							sys_flag.Power_1_Sec = 0;
							Tp_value = Tp_value + 1;
						}
					}
					else
					{
						if(sys_flag.Power_1_Sec)
						{
							sys_flag.Power_1_Sec = 0;
							Tp_value = Tp_value - 1;
						}
					}
				}
				else
				{
					if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value + 1))
					{
						if(sys_flag.Power_1_Sec)
						{
							sys_flag.Power_1_Sec = 0;
							Tp_value = Tp_value - 1;
						}
					}
				}
			}
		}
		else
		{
			if(Real_Pressure > Man_Set_Pressure)
			{
				if(sys_flag.Power_1_Sec)
				{
					sys_flag.Power_1_Sec = 0;
					Tp_value = Tp_value - 1;
				}
			}
		}
	}

	/* 限制功率范围 */
	now_percent = Tp_value;
	if(now_percent > air_max)
		now_percent = air_max;
	if(now_percent < air_min)
		now_percent = air_min;

	if(now_percent >= 70)
		sys_flag.get_60_percent_flag = 1;

	/* 输出PWM */
	PWM_Adjust(now_percent);

	/* 超压停炉 */
	if(Real_Pressure >= stop_wait_pressure || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
	{
		sys_data.Data_12H |= Set_Bit_4;
		Abnormal_Events.target_complete_event = 1;
	}

	return OK;
}

/**
 * @brief  相变压力平衡控制
 */
uint8 XB_System_Pressure_Balance_Function(void)
{
	static uint16 Man_Set_Pressure = 0;
	static uint8 air_min = 0;
	static uint8 air_max = 0;
	static uint16 stop_wait_pressure = 0;
	uint8 Tp_value = 0;

	uint16 Real_Pressure = 0;
	static uint8 Yacha_Value = 65;
	uint16 Max_Pressure = 150;

	/* 根据设备类型选择压力源 */
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
	{
		Yacha_Value = 65;
		Real_Pressure = Temperature_Data.Inside_High_Pressure;
	}
	else
	{
		Yacha_Value = 0;
		Real_Pressure = Temperature_Data.Pressure_Value;
	}

	/* 获取功率范围 */
	air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);
	air_max = Sys_Admin.Max_Work_Power;
	
	if(air_max >= 100)
		air_max = 100;
	if(air_max < 20)
		air_max = 25;

	/* 计算目标压力 */
	Man_Set_Pressure = sys_config_data.zhuan_huan_temperture_value + Yacha_Value;
	stop_wait_pressure = sys_config_data.Auto_stop_pressure + Yacha_Value;
	
	Tp_value = now_percent;

	/* 压力低于设定值 - 升功率 */
	if(Temperature_Data.Pressure_Value < sys_config_data.zhuan_huan_temperture_value)
	{
		if(Real_Pressure < Man_Set_Pressure)
		{
			if(sys_flag.Pressure_ChangeTime > 6)
			{
				sys_flag.get_60_percent_flag = OK;
			}
			if(sys_flag.Pressure_ChangeTime <= 5)
			{
				sys_flag.get_60_percent_flag = 0;
			}
			
			if(sys_flag.get_60_percent_flag)
			{
				if(sys_flag.Power_1_Sec)
				{
					sys_flag.Power_1_Sec = 0;
					Tp_value = Tp_value + 1;
				}
			}
			else
			{
				if(sys_flag.Power_5_Sec)
				{
					sys_flag.Power_5_Sec = 0;
					Tp_value = Tp_value + 1;
				}
			}
		}
	}

	/* 压力等于设定值 */
	if(Real_Pressure == Man_Set_Pressure)
	{
		if(now_percent > 80)
		{
			Tp_value = 80;
		}
		sys_flag.get_60_percent_flag = 1;
	}

	/* 压力高于设定值 - 降功率 */
	if(Real_Pressure > Man_Set_Pressure || Temperature_Data.Pressure_Value >= sys_config_data.zhuan_huan_temperture_value)
	{
		if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value))
		{
			if(Real_Pressure > Man_Set_Pressure)
			{
				if(now_percent > 80)
				{
					Tp_value = 70;
				}
				if(sys_flag.Power_1_Sec)
				{
					sys_flag.Power_1_Sec = 0;
					Tp_value = Tp_value - 1;
				}
			}
			else
			{
				if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value + 2))
				{
					if(sys_flag.Power_1_Sec)
					{
						sys_flag.Power_1_Sec = 0;
						Tp_value = Tp_value - 1;
					}
				}
				else
				{
					if(Real_Pressure <= (Man_Set_Pressure - 10))
					{
						if(sys_flag.Power_1_Sec)
						{
							sys_flag.Power_1_Sec = 0;
							Tp_value = Tp_value + 1;
						}
					}
				}
			}
		}
		else
		{
			if(Real_Pressure > Man_Set_Pressure)
			{
				if(sys_flag.Power_1_Sec)
				{
					sys_flag.Power_1_Sec = 0;
					Tp_value = Tp_value - 1;
				}
			}
		}
	}

	/* 限制功率范围 */
	now_percent = Tp_value;
	if(now_percent > air_max)
		now_percent = air_max;
	if(now_percent < air_min)
		now_percent = air_min;

	if(now_percent >= 70)
		sys_flag.get_60_percent_flag = 1;

	/* 输出PWM */
	PWM_Adjust(now_percent);

	/* 超压停炉 */
	if(Real_Pressure >= stop_wait_pressure || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
	{
		sys_data.Data_12H |= Set_Bit_4;
		Abnormal_Events.target_complete_event = 1;
	}

	return OK;
}

/**
 * @brief  压力变化速度检测
 */
uint8 Speed_Pressure_Function(void)
{
	static uint16 Old_Pressure = 0;
	uint16 New_Pressure = 0;
	static uint16 TimeCount = 0;
	uint8 Chazhi = 0;

	/* 根据设备类型选择压力源 */
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
	{
		New_Pressure = Temperature_Data.Inside_High_Pressure;
	}
	else
	{
		New_Pressure = Temperature_Data.Pressure_Value;
	}

	/* 每秒计算一次压力变化 */
	if(sys_flag.Pressure_1sFlag)
	{
		sys_flag.Pressure_1sFlag = 0;
		
		if(sys_flag.flame_state)
		{
			TimeCount++;
			
			/* 压力上升 */
			if(New_Pressure > Old_Pressure)
			{
				Chazhi = New_Pressure - Old_Pressure;
				Old_Pressure = New_Pressure;
				sys_flag.Pressure_ChangeTime = TimeCount;
				sys_flag.Pressure_ChangeTime = sys_flag.Pressure_ChangeTime / Chazhi;
				TimeCount = 0;
			}

			/* 压力下降 */
			if(New_Pressure < Old_Pressure)
			{
				Chazhi = Old_Pressure - New_Pressure;
				Old_Pressure = New_Pressure;
				sys_flag.Pressure_ChangeTime = TimeCount;
				sys_flag.Pressure_ChangeTime = sys_flag.Pressure_ChangeTime / Chazhi;
				TimeCount = 0;
			}
		}
		else
		{
			/* 没有火焰时状态清零 */
			Old_Pressure = New_Pressure;
			TimeCount = 0;
			sys_flag.Pressure_ChangeTime = 0;
		}
	}

	return 0;
}

