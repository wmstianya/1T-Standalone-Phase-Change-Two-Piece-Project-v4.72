#include "main.h"
#ifndef __USART3_H
#define __USART3_H	  	     
 


extern uint8 USART3_RX_BUF[256];
extern uint16 USART3_RX_STA;
extern uint8 re3_time_falg;
extern uint16 re3_time;
extern uint8 send3_falg;


///////////////////////////////////////USART2 printf支持部分//////////////////////////////////
//串口2,u2_printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
////////////////////////////////////////////////////////////////////////////////////////////////
void u3_printf(char* fmt, ...);
///////////////////////////////////////USART2 初始化配置部分//////////////////////////////////	    
//功能：初始化IO 串口2
//输入参数
//bound:波特率
//输出参数
//无
//////////////////////////////////////////////////////////////////////////////////////////////
void uart3_init(u32 bound);	

uint8 JiZu_ReadAndWrite_Function(uint8 address);
void ModBus_Uart3_LocalRX_Communication(void);

uint8 Modbus3_UnionTx_Communication(void);
uint8 ModBus3_RTU_Write06(uint8 Target_Address,uint16 Data_Address,uint16 Data16);
uint8 ModBus3_RTU_Read03(uint8 Target_Address,uint16 Data_Address,uint8 Data_Length );
uint8 Modbus3_UnionRx_DataProcess(uint8 Cmd,uint8 address);

uint8 ModBus3_RTU_Write10(uint8 Target_Address,uint16 Data_Address);
uint8 Union_MuxJiZu_Control_Function(void);



#endif

