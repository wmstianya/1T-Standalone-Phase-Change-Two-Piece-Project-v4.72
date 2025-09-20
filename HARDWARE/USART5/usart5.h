#include "main.h"
#ifndef __USART5_H
#define __USART5_H	  	     
 







typedef struct LCDM_B
{
	uint8 Air_Value;
	uint8 Pump_Value;
	uint8 Flame_Value;
	uint8 Auto_Paiwu_Value;
	uint8 Normal_Value;  //常规动画的值
	
	
}LCD_MB;








typedef struct _LCD_GG_
{
	
	uint8 Lcd_Cmd;
	uint8 Start_Address_H;
	uint8 Start_Address_L;

	uint8 Data_00H;
	uint8 Data_00L;// 风机动画地址
	uint8 Data_01H;
	uint8 Data_01L;//
	
	uint8 Data_02H;
	uint8 Data_02L;//水泵动画地址
	uint8 Data_03H;
	uint8 Data_03L;//

	uint8 Data_04H;
	uint8 Data_04L;//火焰动画地址
	uint8 Data_05H;
	uint8 Data_05L;//

	uint8 Data_06H;
	uint8 Data_06L;//自动排污保留未使用
	uint8 Data_07H;
	uint8 Data_07L;// 

	
	uint8 Data_08H;
	uint8 Data_08L;	//主界面，运行时，动画显示	
	uint8 Data_09H;
	uint8 Data_09L;// 
	
	
	
}LCD_SDATA;








///////////////////////////////////////USART2 printf支持部分//////////////////////////////////
void U5_send_byte(u8 data);
void U5_send_str(u8 *str,u8 s);



////////////////////////////////////////////////////////////////////////////////////////////////
void U5_Printf(char* fmt,...);
///////////////////////////////////////USART2 初始化配置部分//////////////////////////////////	    
//功能：初始化IO 串口2
//输入参数
//bound:波特率
//输出参数
//无
//////////////////////////////////////////////////////////////////////////////////////////////
void uart5_init(u32 bound);	

void send_byte(u8 data);
void send_str(u8 *str,u8 s);







#endif




