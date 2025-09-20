#ifndef __MAIN_H
#define __MAIN_H

typedef unsigned char  uint8;                   /* 无符号8位整型变量                        */
typedef signed   char  int8;                    /* 有符号8位整型变量                        */
typedef unsigned short uint16;                  /* 无符号16位整型变量                       */
typedef signed   short int16;                   /* 有符号16位整型变量                       */
typedef unsigned long   uint32;                  /* 无符号32位整型变量                       */
typedef signed   long   int32;                   /* 有符号32位整型变量                       */
typedef float          fp32;                    /* 单精度浮点数（32位长度）                 */
typedef double         fp64;                    /* 双精度浮点数（64位长度） 				*/



#define false 0
#define	true  1

#include "ADS1220.h" 

#include "usr_c210.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"
#include "stm32f10x_it.h"
#include "usart2.h"
#include "usart3.h"
#include "usart4.h"
#include "usart5.h"

#include "stdarg.h"
#include "bsp_led.h"
#include "bsp_relays.h"
#include "bsp_adc.h"
#include "bsp_rccclkconfig.h"
#include "bsp_systick.h"
#include "bsp_parallel_serial.h" 
#include "system_control.h"
#include "bsp_rtc.h"
#include "bsp_calendar.h"
#include "bsp_date.h"
#include "internal_flash.h"
#include "pwm_output.h"
#include "bsp_iwdg.h" 
#include	"activate_key.h"
#include "bsp_spi_flash.h"




extern const uint16 Soft_Version; 


#endif
