#include "usart5.h"
#include "stdarg.h"







void U5_send_byte(u8 data)
{
	 USART_SendData(UART5,data);
	 while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
}

//串口5发送s个字符
void U5_send_str(u8 *str,u8 s)
{
	u8 i;
	for(i=0;i<s;i++)
	{
		U5_send_byte(*str);
		str++;
	}
}


///////////////////////////////////////USART5 printf支持部分//////////////////////////////////
//串口2,u2_printf 函数
//确保一次发送数据不超过USART4_MAX_SEND_LEN字节
////////////////////////////////////////////////////////////////////////////////////////////////
void U5_Printf(char* fmt,...)  
{  
  int len=0;
	int cnt=0;

	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)U5_Inf.TX_Data,fmt,ap);
	va_end(ap);
	len = strlen((const char*)U5_Inf.TX_Data);
	while(len--)
	  {
	  while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=1); //等待发送结束
	  	USART_SendData(UART5,U5_Inf.TX_Data[cnt++]);
	  }
}










///////////////////////////////////////USART2 初始化配置部分//////////////////////////////////	    
//功能：初始化IO 串口2
//输入参数
//bound:波特率
//输出参数
//无
//////////////////////////////////////////////////////////////////////////////////////////////	  
void uart5_init(u32 bound)
{  	 		 
	//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//使能USART5
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD, ENABLE);	//使能GPIOC和GPIOD时钟
	 
	USART_DeInit(UART5);  //复位串口5

     //UART5_TX   PC.12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
   
    //UART5_RX	  PD.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);  

   

	
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	        //根据指定的参数初始化VIC寄存器
  
    //USART5 初始化设置
	USART_InitStructure.USART_BaudRate = bound;   // 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;  //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //收发模式

    USART_Init(UART5, &USART_InitStructure);   //初始化串口
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);  //开启中断
    USART_Cmd(UART5, ENABLE);                      //使能串口 
	
//	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);    //使能串口2的DMA发送
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF,1000);  //DMA1通道7,外设为串口2,存储器为USART2_TX_BUF,长度1000. 										  	
}










//MCU将更新过的数据发给LCD


void send_byte(u8 data)
{
	 USART_SendData(USART2,data);
	 while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
}
void send_str(u8 *str,u8 s)
{
	u8 i;
	for(i=0;i<s;i++)
	{
		send_byte(*str);
		str++;
	}
}







