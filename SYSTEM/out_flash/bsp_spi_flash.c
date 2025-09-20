
 
  
#include "bsp_spi_flash.h"
#include "system_control.h"


static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;    
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

OUT_FLASH_BULID Flash_Data;
OUT_FLASH_BULID Flash_Buffer;



/**
  * @brief  SPI_FLASH初始化
  * @param  无
  * @retval 无
  */
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;


  
	/* 使能SPI时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	
	/* 使能SPI引脚相关的时钟 */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
  //SPI_FLASH_SPI pins: SCK--PB3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
  //SPI_FLASH_SPI pins: MISO--PB4
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
  //SPI_FLASH_SPI pins: MOSI--PB5 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

   //SPI_FLASH_SPI_CS_PIN pin: CS---PB6

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  
  //SPI_FLASH_SPI_CS_PIN pin: CS--PA15
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* 停止信号 FLASH: CS引脚高电平*/
  SPI_FLASH_CS_HIGH();

  /* SPI 模式配置 */
  // FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
 // SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//第一个版本为4分频
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(FLASH_SPIx , &SPI_InitStructure);

  /* 使能 SPI  */
  SPI_Cmd(FLASH_SPIx , ENABLE);
	
}
 /**
  * @brief  擦除FLASH扇区
  * @param  SectorAddr：要擦除的扇区地址
  * @retval 无
  */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* 擦除扇区 */
  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();
  /* 发送扇区擦除指令*/
  SPI_FLASH_SendByte(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();
  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

 /**
  * @brief  擦除FLASH扇区，整片擦除
  * @param  无
  * @retval 无
  */
void SPI_FLASH_BulkErase(void)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 整块 Erase */
  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();
  /* 发送整块擦除指令*/
  SPI_FLASH_SendByte(W25X_ChipErase);
  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();

  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

 /**
  * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * @retval 无
  */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();
  /* 写页写指令*/
  SPI_FLASH_SendByte(W25X_PageProgram);
  /*发送写地址的高位*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     FLASH_ERROR("SPI_FLASH_PageWrite too large!"); 
  }

  /* 写入数据*/
  while (NumByteToWrite--)
  {
    /* 发送当前要写入的字节数据 */
    SPI_FLASH_SendByte(*pBuffer);
    /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();

  /* 等待写入完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

 /**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param  WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度
  * @retval 无
  */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	/*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/*差count个数据值，刚好可以对齐到页地址*/
  count = SPI_FLASH_PageSize - Addr;
	/*计算出要写多少整数页*/
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	/*mod运算求余，计算出剩余不满一页的字节数*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
	
	/* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
  if (Addr == 0)
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    { 
			/*先把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
	/* 若地址与 SPI_FLASH_PageSize 不对齐  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0)
    {
			/*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				/*先写满当前页*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
				
        WriteAddr +=  count;
        pBuffer += count;
				/*再写剩余的数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*地址不对齐多出的count分开处理，不加入这个运算*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
			
			/* 先写完count个数据，为的是让下一次要写的地址对齐 */
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
			
			/* 接下来就重复地址对齐的情况 */
      WriteAddr +=  count;
      pBuffer += count;
			/*把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

 /**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
  */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();

  /* 发送 读 指令 */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* 发送 读 地址高位 */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
	/* 读取数据 */
  while (NumByteToRead--) /* while there is data to be read */
  {
    /* 读取一个字节*/
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();
}

 /**
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* 开始通讯：CS低电平 */
  SPI_FLASH_CS_LOW();

  /* 发送JEDEC指令，读取ID */
  SPI_FLASH_SendByte(W25X_JedecDeviceID);

  /* 读取一个字节数据 */
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

 /* 停止通讯：CS高电平 */
  SPI_FLASH_CS_HIGH();

  /*把数据组合起来，作为函数的返回值*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}
 /**
  * @brief  读取FLASH Device ID
  * @param 	无
  * @retval FLASH Device ID
  */
u32 SPI_FLASH_ReadDeviceID(void)
{
  u32 Temp = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  
  /* Read a byte from the FLASH */
  Temp = SPI_FLASH_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  return Temp;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}


 /**
  * @brief  使用SPI读取一个字节的数据
  * @param  无
  * @retval 返回接收到的数据
  */
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

 /**
  * @brief  使用SPI发送一个字节的数据
  * @param  byte：要发送的数据
  * @retval 返回接收到的数据
  */
u8 SPI_FLASH_SendByte(u8 byte)
{
	 SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 等待发送缓冲区为空，TXE事件 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_TXE) == RESET)
	{
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
   }

  /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
  SPI_I2S_SendData(FLASH_SPIx , byte);

	SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 等待接收缓冲区非空，RXNE事件 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
   }

  /* 读取数据寄存器，获取接收缓冲区数据 */
  return SPI_I2S_ReceiveData(FLASH_SPIx );
}

 /**
  * @brief  使用SPI发送两个字节的数据
  * @param  byte：要发送的数据
  * @retval 返回接收到的数据
  */
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
	  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 等待发送缓冲区为空，TXE事件 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_TXE) == RESET)
	{
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(2);
   }
	
  /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
  SPI_I2S_SendData(FLASH_SPIx , HalfWord);

	 SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 等待接收缓冲区非空，RXNE事件 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_RXNE) == RESET)
	 {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(3);
   }
  /* 读取数据寄存器，获取接收缓冲区数据 */
  return SPI_I2S_ReceiveData(FLASH_SPIx );
}

 /**
  * @brief  向FLASH发送 写使能 命令
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
  /* 通讯开始：CS低 */
  SPI_FLASH_CS_LOW();

  /* 发送写使能命令*/
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /*通讯结束：CS高 */
  SPI_FLASH_CS_HIGH();
}

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                  0x01

 /**
  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* 选择 FLASH: CS 低 */
  SPI_FLASH_CS_LOW();

  /* 发送 读状态寄存器 命令 */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* 若FLASH忙碌，则等待 */
  do
  {
		/* 读取FLASH芯片的状态寄存器 */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET);  /* 正在写入标志 */

  /* 停止信号  FLASH: CS 高 */
  SPI_FLASH_CS_HIGH();
}


//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  /* 通讯开始：CS低 */
  SPI_FLASH_CS_LOW();

  /* 发送 掉电 命令 */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /*通讯结束：CS高 */
  SPI_FLASH_CS_HIGH();
}   

//唤醒
void SPI_Flash_WAKEUP(void)   
{
  /*选择 FLASH: CS 低 */
  SPI_FLASH_CS_LOW();

  /* 发送 上电 命令 */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

   /* 停止信号 FLASH: CS 高 */
  SPI_FLASH_CS_HIGH();
}   
   

/**
  * @brief  等待超时回调函数
  * @param  None.
  * @retval None.
  */
static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* 等待超时后的处理,输出错误信息 */
  FLASH_ERROR("SPI 等待超时!errorCode = %d",errorCode);
  return 0;
}



/**
  * @brief  测试SPI FLASH 的好坏
  * @param  None.
  * @retval None.
  */

TestStatus SPI_FLASH_TEST(void)
{
		TestStatus TransferStatus1 = FAILED;
		uint8_t Tx_Buffer[] = "Hello Passmack!\r\n";
		uint8_t Rx_Buffer[20];
		uint8_t size_value = 0;

		size_value = countof(Tx_Buffer) - 1;


	 
		printf("\r\n FlashID is 0x%X\r\n", SPI_FLASH_ReadID());


		SPI_FLASH_SectorErase(0x00000);	 	 
		
		
		SPI_FLASH_BufferWrite(Tx_Buffer, 0x00000, size_value);		
		printf("\r\n 写入的数据为： %s \r\t", Tx_Buffer);
		
	
		SPI_FLASH_BufferRead(Rx_Buffer, 0x00000, size_value);
		printf("\r\n 读出的数据为 ： %s \r\n", Rx_Buffer);
		
	
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, size_value);
		
		if( PASSED == TransferStatus1 )
		{ 
			printf("\r\n外部FLASH测试成功!\n\r");
			return PASSED; 
		}
		else
		{        
			printf("\r\n 外部FLASH测试失败!\n\r");
			return FAILED; 
		}
}


/**
  * @brief  对FLASH数据进行更新，以0x10000地址为起点，10W次，地址增加0x01000H,该地址改变后存储在0x1000H
  * @param 	sys_flag.Flash_Address.
  * @retval None.
  */

void  SPI_FLASH_DATA_UPDATA(void)
{

	//要对FLASH 地址进行最低检测，防止以后FLASH 损坏，而MCU未坏的情况
	if(sys_flag.Flash_Address < FLASH_Min_WriteAddress)
		{
			sys_flag.Flash_Address = FLASH_Min_WriteAddress;
			Flash_Buffer.Buffer_Data = sys_flag.Flash_Address;
			
			SPI_FLASH_SectorErase(FLASH_PAGE_Save_Address);	
			SPI_FLASH_BufferWrite((uint8 *)&Flash_Buffer, FLASH_PAGE_Save_Address,sizeof(Flash_Buffer));	
		}

	SysTick_Delay_ms(5);
	//第一步： 读出数据将要存放的地址
	SPI_FLASH_BufferRead((uint8 *)&Flash_Buffer, FLASH_PAGE_Save_Address, sizeof(Flash_Buffer));
	sys_flag.Flash_Address = Flash_Buffer.Buffer_Data;

	//第二步： 读出地址中的数据，存在Buffer中
	SPI_FLASH_BufferRead((uint8 *)&Flash_Buffer, sys_flag.Flash_Address, sizeof(Flash_Buffer));
	Flash_Data.ProgramTimes = Flash_Buffer.ProgramTimes;
	
	Flash_Data.ProgramTimes++;  //对烧写次数进行累加 并最终存储在数据区
	
	//第三步： 判断存放的地址烧写次数是否合格  10W次，一个更新周期
	if(Flash_Data.ProgramTimes > 100000)
		{
			Flash_Data.ProgramTimes  = 0;//对计数清零
			memset(&Flash_Buffer,0,sizeof(Flash_Buffer));
			sys_flag.Flash_Address = sys_flag.Flash_Address + 0x01000;//加一页，并更新到FLASH中
			Flash_Buffer.Buffer_Data = sys_flag.Flash_Address;
			SPI_FLASH_SectorErase(FLASH_PAGE_Save_Address);	
			SPI_FLASH_BufferWrite((uint8 *)&Flash_Buffer, FLASH_PAGE_Save_Address,sizeof(Flash_Buffer));	
		}

	//第四步： 将数据存储
	SPI_FLASH_SectorErase(sys_flag.Flash_Address);
	SPI_FLASH_BufferWrite((uint8 *)&Flash_Data, sys_flag.Flash_Address,sizeof(Flash_Data));	
		
	//第五步： 将新数据取回	
	SPI_FLASH_BufferRead((uint8 *)&Flash_Data, sys_flag.Flash_Address, sizeof(Flash_Data));	
	memset(&Flash_Buffer,0,sizeof(Flash_Buffer));	
}


/**
  * @brief  比较两个字符串是否相等
  * @param  None.
  * @retval None.
  */

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}


uint8 Load_Spi_Flash_Error_Data(void)
{
	uint8 sstemp1[6] = {0x5A,0XA5,0X09,0X82,0x00,0X80};
	//第一步：读取历史故障信息
		if(SPI_FLASH_TEST())
			{
				SPI_FLASH_BufferRead((uint8 *)&SPI_Error_Data, SPI_ERROR_DATA_ADDRESS, sizeof(SPI_Error_Data));
				//第二步：将SPI FLASH中的数据信息赋值给现有的故障结构体，共8个故障
			  Err_Lcd_Memory[0] = SPI_Error_Data.Error1;
			  Err_Lcd_Memory[1] = SPI_Error_Data.Error2;
			  Err_Lcd_Memory[2] = SPI_Error_Data.Error3;
			  Err_Lcd_Memory[3] = SPI_Error_Data.Error4;
			  Err_Lcd_Memory[4] = SPI_Error_Data.Error5;
			  Err_Lcd_Memory[5] = SPI_Error_Data.Error6;
			  Err_Lcd_Memory[6] = SPI_Error_Data.Error7;
			  Err_Lcd_Memory[7] = SPI_Error_Data.Error8;
			}
		else
			{
				//FLASH测试读取失败时，故障界面进行清零
				memset(& Err_Lcd_Memory[0],0,sizeof( Err_Lcd_Memory[0]));
				Err_Lcd_Memory[1] = Err_Lcd_Memory[0];
				Err_Lcd_Memory[2] = Err_Lcd_Memory[0];
				Err_Lcd_Memory[3] = Err_Lcd_Memory[0];
				Err_Lcd_Memory[4] = Err_Lcd_Memory[0];
				Err_Lcd_Memory[5] = Err_Lcd_Memory[0];
				Err_Lcd_Memory[6] = Err_Lcd_Memory[0];
				Err_Lcd_Memory[7] = Err_Lcd_Memory[0];
			}
			

	//第三步：将信息发送给显示屏		  	
			  	 sstemp1[5] = 0x80;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[0],sizeof(Err_Lcd_Memory[0]));

				sstemp1[5] = 0x83;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[1],sizeof(Err_Lcd_Memory[1]));

				sstemp1[5] = 0x86;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[2],sizeof(Err_Lcd_Memory[2]));

				sstemp1[5] = 0x89;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[3],sizeof(Err_Lcd_Memory[3]));

				sstemp1[5] = 0x8C;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[4],sizeof(Err_Lcd_Memory[4]));

				sstemp1[5] = 0x8F;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[5],sizeof(Err_Lcd_Memory[5]));

				sstemp1[5] = 0x92;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[6],sizeof(Err_Lcd_Memory[6]));

				sstemp1[5] = 0x95;
				send_str(sstemp1,6);  //发送包头
				send_str((uint8 *)&Err_Lcd_Memory[7],sizeof(Err_Lcd_Memory[7]));

	return 0 ;
}


uint8 Save_Spi_Flash_Error_Data(void)
{
	if(SPI_FLASH_TEST())
		{
			SPI_Error_Data.Error1 = Err_Lcd_Memory[0];
			SPI_Error_Data.Error2 = Err_Lcd_Memory[1];
			SPI_Error_Data.Error3 = Err_Lcd_Memory[2]; 
			SPI_Error_Data.Error4 = Err_Lcd_Memory[3]; 
			SPI_Error_Data.Error5 = Err_Lcd_Memory[4]; 
			SPI_Error_Data.Error6 = Err_Lcd_Memory[5]; 
			SPI_Error_Data.Error7 = Err_Lcd_Memory[6]; 
			SPI_Error_Data.Error8 = Err_Lcd_Memory[7]; 
			SPI_FLASH_SectorErase(SPI_ERROR_DATA_ADDRESS);
			SPI_FLASH_BufferWrite((uint8 *)&SPI_Error_Data, SPI_ERROR_DATA_ADDRESS,sizeof(SPI_Error_Data));	
		}
	return 0 ;
}

/*********************************************END OF FILE**********************/
