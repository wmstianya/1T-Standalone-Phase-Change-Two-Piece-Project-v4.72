/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  helic
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   led应用函数接口和蜂鸣器接口定义
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "bsp_led.h"   


SPI_InitType SPI_Gpio_Init =
{
	{GPIOE,GPIOE,GPIOE,GPIOE},
	{GPIO_Pin_8,GPIO_Pin_7,GPIO_Pin_9,GPIO_Pin_10}
};


#define SPI_CS_H GPIO_WriteBit(SPI_Gpio_Init.port[CS],SPI_Gpio_Init.pin[CS],Bit_SET)
#define SPI_CS_L GPIO_WriteBit(SPI_Gpio_Init.port[CS],SPI_Gpio_Init.pin[CS],Bit_RESET)

#define SPI_CLK_H GPIO_WriteBit(SPI_Gpio_Init.port[SCLK],SPI_Gpio_Init.pin[SCLK],Bit_SET)
#define SPI_CLK_L GPIO_WriteBit(SPI_Gpio_Init.port[SCLK],SPI_Gpio_Init.pin[SCLK],Bit_RESET)

#define SPI_DIN_H GPIO_WriteBit(SPI_Gpio_Init.port[DIN],SPI_Gpio_Init.pin[DIN],Bit_SET)
#define SPI_DIN_L GPIO_WriteBit(SPI_Gpio_Init.port[DIN],SPI_Gpio_Init.pin[DIN],Bit_RESET)

#define SPI_MISO_DOUT GPIO_ReadInputDataBit(SPI_Gpio_Init.port[DOUT],SPI_Gpio_Init.pin[DOUT])


Beep_Message Beep_Data;

//uint16 beep_time_set = 0;
//uint8  beep_start_flag = 0; //报警声音开启，滴 滴 声音
//uint8	 beep_on_flag  = 0;  //蜂鸣器响
//uint8  beep_period_flag =0;//周期标志
//uint8  beep_period_time = 200 ;//设定周期时间
uint8		re4_time		 = 0;	

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	
//===================BEEP====================================
	
		RCC_APB2PeriphClockCmd( BEEP_GPIO_CLK, ENABLE);
	 
		GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;	

		 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	 
		GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);	

//===================工作LED====================================

		
	 	RCC_APB2PeriphClockCmd( WORK_LED_GPIO_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Pin = WORK_LED_GPIO_PIN;	

		 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	 
		GPIO_Init(WORK_LED_GPIO_PORT, &GPIO_InitStructure);	
		


//===================外置看门狗 WDI 配置====================================
		RCC_APB2PeriphClockCmd( WDI_GPIO_CLK, ENABLE);
	/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = WDI_GPIO_PIN;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIO*/
		GPIO_Init(WDI_GPIO_PORT, &GPIO_InitStructure);	




//================拨码开关地址配置端口====================================
	
		RCC_APB2PeriphClockCmd(SLAVE_GPIO_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Pin =  SLAVE_PIN_2 | SLAVE_PIN_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(SLAVE_ADDRESS_PORT, &GPIO_InitStructure);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin =  SLAVE_PIN_3 | SLAVE_PIN_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA ,&GPIO_InitStructure);
	



//=================输入检测配置-------------------

		
		RCC_APB2PeriphClockCmd(INPUT_GPIO_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Pin = INPUT_PIN_1 | INPUT_PIN_2 |INPUT_PIN_3 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入模式
		GPIO_Init(INPUT_ADDRESS_PORT, &GPIO_InitStructure);

		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

		
		//IO9 和IO10 跟其它信号相反
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin = INPUT_PIN_4 | INPUT_PIN_5 |INPUT_PIN_6|INPUT_PIN_7|INPUT_PIN_8|INPUT_PIN_9|INPUT_PIN_10 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入模式
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		GPIO_InitStructure.GPIO_Pin = INPUT_PIN_11 | INPUT_PIN_12  ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入模式
		GPIO_Init(GPIOE, &GPIO_InitStructure);


	//==============风机反馈信号引脚检测，PB0             上拉输入============		

	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	

	 
}



uint8 IO_Interrupt_Config(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure ;
	  EXTI_InitTypeDef EXTI_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 // 使能PD端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
 
    // 配置PD12为输入 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 可以根据需求设置上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);
 
    // 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 
    // 配置PD12与中断线路
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);
 
    // 配置中断触发方式
   
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 根据需求配置触发方式
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
 
    // 配置NVIC
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



	/**********************PWM s上升沿检测*********************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 可以根据需求设置上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 根据需求配置触发方式
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



	return 0;
}

void  BEEP_TIME(uint16 nms)
{
		
		Beep_Data.beep_time_set = nms;//蜂鸣器时间设定
		Beep_Data.beep_on_flag = 1;//蜂鸣器开启标志

}






uint8 PortPin_Scan(GPIO_TypeDef *GPIOx,uint16 GPIO_Pin)
{
	if( GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == PIN_HIGH )
	{
		return PIN_HIGH;
	}
	else return PIN_LOW;
}


uint8 New_Flame_Scan(void)
{
	




	return 0 ;
}



void Delay_us(uint16 Value)
{
	uint8 i = 0;
  while(Value--)
	{ 
		for( i=0;i<20;i++)
		{
		   
		}
	}
}



/**
  * @brief  Delay_us(uint16_t Value) 1us
  * @param ???±us????
  * @retval int
  */
static void SPI_delay_us(uint16 Value)
{
	uint8 i=0;
  while(Value--)
	{ 
		for(i=0;i<10;i++)
		{
			 __NOP;
		}
	}
}

	

void SPI_Config_Init(void)
{
	uint8 i = 0;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  for(i=0;i<3;i++)
  {
		GPIO_InitStruct.GPIO_Pin = SPI_Gpio_Init.pin[i];
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(SPI_Gpio_Init.port[i], &GPIO_InitStruct);

  }
  for(i=3;i<4;i++)
  {
		GPIO_InitStruct.GPIO_Pin = SPI_Gpio_Init.pin[i];
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(SPI_Gpio_Init.port[i], &GPIO_InitStruct);
  }
  SPI_CS_H;
  SPI_CLK_L; 
}


void SpiCsStatus( uint8_t state)
{
  switch(state)
  {
	  case SPI_DISABLE:
			SPI_CS_H;
	  break;
	  case SPI_ENABLE:
			SPI_CS_L;
	  break;
  }
}
uint8 SPI_RW_Data_MODE0(uint8 sendData)
{
	uint8 n = 0;
	uint8 revData = 0;
	for(n=0;n<8;n++)
	{
		if(sendData&0x80)
			SPI_DIN_H;
		else 
			SPI_DIN_L;
		sendData <<= 1;
		SPI_delay_us(100);
		SPI_CLK_H;
		revData<<=1;
		if(SPI_MISO_DOUT)
			revData ++;
		SPI_delay_us(100);
		SPI_CLK_L;
	}
	return revData;
}


uint8  SpiReadData(void)
{
	uint8 ReciveData[5] = {0};
	uint8 DataCRC = 0;
	//uint8 TxData[5] = {0x68,0x00,0x64,0x01,0xCD};
	uint8 TxData[5] = {0};
	uint16 Alarm_Value = 0;
	static uint16 Alarm_Count = 0;

	static uint8 Unconnect_Count = 0;
	uint8  i=0;
	//发送: 0x68   (设定的温度 HH LL)      (使能 0x01)   (和校验取低位)

	//接收: 0x68   (实时温度 HH LL)      (使能 0x01)   (和校验取低位)
	if(sys_flag.SPI_ReadRX_Flag == FALSE) //每秒检查一次
		return 0;

	sys_flag.SPI_ReadRX_Flag = FALSE;

/******************发送数据准备阶段*******************/
	TxData[0] = 0x68; //固定头
	TxData[1] = Sys_Admin.Inside_WenDu_ProtectValue >> 8;   //设定值高位
	TxData[2] = Sys_Admin.Inside_WenDu_ProtectValue & 0x00FF;   //设定值低位
	TxData[3] = Sys_Admin.YuRe_Enabled;  //使能开关
	TxData[4] = TxData[0] + TxData[1] + TxData[2] + TxData[3];  //和校验


	if(Sys_Admin.Inside_WenDu_ProtectValue < 3)
		Sys_Admin.Inside_WenDu_ProtectValue = 200; //防止保护的值过小，做下边界处理

	Alarm_Value = Sys_Admin.Inside_WenDu_ProtectValue - 3;  //提前三度预警

	
	if(Sys_Admin.YuRe_Enabled)
		{
			if(sys_flag.Protect_WenDu >= Sys_Admin.Inside_WenDu_ProtectValue)
				{
					Alarm_Count ++;
					if(Alarm_Count >= 4)  //防止温度瞬跳，增加了滤波
						sys_flag.Error_Code = Error14_BenTiValueVBad;
				}
			else
				Alarm_Count = 0;
		}
	else
		Alarm_Count = 0;
	
	
	SpiCsStatus(SPI_ENABLE);
	for( i=0;i<5;i++)
	{
			ReciveData[i] = 0;
			
			ReciveData[i] = SPI_RW_Data_MODE0(TxData[i]);
			
	}
	DataCRC = ReciveData[0] + ReciveData[1] + ReciveData[2] + ReciveData[3]; //和校验

	
	
	if(DataCRC == ReciveData[4] && ReciveData[0] == 0x68)
		{
				//校验正确后，将数据赋值给程序
				sys_flag.Protect_WenDu = ReciveData[1]*256 + ReciveData[2];
				Unconnect_Count = 0;
				sys_flag.Find_Host = 0;
		}
	else
		{
			
			if(Sys_Admin.YuRe_Enabled)
				{
					Unconnect_Count ++;
					sys_flag.Find_Host = Unconnect_Count;
					if(Unconnect_Count > 15) //每秒一次，连续累计30次，则报警
						{
							sys_flag.Error_Code = Error17_OutWenKong_TxBad;
							sys_flag.Protect_WenDu = 0; //如果不启用，则值为零
						}
						
				}
				
		}
	

	if(sys_flag.Protect_WenDu > 390) //超过400度，则认为设备不能使用
		sys_flag.Protect_WenDu = 999;


	LCD10D.DLCD.LuNei_WenDu = sys_flag.Protect_WenDu; //给显示屏
	
	SpiCsStatus(SPI_DISABLE);


	return 0;
}







/*********************************************END OF FILE**********************/

