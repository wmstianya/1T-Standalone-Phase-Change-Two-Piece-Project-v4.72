
#include "main.h"




void Write_Internal_Flash(void)
{
	uint16 test_buffer = 0;
			
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY |FLASH_FLAG_OPTERR);
		//暂定数据写在第60页
			FLASH_Unlock();  //内部FLASH解锁 	
			 FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY |FLASH_FLAG_OPTERR);
			//擦除指定页
			test_buffer = FLASH_ErasePage(TARGET_HEAD_ADDRESS);
			FLASH_Lock();

			
			FLASH_Unlock(); 
			 
	
			
			FLASH_ProgramWord(ZHUAN_HUAN_TEMPERATURE,sys_config_data.zhuan_huan_temperture_value);
			FLASH_ProgramWord(AUTO_STOP_PRESSURE_ADDRESS,sys_config_data.Auto_stop_pressure);
			FLASH_ProgramWord(AUTO_START_PRESSURE_ADDRESS,sys_config_data.Auto_start_pressure);

			FLASH_ProgramWord(SYS_LOCK_SET_ADDRESS,sys_config_data.Sys_Lock_Set);//远程一键锁停控制不走CRC效验

			FLASH_ProgramWord(CHECK_WIFI_ADDRESS,sys_config_data.wifi_record);

			FLASH_ProgramWord(CHECK_FLASH_ADDRESS,FLASH_BKP_DATA);//防止第二次写入

			
			FLASH_ProgramWord(WIFI_LOCK_SET_ADDRESS,sys_flag.Wifi_Lock_System);
			FLASH_ProgramWord(WIFI_LOCK_YEAR_ADDRESS,sys_flag.wifi_Lock_Year);
			FLASH_ProgramWord(WIFI_LOCK_MONTH_ADDRESS,sys_flag.wifi_Lock_Month);
			FLASH_ProgramWord(WIFI_LOCK_DAY_ADDRESS,sys_flag.Wifi_Lock_Day);
			
			
			FLASH_Lock();

		//	__enable_irq();
	
	
	 
}


void Write_Admin_Flash(void)
{
	 FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY |FLASH_FLAG_OPTERR);
	FLASH_Unlock();  //内部FLASH解锁 	
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY |FLASH_FLAG_OPTERR);
	//擦除指定页
	FLASH_ErasePage(ADMIN_HEAD_ADDRESS);
	FLASH_Lock();


		
			
	FLASH_Unlock(); 

	FLASH_ProgramWord(FIRST_BLOW_ADDRESS,Sys_Admin.First_Blow_Time);

	FLASH_ProgramWord(LAST_BLOW_ADDRESS,Sys_Admin.Last_Blow_Time);
	FLASH_ProgramWord(WEN_HUO_ADDRESS,Sys_Admin.Wen_Huo_Time);
	FLASH_ProgramWord(STOP_TEMPERATURE,Sys_Admin.Stop_Temperature);
	FLASH_ProgramWord(DIAN_HUO_POWER_ADDRESS,Sys_Admin.Dian_Huo_Power);//对点火功率写入

	FLASH_ProgramWord(ADMIN_WORK_DAY_ADDRESS,Sys_Admin.Admin_Work_Day);
	FLASH_ProgramWord(ADMIN_SAVE_YEAR_ADDRESS,Sys_Admin.Admin_Save_Year);
	FLASH_ProgramWord(ADMIN_SAVE_MONTH_ADDRESS,Sys_Admin.Admin_Save_Month);
	FLASH_ProgramWord(ADMIN_SAVE_DAY_ADDRESS,Sys_Admin.Admin_Save_Day);

	FLASH_ProgramWord(MAX_WORK_POWER_ADDRESS,Sys_Admin.Max_Work_Power); //最大可运行功率的写入

	FLASH_ProgramWord(FAN_SPEED_CHECK_ADDRESS,Sys_Admin.Fan_Speed_Check); 
	FLASH_ProgramWord(FAN_SPEED_VALUE_ADDRESS,Sys_Admin.Fan_Speed_Value); 
	FLASH_ProgramWord(FAN_FIRE_VALUE_ADDRESS,Sys_Admin.Fan_Fire_Value); 
	FLASH_ProgramWord(DANGER_SMOKE_VALUE_ADDRESS,Sys_Admin.Danger_Smoke_Value); 
	FLASH_ProgramWord(AUTO_BLOW_SET_ADDRESS,Sys_Admin.Auto_Blow_Set); //自动排污地址保存
	FLASH_ProgramWord(FIRE_MODE_SET_ADDRESS,Sys_Admin.Fire_Mode_Set); //引火方式参数保存
	
	 

	FLASH_ProgramWord(MODBUS_FUNCTION_ADDRESS,Sys_Admin.ModBus_Function); 
	
	
	FLASH_ProgramWord(SUPPLY_MAX_TIME_ADDRESS,Sys_Admin.Supply_Max_Time);//补水超时报警值保存

	  FLASH_ProgramWord(DEVICE_MAX_PRESSURE_SET_ADDRESS,Sys_Admin.DeviceMaxPressureSet);

	  FLASH_ProgramWord(ACCESS_DATA_MODE_SET_ADDRESS,Sys_Admin.Access_Data_Mode);
	FLASH_ProgramWord(MODBUS_ADDRESS_ADDRESS,Sys_Admin.ModBus_Address);

	  
	 FLASH_ProgramWord(PAIWU_STAGE1TIME_ADDRESS,Sys_Admin.PaiwuStage1_Time);
	 FLASH_ProgramWord(PAIWU_STAGE1_OPENFLAG_ADDRESS,Sys_Admin.PaiwuStage1_OpenFlag);
	  FLASH_ProgramWord(PAIWU_STAGE2TIME_ADDRESS,Sys_Admin.PaiwuStage2_Time);
	 FLASH_ProgramWord(PAIWU_STAGE2_OPENFLAG_ADDRESS,Sys_Admin.PaiwuStage2_OpenFlag);


	 
	FLASH_ProgramWord(BALANCE_BIGPOWER_ADDRESS,Sys_Admin.Balance_Big_Power);
		FLASH_ProgramWord(BALANCE_SMALLPOWER_ADDRESS,Sys_Admin.Balance_Small_Power);
		FLASH_ProgramWord(BALANCE_BIGTIME_ADDRESS,Sys_Admin.Balance_Big_Time);
		FLASH_ProgramWord(BALANCE_SMALLTIME_ADDRESS,Sys_Admin.Balance_Small_Time);

	  FLASH_ProgramWord(FIRE_DETECT_MODE_ADDRESS,Sys_Admin.Fire_Detect_Mode);
	  	 FLASH_ProgramWord(LOGIN_CONTROL_MODE_ADDRESS,Sys_Admin.Login_Control);
		  FLASH_ProgramWord(SPECIAL_SECS_ADDRESS,Sys_Admin.Special_Secs);

		  
		 FLASH_ProgramWord(WENDU_PROTECT_ADDRESS,Sys_Admin.YuRe_Enabled);
		  FLASH_ProgramWord(BENTI_WENDU_PROTECT_ADDRESS,Sys_Admin.Inside_WenDu_ProtectValue);
		  FLASH_ProgramWord(STEAM_WENDU_PROTECT_ADDRESS,Sys_Admin.Steam_WenDu_Protect);
		 FLASH_ProgramWord(SAFETY_PROTECT_ADDRESS,Sys_Admin.SafetyProtect_Flag);

		 
		  
		  FLASH_ProgramWord(WATER_BIANPIN_ADDRESS,Sys_Admin.Water_BianPin_Enabled);
		FLASH_ProgramWord(WATER_MAXPERCENT_ADDRESS,Sys_Admin.Water_Max_Percent);

		FLASH_ProgramWord(LianXu_PaiWu_Enabled_ADDRESS,Sys_Admin.LianXu_PaiWu_Enabled);
		FLASH_ProgramWord(LianXu_PaiWu_DelayTime_ADDRESS,Sys_Admin.LianXu_PaiWu_DelayTime);
		FLASH_ProgramWord(LianXu_PaiWu_OpenSecs_ADDRESS,Sys_Admin.LianXu_PaiWu_OpenSecs);

		FLASH_ProgramWord(Device_Style_Choice_ADDRESS,Sys_Admin.Device_Style );

		FLASH_ProgramWord(FAN_PULSE_RPM_ADDRESS,Sys_Admin.Fan_Pulse_Rpm );

		FLASH_ProgramWord(CHAYA_WATER_ENABLED,Sys_Admin.ChaYa_WaterHigh_Enabled );
		FLASH_ProgramWord(CHAYA_WATERLOW_SET,Sys_Admin.ChaYa_WaterLow_Set );
		FLASH_ProgramWord(CHAYA_WATERMID_SET,Sys_Admin.ChaYa_WaterMid_Set);
		FLASH_ProgramWord(CHAYA_WATERHIGH_SET,Sys_Admin.ChaYa_WaterHigh_Set);

		  
	FLASH_Lock();
}



void Write_Second_Flash(void)
{
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY |FLASH_FLAG_OPTERR);

	FLASH_Unlock();  //内部FLASH解锁 

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY |FLASH_FLAG_OPTERR);
	//擦除第二部分
	FLASH_ErasePage(SECOND_HEAD_ADDRESS);
	FLASH_Lock();
			
	FLASH_Unlock(); 

	//系统累计时间
	FLASH_ProgramWord(SYS_WORK_TIME_ADDRESS,sys_time_inf.All_Minutes);

	FLASH_ProgramWord(A1_KEY_POWER_ADDRESS,SlaveG[1].Key_Power);

	FLASH_ProgramWord(A2_KEY_POWER_ADDRESS,SlaveG[2].Key_Power);
	FLASH_ProgramWord(A2_FIRE_POWER_ADDRESS,SlaveG[2].Fire_Power);
	FLASH_ProgramWord(A2_MAX_POWER_ADDRESS,SlaveG[2].Max_Power);
	FLASH_ProgramWord(A2_SMOKE_PROTECT_ADDRESS,SlaveG[2].Smoke_Protect);
	FLASH_ProgramWord(A2_INSIDESMOKE_PROTECT_ADDRESS,SlaveG[2].Inside_WenDu_ProtectValue);

	
	FLASH_ProgramWord(A3_KEY_POWER_ADDRESS,SlaveG[3].Key_Power);
	FLASH_ProgramWord(A3_FIRE_POWER_ADDRESS,SlaveG[3].Fire_Power);
	FLASH_ProgramWord(A3_MAX_POWER_ADDRESS,SlaveG[3].Max_Power);
	FLASH_ProgramWord(A3_SMOKE_PROTECT_ADDRESS,SlaveG[3].Smoke_Protect);
	FLASH_ProgramWord(A3_INSIDESMOKE_PROTECT_ADDRESS,SlaveG[3].Inside_WenDu_ProtectValue);

	 
	
	FLASH_Lock();
	
	
	
}








