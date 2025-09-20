


#ifndef __C210_H
#define __C210_H	

#include "main.h"


/* 定义WIFI连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的WIFI引脚 */





/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
 

typedef union _Internet_COM
{
	uint8 ALL[35];
	struct{
	 uint8  Address;
	 uint8  Cmd;
	 uint8  Length;

	
	 uint8  Data_01;
	 uint8  Data_02;
	 uint8  Data_03;
	 uint8  Data_04;
	 uint8  Data_05;
	 uint8  Data_06;
	 uint8  Data_07;
	 uint8  Data_08;
	 uint8  Data_09;
	 uint8  Data_10;

	 uint8  Data_11;
	 uint8  Data_12;
	 uint8  Data_13;
	 uint8  Data_14;
	 uint8  Data_15;
	 uint8  Data_16;
	 uint8  Data_17;
	 uint8  Data_18;
	 uint8  Data_19;
	 uint8  Data_20;

	 uint8  Data_21;
	 uint8  Data_22;
	 uint8  Data_23;
	 uint8  Data_24;
	 uint8  Data_25;
	 uint8  Data_26;
	 uint8  Data_27;
	 uint8  Data_28;
	 uint8  Data_29;
	 uint8  Data_30;

	 uint8 Crc_H;
	 uint8 Crc_L;
		}Mess;
	
		 
		
}WWW_DATA;







extern WWW_DATA Talk;

uint8  New_Server_Cmd_Analyse(void);


void Mcu_mail_Wifi(void);

void Server_Cmd_Analyse(void);

uint8 Boss_Lock_Function(void);

uint8 ModuBusRTU_WriteResponse(uint16 address,uint16 Data16);
uint8 ModuBusRTU_Write0x10Response(uint16 address,uint16 Data16);

uint32 Char_to_Int_6(uint32 number_H,uint32 number_L) ;



#endif




