#ifndef __LED_H
#define	__LED_H


#include "stm32f10x.h"
#include "main.h"


typedef struct _BEEP
{
		uint16 beep_time_set;
	  uint16  time;   //1ms定时累加
		uint8 beep_on_flag;
		
	volatile	uint8  beep_start_flag ; //报警声音开启，滴 滴 声音
	volatile	uint8 beep_period_flag ;//周期标志
	
		
}Beep_Message;

//#define Beep_Period_Time  500 ;//设定周期时间500MS
//extern uint8 beep_on_flag;
extern uint8 Temperature_Channel ;
extern uint8 Channel_Now ;

extern Beep_Message Beep_Data;


#define PIN_HIGH  1
#define PIN_LOW   0

/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */




//蜂鸣器接口定义

#define BEEP_GPIO_PORT    	GPIOD			              /* GPIO端口 */
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define BEEP_GPIO_PIN		GPIO_Pin_4		        /* 连接到SCL时钟线的GPIO */

//外置看门狗周期信号引脚
#define WDI_GPIO_PORT    	GPIOE			              /* GPIO端口 */
#define WDI_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define WDI_GPIO_PIN		GPIO_Pin_3		        /* 连接到SCL时钟线的GPIO */

//工作指示灯
#define WORK_LED_GPIO_PORT    	GPIOE			              /* GPIO端口 */
#define WORK_LED_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define WORK_LED_GPIO_PIN		GPIO_Pin_2		        /* 连接到SCL时钟线的GPIO */




#define INPUT_ADDRESS_PORT    GPIOD
#define INPUT_GPIO_CLK 		  RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */	
#define INPUT_PIN_1			  GPIO_Pin_5
#define INPUT_PIN_2			  GPIO_Pin_6
#define INPUT_PIN_3			  GPIO_Pin_7


#define INPUT_PIN_4			  GPIO_Pin_3
#define INPUT_PIN_5			  GPIO_Pin_4
#define INPUT_PIN_6			  GPIO_Pin_5
#define INPUT_PIN_7			  GPIO_Pin_6
#define INPUT_PIN_8			  GPIO_Pin_7
#define INPUT_PIN_9			  GPIO_Pin_8
#define INPUT_PIN_10		  GPIO_Pin_9

#define INPUT_PIN_11		  GPIO_Pin_0
#define INPUT_PIN_12		  GPIO_Pin_1



//从机地址配置引脚


#define SLAVE_ADDRESS_PORT    GPIOC
#define SLAVE_GPIO_CLK 		  RCC_APB2Periph_GPIOC		/* GPIO端口时钟 */	
#define SLAVE_PIN_1			  GPIO_Pin_5   //ADDR 1
#define SLAVE_PIN_2			  GPIO_Pin_4	 //ADDR 2
#define SLAVE_PIN_3			  GPIO_Pin_7
#define SLAVE_PIN_4			  GPIO_Pin_6











 
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


#define WORK_LED_TOGGLE		 digitalToggle(WORK_LED_GPIO_PORT,WORK_LED_GPIO_PIN)

#define WDI_TOGGLE		 digitalToggle(WDI_GPIO_PORT,WDI_GPIO_PIN)
#define WDI_OFF		   digitalLo(WDI_GPIO_PORT,WDI_GPIO_PIN)
#define WDI_ON		   digitalHi(WDI_GPIO_PORT,WDI_GPIO_PIN)



#define BEEP_TOGGLE		 digitalToggle(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_OFF		   digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_ON			   digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)







//*****************SPI 管脚定义配置 ***********************************//
#define SPI_ENABLE    1
#define SPI_DISABLE   0

	typedef enum
	{
		CS,
		SCLK,
		DIN,
		DOUT
	}SPI_GPIO;

	typedef struct 
	{
	   GPIO_TypeDef *port[4];
	   uint16_t pin[4];	
	}SPI_InitType;


//============================================================================//

//============================================================================//
void LED_GPIO_Config(void);



void BEEP_TIME(uint16 nms);

uint8 PortPin_Scan(GPIO_TypeDef *GPIOx,uint16 GPIO_Pin);


uint8 New_Flame_Scan(void);

void Delay_us(uint16 Value);




/***************SPI函数定义***********************/
static void SPI_delay_us(uint16 Value);
void SPI_Config_Init(void);
void SpiCsStatus( uint8 state);
uint8 SPI_RW_Data_MODE0(uint8 sendData);
uint8  SpiReadData(void);



#endif /* __LED_H */


