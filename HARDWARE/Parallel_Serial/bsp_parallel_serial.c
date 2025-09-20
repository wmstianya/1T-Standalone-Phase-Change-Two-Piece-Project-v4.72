/**
  ******************************************************************************
  * @file   bsp_parallel_serial.c
  * @author  helic
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   并转串（HEF4021B）应用函数接口
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "bsp_parallel_serial.h" 


struct HEF_4021B{
		uint8  Din_1[8];
		uint8  Din_2[8];
}HEF_4021B_DATA;


uint8 SER1_Data[8];
uint8 SER2_Data[8];



/**
  * @brief  初始化控制并转串的IO
  * @param  无
  * @retval 无
  */
void PARALLEL_SERIAL_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED相关的GPIO外设时钟*/
		RCC_APB2PeriphClockCmd( SERI1_GPIO_CLK | SERI2_GPIO_PIN , ENABLE);
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = SERI1_GPIO_PIN;	

	// 设置按键的引脚为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	
	/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIO*/
		GPIO_Init(SERI1_GPIO_PORT, &GPIO_InitStructure);	         //SERI1
		
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = SERI2_GPIO_PIN;                 //SERI2

		/*调用库函数，初始化GPIO*/
		GPIO_Init(SERI2_GPIO_PORT, &GPIO_InitStructure);
		
		
		
		/*开启LED相关的GPIO外设时钟*/
		RCC_APB2PeriphClockCmd( SERCP_GPIO_CLK | SERPL_GPIO_CLK , ENABLE);
		
			/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = SERCP_GPIO_PIN;                 //SERCP 

		/*设置引脚模式为通用推挽输出*/	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  

		/*调用库函数，初始化GPIO*/
		GPIO_Init(SERCP_GPIO_PORT, &GPIO_InitStructure);
		
		 	/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = SERPL_GPIO_PIN;                 //SERPL 

		/*调用库函数，初始化GPIO*/
		GPIO_Init(SERPL_GPIO_PORT, &GPIO_InitStructure);
		
			/*选择要控制的GPIO引脚*/
	 
}


void read_serial_data(void)
{
		
	uint8 Value = 0;
	/**************************输入脚1************************/ 
	IO_Status.Target.water_protect = ! PortPin_Scan(GPIOD,INPUT_PIN_1); // 输入脚1
	if(IO_Status.Target.water_protect)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,1);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,1);
	/**************************输入脚2************************/ 	
	IO_Status.Target.water_mid = !PortPin_Scan(GPIOD,INPUT_PIN_2);// 输入脚2
	if(IO_Status.Target.water_mid)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,2);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,2);
	/**************************输入脚3************************/ 	
	IO_Status.Target.water_high = !PortPin_Scan(GPIOD,INPUT_PIN_3);// 输入脚3
	
	if(IO_Status.Target.water_high)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,3);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,3);
	
	
	/**************************输入脚4************************/ 	
	IO_Status.Target.water_shigh = !PortPin_Scan(GPIOB,INPUT_PIN_4);// 输入脚4
	
	if(IO_Status.Target.water_shigh)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,4);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,4);
		
	
	/**************************输入脚5************************/ 	
	IO_Status.Target.Flame_Signal = !PortPin_Scan(GPIOB,INPUT_PIN_5);
	
	if(IO_Status.Target.Flame_Signal)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,5);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,5);
		

	/**************************输入脚6         相变上使用，作为内胆的压控保护*************************/ 	
	Value = !PortPin_Scan(GPIOB,INPUT_PIN_6);
	IO_Status.Target.XB_Hpress_Ykong = Value;
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)  //针对相变机组的类型
		{
			IO_Status.Target.XB_Hpress_Ykong = Value;
		}
	
	if(Value)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,6);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,6);
	 
	/**************************输入脚7        ***********************/ 	
	Value = !PortPin_Scan(GPIOB,INPUT_PIN_7); 
	IO_Status.Target.XB_WaterLow = Value;
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)  //针对相变机组的类型
		{
			IO_Status.Target.XB_WaterLow = Value;
		}
	
	if(Value)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,7);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,7);

	/**************************输入脚8************************/ 	
	Value = !PortPin_Scan(GPIOB,INPUT_PIN_8);// 输入脚8
	IO_Status.Target.XB_WaterHigh = Value;
	if(Value)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,8);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,8);

	//下面四个信号相反
	/**************************燃气压力信号************************/ 	
	IO_Status.Target.gas_low_pressure  = PortPin_Scan(GPIOB,INPUT_PIN_9); 
	
	if(Value)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,9);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,9);
	/**************************风压信号************************/ 	
	IO_Status.Target.Air_Door  = PortPin_Scan(GPIOB,INPUT_PIN_10); 
	
	if(IO_Status.Target.Air_Door)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,10);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,10);
	/**************************输入脚11************************/ 	
	IO_Status.Target.hot_protect = PortPin_Scan(GPIOE,INPUT_PIN_11);// 输入脚11
	
	if(IO_Status.Target.hot_protect)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,11);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,11);

	/**************************输入脚12************************/ 	
	IO_Status.Target.hpressure_signal = PortPin_Scan(GPIOE,INPUT_PIN_12);// 输入脚12	
	
	if(IO_Status.Target.hpressure_signal)
    	sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,12);
	else
		sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,12);
	


	

	if(IO_Status.Target.Flame_Signal)
			{
				//有火焰直接就传递信号
				sys_flag.flame_state = IO_Status.Target.Flame_Signal;
				sys_flag.Inputs_State = SET_BIT_N(sys_flag.Inputs_State,5);
				sys_flag.FlameFilter = 0;
			}
		else
			{
				//熄火反应进行延迟
				
				if(sys_flag.FlameFilter > 15) //连续检测到10次，每次100ms
					{
						sys_flag.flame_state = IO_Status.Target.Flame_Signal;
						sys_flag.Inputs_State = CLR_BIT_N(sys_flag.Inputs_State,5);
						sys_flag.FlameFilter = 0;
					}
				else
					{
						
					}

				//sys_flag.flame_state = IO_Status.Target.Flame_Signal;  //燃烧机测试使用
			}
			
	Switch_Inf.Flame_Flag = sys_flag.flame_state;


/**************************************************************************/
//****************使用差压液位变送器来判定液位的中高低
/**************************************************************************/

	if(Sys_Admin.ChaYa_WaterHigh_Enabled)
		{

			IO_Status.Target.water_protect = 0;
			IO_Status.Target.water_mid = 0;
			IO_Status.Target.water_high = 0;
			IO_Status.Target.water_shigh = 0;

			if(sys_flag.ChaYaWater_Value >= Sys_Admin.ChaYa_WaterLow_Set)
				{
					IO_Status.Target.water_protect = OK;
				}

			if(sys_flag.ChaYaWater_Value >= Sys_Admin.ChaYa_WaterMid_Set)
				{
					IO_Status.Target.water_protect = OK;
					IO_Status.Target.water_mid = OK;
				}

			if(sys_flag.ChaYaWater_Value >= Sys_Admin.ChaYa_WaterHigh_Set)
				{
					IO_Status.Target.water_protect = OK;
					IO_Status.Target.water_mid = OK;
					IO_Status.Target.water_high = OK;
				}

			if(sys_flag.ChaYaWater_Value >= 600)  //600mm  则认为水位有问题，需要检查
				{
					IO_Status.Target.water_protect = OK;
					IO_Status.Target.water_mid = FALSE;
					IO_Status.Target.water_high = OK;
				}

			

		
		}

	

	
}
	
	



 




