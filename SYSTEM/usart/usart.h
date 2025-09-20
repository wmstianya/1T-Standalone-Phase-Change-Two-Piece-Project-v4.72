#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "main.h"


#define             macUSART_BAUD_RATE                       115200

#define             macUSARTx                                USART1
#define             macUSART_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define             macUSART_CLK                             RCC_APB2Periph_USART1
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)     
#define             macUSART_TX_PORT                         GPIOA   
#define             macUSART_TX_PIN                          GPIO_Pin_9
#define             macUSART_RX_PORT                         GPIOA   
#define             macUSART_RX_PIN                          GPIO_Pin_10
#define             macUSART_IRQ                             USART1_IRQn
#define             macUSART_INT_FUN                         USART1_IRQHandler
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern 	uint16 Usart1_Rx_Length ;




 




//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void USARTx_Config(void);

void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);




typedef union _RTU_COM
{
	uint8 ALL[21];
	struct{
	 uint8  Address;
	 uint8  Cmd;
	 uint8  Length;

	 uint8  Data_01H;
	 uint8  Data_01L;
	 
	 uint8  Data_02H;
	 uint8  Data_02L;
	 
	 uint8  Data_03H;
	 uint8  Data_03L;
	 
	 uint8  Data_04H;
	 uint8  Data_04L;
	 
	 uint8  Data_05H;
	 uint8  Data_05L;

	 uint8  Data_06H;
	 uint8  Data_06L;

	  uint8  Data_07H;
	 uint8  Data_07L;

	  uint8  Data_08H;
	 uint8  Data_08L;

	

	 uint8 Crc_H;
	 uint8 Crc_L;
		}Mess;
	
		 
		
}RTU_DATA;


//===============================


 
 


extern	uint8 send1_falg;
extern	uint8 USART1_RX_BUF[];
//===================================
extern RTU_DATA Rtu_Data ;



void u1_printf(char* fmt,...)  ;

void ModBus_Communication(void);

uint16 ModBusCRC16(unsigned char *ptr,uint8 size);


uint16 Lcd_CRC16(unsigned char *ptr,uint8 size);





#endif



