#ifndef __PARARREL_SERIAL_H
#define	__PARARREL_SERIAL_H

#include "main.h"
#include "stm32f10x.h"



extern uint8 SER1_Data[8];
extern uint8 SER2_Data[8];


#define SERI1_GPIO_PORT    	GPIOB	              /* GPIO端口 */
#define SERI1_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define SERI1_GPIO_PIN			GPIO_Pin_9			        /* POWER_12V */



#define SERI2_GPIO_PORT    	GPIOB		              /* GPIO端口 */
#define SERI2_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define SERI2_GPIO_PIN			GPIO_Pin_8			        /* RELAY1 */



#define SERCP_GPIO_PORT    	GPIOE		              /* GPIO端口 */
#define SERCP_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define SERCP_GPIO_PIN			GPIO_Pin_0			        /* RELAY2 */


#define SERPL_GPIO_PORT    	GPIOE	              /* GPIO端口 */
#define SERPL_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define SERPL_GPIO_PIN			GPIO_Pin_1			        /* RELAY3 */

	
 



/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态


/* 定义控制IO的宏 */
#define SERCP_TOGGLE		 digitalToggle(SERCP_GPIO_PORT,SERCP_GPIO_PIN)
#define SERCP_OFF		   digitalLo(SERCP_GPIO_PORT,SERCP_GPIO_PIN)
#define SERCP_ON			   digitalHi(SERCP_GPIO_PORT,SERCP_GPIO_PIN)


#define SERPL_TOGGLE		 digitalToggle(SERPL_GPIO_PORT,SERPL_GPIO_PIN)
#define SERPL_OFF		   digitalLo(SERPL_GPIO_PORT,SERPL_GPIO_PIN)
#define SERPL_ON			   digitalHi(SERPL_GPIO_PORT,SERPL_GPIO_PIN)


 
 
 




void PARALLEL_SERIAL_GPIO_Config(void);
void read_serial_data(void);
 

#endif /*__PARARREL_SERIAL_H */



