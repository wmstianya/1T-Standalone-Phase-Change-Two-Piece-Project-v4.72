/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  helic
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   继电器应用函数接口
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
 
#include "main.h"

SWITCH_STATUS Switch_Inf; 




/**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void RELAYS_GPIO_Config(void)
{		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

	
	//1__________继电器1初始化
			RCC_APB2PeriphClockCmd( RELAY1_GPIO_CLK, ENABLE);
		/*选择要控制的GPIO引脚*/
			GPIO_InitStructure.GPIO_Pin = RELAY1_GPIO_PIN | RELAY2_GPIO_PIN | RELAY3_GPIO_PIN | RELAY5_GPIO_PIN | RELAY6_GPIO_PIN;	
			
			/*设置引脚模式为通用推挽输出*/
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	
			/*设置引脚速率为50MHz */   
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
			/*调用库函数，初始化GPIO*/
			GPIO_Init(RELAY1_GPIO_PORT, &GPIO_InitStructure);	
	
	
			RCC_APB2PeriphClockCmd( RELAY7_GPIO_CLK, ENABLE);
		
			GPIO_InitStructure.GPIO_Pin = RELAY7_GPIO_PIN | RELAY8_GPIO_PIN | RELAY9_GPIO_PIN | RELAY10_GPIO_PIN;	
	
			
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	
			
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
	
			GPIO_Init(RELAY7_GPIO_PORT, &GPIO_InitStructure);	

		
	
			RCC_APB2PeriphClockCmd( RELAY4_GPIO_CLK, ENABLE);
		
			GPIO_InitStructure.GPIO_Pin = RELAY4_GPIO_PIN ; 
	
			
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	
			
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
			
			GPIO_Init(RELAY4_GPIO_PORT, &GPIO_InitStructure);	

	
}




/*
继电器1： 补水

继电器2：  点火

继电器3：补水电磁阀

继电器4： 明火阀

继电器5： 燃气阀组

继电器6： 自动排污阀

继电器7： 风机

继电器8： 报警输出


*/


/**
  * @brief  控制继电器点火，开始
  * @param  无
  * @retval 无
  */
void	Dian_Huo_Start(void)
{
//第二二二二路继电器 =============点火继电器————————核心继电器
		
		Switch_Inf.dian_huo_flag = 1;
		 
	 	
		 
}



/**
  * @brief  控制点火继电器，关闭
  * @param  无
  * @retval 无
  */
void	Dian_Huo_OFF(void)
{		
	Switch_Inf.dian_huo_flag = 0;
	 
}

//燃气阀组开启
void 	Send_Gas_Open(void) //燃气阀组开启
{
	//若燃气阀组未开启，则开启相应的继电器
	

		Switch_Inf.gas_on_flag = 1;
		 
	
}


//燃气阀组关闭
void	Send_Gas_Close(void)//燃气阀组关闭
{

//第五五五五路继电器 =============燃气电源开关————————核心继电器	
	
		Switch_Inf.gas_on_flag = 0;
		
		
}


/**
  * @brief  控制风机电机启动， 
  * @param  无
  * @retval 无
  */
void	Send_Air_Open(void)
{
//第七七路继电器 =============风机电源开关————————核心继电器	

		
	
		
 		Switch_Inf.air_on_flag  = 1;

		
}


/**
  * @brief  风机电机关闭
  * @param  无
  * @retval 无
  */
void	Send_Air_Close(void)
{
	
			
		//关闭风机动画显示
	
		
		Switch_Inf.air_on_flag  = 0;
		
		
		PWM_Adjust(0);
		 

		now_percent = 0;
	
		
}


/**
  * @brief  控制风机吹扫和点火风速，功率50%
  * @param  无
  * @retval 无
  */

void Feed_First_Level(void)
{
		 
		
	 
		PWM_Adjust(100);
		 
		 
		now_percent = 100;
		 
		
		
	

}









uint8 Dian_Huo_Air_Level(void)
{
		 
		uint8 value = 0;

		value =  *(uint32 *)(DIAN_HUO_POWER_ADDRESS);

		if(value < Min_Dian_Huo_Power)
			value = Min_Dian_Huo_Power;

		if(value > Max_Dian_Huo_Power)
			value = Max_Dian_Huo_Power;    //TIM3->CCR1 = 500 - （value*5）

		
	 	
		PWM_Adjust(value);

		now_percent =value;
		 
		
		 
	
		 
		return OK;

}

/**
  * @brief  控制风机电机速度，
  * @param  无
  * @retval 无
  */

void Feed_test_begin(void)
{
		//测试模式中，风机100%的风量吹扫
		
		sys_data.Data_1FH = 0X64;
		

		PWM_Adjust(100);


}



 void Feed_test_over(void)
{
	 
	
	
	PWM_Adjust(0);
	now_percent = 0;
	 
	 
}



/**
  * @brief  控制给水主泵、、开 
  * @param  无
  * @retval 无
  */
uint8  Feed_Main_Pump_ON(void)
{
	//在手动模式不考虑频繁开启的问题
	if(sys_flag.WaterOut_Time <= 2)
		return 0;

	

	Switch_Inf.water_switch_flag = 1;	

	
		
	return 0;
}



uint8 Logic_Pump_On(void)
{
	//两台设备公用一个泵的处理
	//RELAY5_ON;
	
	Switch_Inf.water_switch_flag = 1;


	 
	 
	return 0;
}

uint8 Logic_Pump_OFF(void)
{
	//RELAY5_OFF;
		
		//开启水泵动画显示
			
		Switch_Inf.water_switch_flag = 0;	
	
	

		return 0;
}


/**
  * @brief  控制给水主泵、、CLOSE
  * @param  无
  * @retval 无
  */
uint8 Feed_Main_Pump_OFF(void)
{
		
		//开启水泵动画显示
		//	RELAY5_OFF;

		sys_flag.Water_Percent = 0;
		Switch_Inf.water_switch_flag = 0;
		

		return 0;
		
}


uint8 Special_Water_Open(void)
{
	
	return 0;
}



uint8 Special_Water_OFF(void)
{
	
	return 0;
}



uint8  Second_Water_Valve_Open(void)
{
	//用于补水电磁阀开启使用,电磁阀开三秒后，补水泵开
	//RELAY4_ON;
	Switch_Inf.Water_Valve_Flag = OK;
	Water_State.Zstate_Flag = OK;

	return 0;
}

uint8  Second_Water_Valve_Close(void)
{
	//用于补水电磁阀 关闭使用，补水泵关闭三秒后，电磁阀关
	

	switch (Sys_Admin.Device_Style)
		{
			case 0:
			case 1:
					if(sys_flag.WaterClose_Time > 1) 
						{
							Switch_Inf.Water_Valve_Flag = FALSE;
							Water_State.Zstate_Flag = 0;
						}

					break;
			case 2:
			case 3:
					Switch_Inf.Water_Valve_Flag = FALSE;
					Water_State.Zstate_Flag = 0;

					break;
			default:
				Sys_Admin.Device_Style = 0;

					break;
		}
		
	//if(sys_flag.WaterClose_Time <= 1)  //水泵关闭小于2秒，不能关闭补水电磁阀
	//	return 0;
		
	
	
	
	
	return 0;
}



void  Pai_Wu_Door_Open(void)
{

	
	RELAY9_ON;
	
	Switch_Inf.pai_wu_flag =1;
	
	
}


void  Pai_Wu_Door_Close(void)
{
	
	RELAY9_OFF;
	
	Switch_Inf.pai_wu_flag = 0;
	
}


void WTS_Gas_One_Open(void)
{
	//明火方式下，才会使用
	 Switch_Inf.MingHuo_Flag = OK;
	//RELAY2_ON;
}



void WTS_Gas_One_Close(void)
{
	//明火方式下，才会使用
	Switch_Inf.MingHuo_Flag = 0;
	//RELAY2_OFF;
}


uint8 LianXu_Paiwu_Open(void)
{
	//RELAY10_ON;	
	 
	Switch_Inf.LianXu_PaiWu_flag = OK;
	
	return 0;
}

uint8 LianXu_Paiwu_Close(void)
{
	//RELAY10_OFF;	

	Switch_Inf.LianXu_PaiWu_flag = FALSE;
	 
	return 0;
}


uint8 Alarm_Out_Function(void)
{

	if(Sys_Admin.Device_Style == 0 || Sys_Admin.Device_Style == 1)
		{
			if(sys_flag.Error_Code)
				{
					if(sys_flag.Sys_1_Sec)
						{
							sys_flag.Sys_1_Sec = 0;
							if(sys_flag.Alarm_Out)
								{
									RELAY1_ON;
								}
								
						}	
				}
			else
				{
					RELAY1_OFF;
				}
				
		}


	if(Sys_Admin.Device_Style == 2 || Sys_Admin.Device_Style == 3)
		{
			if(sys_flag.Error_Code || LCD10JZ[2].DLCD.Error_Code  )
				{
					if(sys_flag.Sys_1_Sec)
						{
							sys_flag.Sys_1_Sec = 0;
							if(sys_flag.Alarm_Out)
								{
									RELAY1_ON;
								}
								
						}	
				}
			else
				{
					RELAY1_OFF;
				}
				
		}
	 
	
	

	return 0;
}

uint8 Relays_NoInterrupt_ON_OFF(void)
{
	if(sys_flag.Interrupt_Alive == FALSE)
		{
			if(Switch_Inf.air_on_flag)
	  		 	{
	  		 		RELAY6_ON;  //送风电机开启
					RELAY3_ON; //直流烟道阀门开启
	  		 	}
			 else
			 	{
			 		RELAY6_OFF;  //送风电机开启
					RELAY3_OFF; //直流烟道阀门开启
			 	}

			 if(Switch_Inf.gas_on_flag)
			 	{
			 		RELAY7_ON;
			 	}
			 else
			 	{
			 		RELAY7_OFF;
			 	}

			 if(Switch_Inf.dian_huo_flag)
			 	{
			 		RELAY8_ON;
			 	}
			 else
			 	{
			 		RELAY8_OFF;
			 	}


			 if(Switch_Inf.water_switch_flag)
			 	{
			 		RELAY5_ON;
			 	}
			 else
			 	{
			 		RELAY5_OFF;
			 	}

			 if(Switch_Inf.Water_Valve_Flag)
			 	{
			 		RELAY4_ON;
			 	}
			 else
			 	{
			 		RELAY4_OFF;
			 	}

			 if(Switch_Inf.pai_wu_flag)
			 	{
			 		RELAY9_ON;
			 	}
			 else
			 	{
			 		RELAY9_OFF;
			 	}

			 if(Switch_Inf.MingHuo_Flag)
			 	{
			 		RELAY2_ON;
			 	}
			 else
			 	{
			 		RELAY2_OFF;
			 	}

			  if(Switch_Inf.LianXu_PaiWu_flag)
			 	{
			 		RELAY10_ON;
			 	}
			 else
			 	{
			 		RELAY10_OFF;
			 	}

			
		}


	return 0;
}

/*********************************************END OF FILE**********************/

void Close_All_Realys_Function(void)
{
	RELAY1_OFF;
	RELAY2_OFF;
	RELAY3_OFF;
	RELAY4_OFF;
	RELAY5_OFF;
	RELAY6_OFF;
	RELAY7_OFF;
	RELAY8_OFF;
	RELAY9_OFF;
	RELAY10_OFF;
}





