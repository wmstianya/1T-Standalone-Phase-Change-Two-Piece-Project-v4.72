#include "stm32f10x.h"
#include "ADS1220.h"


ADS1220_InitType ADS1220_Init =
{
	{GPIOE,GPIOE,GPIOE,GPIOE,GPIOE},
	{GPIO_Pin_11,GPIO_Pin_12,GPIO_Pin_15,GPIO_Pin_14,GPIO_Pin_13}
};

#define ADS1220_CS_H GPIO_WriteBit(ADS1220_Init.port[ADS1220_CS],ADS1220_Init.pin[ADS1220_CS],Bit_SET)
#define ADS1220_CS_L GPIO_WriteBit(ADS1220_Init.port[ADS1220_CS],ADS1220_Init.pin[ADS1220_CS],Bit_RESET)

#define ADS1220_CLK_H GPIO_WriteBit(ADS1220_Init.port[ADS1220_SCLK],ADS1220_Init.pin[ADS1220_SCLK],Bit_SET)
#define ADS1220_CLK_L GPIO_WriteBit(ADS1220_Init.port[ADS1220_SCLK],ADS1220_Init.pin[ADS1220_SCLK],Bit_RESET)


#define ADS1220_DIN_H GPIO_WriteBit(ADS1220_Init.port[ADS1220_DIN],ADS1220_Init.pin[ADS1220_DIN],Bit_SET)
#define ADS1220_DIN_L GPIO_WriteBit(ADS1220_Init.port[ADS1220_DIN],ADS1220_Init.pin[ADS1220_DIN],Bit_RESET)


#define ADS1200_DOUT GPIO_ReadInputDataBit(GPIOE,ADS1220_Init.pin[ADS1220_DOUT])

#define ADS1200_DRDY GPIO_ReadInputDataBit(GPIOE,ADS1220_Init.pin[ADS1220_DRDY])




/**
  * @brief ADS1220Init(void) ads1220初始化
  * @param none
  * @retval none
  */
void ADS1220Init(void)
{
	uint8_t i = 0;
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  for(i=0;i<3;i++)
  {
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推拉输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		
	  GPIO_InitStructure.GPIO_Pin = ADS1220_Init.pin[i]; 
	  GPIO_Init(GPIOE, &GPIO_InitStructure);
  }
  for(i=3;i<5;i++)
  {
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //推拉输出	
	  GPIO_InitStructure.GPIO_Pin = ADS1220_Init.pin[i]; 
	  GPIO_Init(GPIOE, &GPIO_InitStructure);
  }
  ADS1220_CS_H;
  ADS1220_CLK_L;  
  return;
}
/**
  * @brief ADS1220Init(void) ads1220初始化
  * @param none
  * @retval none
  */
void ADS1220Config(void)
{
	  unsigned Temp;
	  /* AIN1 P AIN0 N 放大倍数8*/
    Temp = ADS1220_MUX_1_0 | ADS1220_GAIN_16 ;
    ADS1220WriteRegister(ADS1220_0_REGISTER, 0x01, &Temp);
    /*连续转换*/
    Temp = ADS1220_CC ;
    ADS1220WriteRegister(ADS1220_1_REGISTER, 0x01, &Temp);
    
    /*使用专用 REFP0 和 REFN0 输入选择的外部基准电压 同时抑制 50Hz 和 60Hz 500ua*/
    Temp = ADS1220_IDAC_500 | 0x50 ;
	  ADS1220WriteRegister(ADS1220_2_REGISTER, 0x01, &Temp);
    
    Temp = ADS1220_IDAC1_AIN2 | ADS1220_IDAC2_AIN3  ;
	  ADS1220WriteRegister(ADS1220_3_REGISTER, 0x01, &Temp);
       	
}

/**
  * @brief  Delay_us(uint16_t Value) 1us
  * @param 延时us函数
  * @retval int
  */
static void Delay_us(uint16_t Value)
{
	uint8_t i=0;
  while(Value--)
	{ 
		for(i=0;i<20;i++)
		{
			 __NOP;
		}
	}
}
/**
  * @brief int ADS1220WaitForDataReady(int Timeout)初始化
  * @param none
  * @retval none
  */
int ADS1220WaitForDataReady(int Timeout)
{

   return ADS1220_NO_ERROR;
}
/**
  * @brief  ADS1220CsStatus(uint8_t state);
  * @param state :ADS1220_DISABLE / ADS1220_ENABLE
  * @retval none
  */
void ADS1220CsStatus( uint8_t state)
{
  switch(state)
  {
	  case ADS1220_DISABLE:
		ADS1220_CS_H;
	  break;
	  case ADS1220_ENABLE:
		ADS1220_CS_L;
	  break;
  }
}


#if CPOL==0&&CPHA==0          //MODE   0  0   
/**
  * @brief  SPI_Send_Data(uint8_t sendData) //MODE   0  0 
  * @param  sendData :发送数据
  * @retval none
  */
void SPI_Send_Data(uint8_t sendData)
{
	uint8_t n =0 ;
	for(n=0;n<8;n++)
	{
		ADS1220_CLK_L;
		Delay_us(20);
		if(sendData & 0x80)
			ADS1220_DIN_H;
		else 
			ADS1220_DIN_L;
		Delay_us(20);
		sendData <<= 1 ;
		ADS1220_CLK_H;
		Delay_us(20);
	}
	ADS1220_CLK_L;
}

/**
  * @brief  uint8_t SPI_Receiver_Data(void)
  * @param  none
  * @retval 接收8位数据
  */
uint8_t SPI_Receiver_Data(void)
{
	uint8_t n = 0;
	uint8_t revData = 0;
	for(n=0;n<8;n++)
	{
		ADS1220_CLK_L;
		Delay_us(20);
		revData <<=1;
		if(ADS1200_DOUT)
			revData |= 0x01;
		else 
			revData &= 0xfe;
		Delay_us(20);
		ADS1220_CLK_H;
		Delay_us(20);
	}
	ADS1220_CLK_L;
	return revData;
}
#endif





#if CPOL==0&&CPHA==1           //MODE  0  1
/**
  * @brief  SPI_Send_Data(uint8_t sendData) //MODE   0 1
  * @param  sendData :发送数据
  * @retval none
  */
void SPI_Send_Data(uint8_t sendData)
{
 uint8_t n;
 ADS1220_CLK_L;
 for(n=0;n<8;n++)
 {
  ADS1220_CLK_H;
	Delay_us(5);
  if(sendData&0x80)
	  ADS1220_DIN_H;
  else 
	  ADS1220_DIN_L;
	Delay_us(5);
  sendData <<= 1;
  ADS1220_CLK_L;
	Delay_us(5);
 }
}
/**
  * @brief  uint8_t SPI_Receiver_Data(void) 模式1
  * @param  none
  * @retval 接收8位数据
  */
uint8_t SPI_Receiver_Data(void)
{
 uint8_t n = 0;
 uint8_t revData = 0;
 for(n=0;n<8;n++)
 {
  ADS1220_CLK_H;
	Delay_us(5);
   revData<<=1;
  if(ADS1200_DOUT)
	  revData |=0x01;
  else 
	  revData &=0xfe;
	Delay_us(5);
  ADS1220_CLK_L;
	Delay_us(5);
 }
  ADS1220_CLK_L;
  return revData;
}
#endif
/**********************************************
模式二           写数据
***********************************************/
#if CPOL==1&&CPHA==0           //MODE   1  0
/**
  * @brief  SPI_Send_Data(uint8_t sendData) //MODE   1 0
  * @param  sendData :发送数据
  * @retval none
  */
void SPI_Send_Data(uint8_t sendData)
{
 uint8_t n;
 for(n=0;n<8;n++)
 {
  ADS1220_CLK_H;
  Delay_us(5);
  if(sendData & 0x80)
	  ADS1220_DIN_H;
  else 
	  ADS1220_DIN_L;
  Delay_us(5);
  sendData <<=1;
  ADS1220_CLK_L;
  Delay_us(5);
 }
  ADS1220_CLK_H;
}
/**
  * @brief  uint8_t SPI_Receiver_Data(void) 模式2
  * @param  none
  * @retval 接收8位数据
  */
uint8_t SPI_Receiver_Data(void)
{
 uint8_t n = 0;
 uint8_t revData = 0;
 for(n=0;n<8;n++)
 {
  ADS1220_CLK_H;
	Delay_us(5);
  revData <<=1;
  if(ADS1200_DOUT)
	  revData|=0x01;
  else 
	  revData&=0xfe;
		
  ADS1220_CLK_L;
	Delay_us(5);
 }
  ADS1220_CLK_H;
  return revData;
}

#endif

/*********************************************
模式三        写数据
*********************************************/
#if CPOL==1&&CPHA==1            //MODE  1  1
/**
  * @brief  SPI_Send_Data(uint8_t sendData) //MODE   1 1
  * @param  sendData :发送数据
  * @retval none
  */
void SPI_Send_Data(uint8_t sendData)
{
	uint8_t n;
	
	ADS1220_CLK_H;
	for(n=0;n<8;n++)
	{
		ADS1220_CLK_L;
		Delay_us(5);
		if(sendData & 0x80)
			ADS1220_DIN_H;
		else 
			ADS1220_DIN_L;
			Delay_us(5);
		sendData <<= 1;
		ADS1220_CLK_H;
		Delay_us(5);
	}
}
/**
  * @brief  uint8_t SPI_Receiver_Data(void) 模式3
  * @param  none
  * @retval 接收8位数据
  */
uint8_t SPI_Receiver_Data(void)
{
 uint8_t n = 0;
 uint8_t revData = 0;
 ADS1220_CLK_L;
 for(n=0;n<8;n++)
 { 
	ADS1220_CLK_L;
	Delay_us(5);
	revData <<= 1;
	if(ADS1200_DOUT)
		revData |=0x01;
	else 
	  revData &=0xfe;
		Delay_us(5);
	ADS1220_CLK_H;
	Delay_us(5);
 }
	ADS1220_CLK_H;
	return revData;
}
#endif
/**
  * @brief  void ADS1220SendByte(uint8_t Byte)
  * @param  uint8_t 发送8位数据
  * @retval none
  */
void ADS1220SendByte(uint8_t Byte)
{	
	SPI_Send_Data(Byte);
}
/**
  * @brief  ADS1220ReceiveByte(void)
  * @param  none
  * @retval 接收8位数据
  */
uint8_t ADS1220ReceiveByte(void)
{
   return SPI_Receiver_Data();
}
/**
  * @brief  ADS1220ReadData(void)
  * @param  none
  * @retval 接收32位数据
  */
long ADS1220ReadData(void)
{
   long Data;
   /* assert CS to start transfer */
   ADS1220CsStatus(ADS1220_ENABLE);
   /* send the command byte */
   ADS1220SendByte(ADS1220_CMD_RDATA);
   /* get the conversion result */

   Data = ADS1220ReceiveByte();
   Data = (Data << 8) | ADS1220ReceiveByte();
   Data = (Data << 8) | ADS1220ReceiveByte();
   /* sign extend data */
   if (Data & 0x800000)
      Data |= 0xff000000; 

   /* de-assert CS */
   ADS1220CsStatus(ADS1220_DISABLE);
   return Data;
}
/**
  * @brief  void ADS1220ReadRegister(int StartAddress, int NumRegs, unsigned * pData) 读寄存器
  * @param  StartAddress:开始地址 0-3 
	* @param  NumRegs 寄存器数量最大3
  * @param  pData :要读到的数据
  * @retval 接收32位数据
  */
void ADS1220ReadRegister(int StartAddress, int NumRegs, unsigned * pData)
{
   int i;
	/* assert CS to start transfer */
	ADS1220CsStatus(ADS1220_ENABLE);
 	/* send the command byte */
	ADS1220SendByte(ADS1220_CMD_RREG | (((StartAddress<<2) & 0x0c) |((NumRegs-1)&0x03)));
   	/* get the register content */
	for (i=0; i< NumRegs; i++)
	{
		*pData++ = ADS1220ReceiveByte();
	}
   	/* de-assert CS */
	ADS1220CsStatus(ADS1220_DISABLE);
	return;
}
/**
  * @brief  void ADS1220ReadRegister(int StartAddress, int NumRegs, unsigned * pData) 写寄存器
  * @param  StartAddress:开始地址 0-3 
	* @param  NumRegs 寄存器数量最大3
  * @param  pData :要读到的数据
  * @retval 接收32位数据
  */
void ADS1220WriteRegister(int StartAddress, int NumRegs, unsigned * pData)
{
	int i;
   	/* assert CS to start transfer */
	ADS1220CsStatus(ADS1220_ENABLE);
   	/* send the command byte */
	ADS1220SendByte(ADS1220_CMD_WREG | (((StartAddress<<2) & 0x0c) |((NumRegs-1)&0x03)));
    /* send the data bytes */
	for (i=0; i< NumRegs; i++)
	{
		ADS1220SendByte(*pData++);
	}
   	/* de-assert CS */
	ADS1220CsStatus(ADS1220_DISABLE);
   	return;
}
/**
  * @brief  ADS1220SendResetCommand(void)重启命令
  * @param  none
  * @retval none
  */
void ADS1220SendResetCommand(void)
{
	/* assert CS to start transfer */
	ADS1220CsStatus(ADS1220_ENABLE);
   	/* send the command byte */
	ADS1220SendByte(ADS1220_CMD_RESET);
   	/* de-assert CS */
	ADS1220CsStatus(ADS1220_DISABLE);
   	return;
}
/**
  * @brief  ADS1220SendStartCommand(void)启动或重启转换
  * @param  none
  * @retval none
  */
void ADS1220SendStartCommand(void)
{
	/* assert CS to start transfer */
	ADS1220CsStatus(ADS1220_ENABLE);
    /* send the command byte */
	ADS1220SendByte(ADS1220_CMD_SYNC);
   	/* de-assert CS */
	ADS1220CsStatus(ADS1220_DISABLE);
    return;
}
/**
  * @brief  ADS1220SendShutdownCommand(void)进入掉电模式 
  * @param  none
  * @retval none
  */
void ADS1220SendShutdownCommand(void)
{
	/* assert CS to start transfer */
	ADS1220CsStatus(ADS1220_ENABLE);
   	/* send the command byte */
	ADS1220SendByte(ADS1220_CMD_SHUTDOWN);
   	/* de-assert CS */
	ADS1220CsStatus(ADS1220_DISABLE);
    return;
}
/**
  * @brief  int ADS1220SetChannel(int Mux)
  * @param  Mux 要设置的通道
  * @retval none
  */
int ADS1220SetChannel(int Mux)
{
	unsigned int cMux = Mux;	   
   /* write the register value containing the new value back to the ADS */
   ADS1220WriteRegister(ADS1220_0_REGISTER, 0x01, &cMux);
   return ADS1220_NO_ERROR;
}
/**
  * @brief  void ADS1220SetGain(ADS1220_Gain Gain)
* @param  Gain:要设置的放大倍数 根据枚举类型 ADS1220_Gain
  * @retval none
  */
void ADS1220SetGain(ADS1220_Gain Gain)
{
	unsigned int temp=0;
  ADS1220ReadRegister(ADS1220_0_REGISTER, 0x01, &temp);
	temp = temp & 0xf0; 
	switch(Gain)
	{
		case GAIN_1:
			temp = temp | GAIN_1;
			break;
		case GAIN_2:
			temp = temp | GAIN_2;
			break;
		case GAIN_4:
			temp = temp | GAIN_4;
			break;
		case GAIN_8:
			temp = temp | GAIN_8;
			break;	
		case GAIN_16:
			temp = temp | GAIN_16;
			break;
		case GAIN_32:
			temp = temp | GAIN_32;
			break;
		case GAIN_64:
			temp = temp | GAIN_64;
			break;
		case GAIN_128:
			temp = temp | GAIN_128;
			break;			
	}
	ADS1220WriteRegister(ADS1220_0_REGISTER, 0x01, &temp);
}

int ADS1220SetPGABypass(int Bypass)
{
	unsigned int cBypass = Bypass;
	/* write the register value containing the new code back to the ADS */
	ADS1220WriteRegister(ADS1220_0_REGISTER, 0x01, &cBypass);
	return ADS1220_NO_ERROR;
}
int ADS1220SetDataRate(int DataRate)
{
	unsigned int cDataRate = DataRate;  
	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_1_REGISTER, 0x01, &cDataRate);
	return ADS1220_NO_ERROR;
}
int ADS1220SetClockMode(int ClockMode)
{
	unsigned int cClockMode = ClockMode;
   	/* write the register value containing the value back to the ADS */
	ADS1220WriteRegister(ADS1220_1_REGISTER, 0x01, &cClockMode);
	return ADS1220_NO_ERROR;
}
int ADS1220SetPowerDown(int PowerDown)
{
	unsigned int cPowerDown = PowerDown;
   	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_1_REGISTER, 0x01, &cPowerDown);
	return ADS1220_NO_ERROR;
}
int ADS1220SetTemperatureMode(int TempMode)
{
	unsigned int cTempMode = TempMode;
   	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_1_REGISTER, 0x01, &cTempMode);
	return ADS1220_NO_ERROR;
}
int ADS1220SetBurnOutSource(int BurnOut)
{
	unsigned int cBurnOut = BurnOut;
   	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_1_REGISTER, 0x01, &cBurnOut);
	return ADS1220_NO_ERROR;
}
int ADS1220SetVoltageReference(int VoltageRef)
{
	unsigned int cVoltageRef = VoltageRef;
   	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_2_REGISTER, 0x01, &cVoltageRef);
	return ADS1220_NO_ERROR;
}
int ADS1220Set50_60Rejection(int Rejection)
{
	unsigned int cRejection = Rejection;
   	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_2_REGISTER, 0x01, &cRejection);
	return ADS1220_NO_ERROR;
}
int ADS1220SetLowSidePowerSwitch(int PowerSwitch)
{
	unsigned int cPowerSwitch = PowerSwitch;
   	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_2_REGISTER, 0x01, &cPowerSwitch);
	return ADS1220_NO_ERROR;
}
int ADS1220SetCurrentDACOutput(int CurrentOutput)
{
	unsigned int cCurrentOutput = CurrentOutput;
   	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_2_REGISTER, 0x01, &cCurrentOutput);
	return ADS1220_NO_ERROR;
}
int ADS1220SetIDACRouting(int IDACRoute)
{
	unsigned int cIDACRoute = IDACRoute;
	/* write the register value containing the new value back to the ADS */
	ADS1220WriteRegister(ADS1220_3_REGISTER, 0x01, &cIDACRoute);
	return ADS1220_NO_ERROR;
}
int ADS1220SetDRDYMode(int DRDYMode)
{
	unsigned int cDRDYMode = DRDYMode;
   	/* write the register value containing the new gain code back to the ADS */
	ADS1220WriteRegister(ADS1220_3_REGISTER, 0x01, &cDRDYMode);
	return ADS1220_NO_ERROR;
}
/*
******************************************************************************
register get value commands
*/
int ADS1220GetChannel(void)
{
	unsigned Temp;
	/* Parse Mux data from register */
	ADS1220ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return (Temp >>4);
}
int ADS1220GetGain(void)
{
	unsigned Temp;
	/* Parse Gain data from register */
	ADS1220ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( (Temp & 0x0e) >>1);
}
int ADS1220GetPGABypass(void)
{
	unsigned Temp;
	/* Parse Bypass data from register */
	ADS1220ReadRegister(ADS1220_0_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return (Temp & 0x01);
}
int ADS1220GetDataRate(void)
{
	unsigned Temp;
	/* Parse DataRate data from register */
	ADS1220ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( Temp >>5 );
}
int ADS1220GetClockMode(void)
{
	unsigned Temp;
	/* Parse ClockMode data from register */
	ADS1220ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( (Temp & 0x18) >>3 );
}
int ADS1220GetPowerDown(void)
{
	unsigned Temp;
	/* Parse PowerDown data from register */
	ADS1220ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( (Temp & 0x04) >>2 );
}
int ADS1220GetTemperatureMode(void)
{
	unsigned Temp;
	/* Parse TempMode data from register */
	ADS1220ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( (Temp & 0x02) >>1 );
}
int ADS1220GetBurnOutSource(void)
{
	unsigned Temp;
	/* Parse BurnOut data from register */
	ADS1220ReadRegister(ADS1220_1_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( Temp & 0x01 );
}
int ADS1220GetVoltageReference(void)
{
	unsigned Temp;
	/* Parse VoltageRef data from register */
	ADS1220ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( Temp >>6 );
}
int ADS1220Get50_60Rejection(void)
{
	unsigned Temp;
	/* Parse Rejection data from register */
	ADS1220ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( (Temp & 0x30) >>4 );
}
int ADS1220GetLowSidePowerSwitch(void)
{
	unsigned Temp;
	/* Parse PowerSwitch data from register */
	ADS1220ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( (Temp & 0x08) >>3);
}
int ADS1220GetCurrentDACOutput(void)
{
	unsigned Temp;
	/* Parse IDACOutput data from register */
	ADS1220ReadRegister(ADS1220_2_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( Temp & 0x07 );
}
int ADS1220GetIDACRouting(int WhichOne)
{
	unsigned Temp;
	/* Check WhichOne sizing */
	if (WhichOne >1) return ADS1220_ERROR;
	
	/* Parse Mux data from register */
	ADS1220ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	if (WhichOne) return ( (Temp & 0x1c) >>2);
	else return ( Temp >>5 );
}
int ADS1220GetDRDYMode(void)
{
	unsigned Temp;
	/* Parse DRDYMode data from register */
	ADS1220ReadRegister(ADS1220_3_REGISTER, 0x01, &Temp);
	/* return the parsed data */
	return ( (Temp & 0x02) >>1 );
}




