#ifndef __REALYS_H
#define	__RELAYS_H


#include "stm32f10x.h"







#define RELAY1_GPIO_PORT    	GPIOD		              /* GPIO端口 */
#define RELAY1_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define RELAY1_GPIO_PIN			GPIO_Pin_2			        /* RELAY1 */



#define RELAY2_GPIO_PORT    	GPIOD		              /* GPIO端口 */
#define RELAY2_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define RELAY2_GPIO_PIN			GPIO_Pin_3		        /* RELAY2 */


#define RELAY3_GPIO_PORT    	GPIOD		              /* GPIO端口 */
#define RELAY3_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define RELAY3_GPIO_PIN			GPIO_Pin_1			        /* RELAY3 */

	
#define RELAY4_GPIO_PORT    	GPIOA		              /* GPIO端口 */
#define RELAY4_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define RELAY4_GPIO_PIN			GPIO_Pin_8			        /* RELAY4  原D13*/


#define RELAY5_GPIO_PORT    	GPIOD		              /* GPIO端口 */
#define RELAY5_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define RELAY5_GPIO_PIN			GPIO_Pin_14			        /* RELAY5 */



#define RELAY6_GPIO_PORT    	GPIOD		              /* GPIO端口 */
#define RELAY6_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define RELAY6_GPIO_PIN			GPIO_Pin_15			        /* RELAY6 */


#define RELAY7_GPIO_PORT    	GPIOC		              /* GPIO端口 */
#define RELAY7_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO端口时钟 */
#define RELAY7_GPIO_PIN			GPIO_Pin_6			        /* RELAY7 */


#define RELAY8_GPIO_PORT    	GPIOC		              /* GPIO端口 */
#define RELAY8_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO端口时钟 */
#define RELAY8_GPIO_PIN			GPIO_Pin_7			        /* RELAY8 */



#define RELAY9_GPIO_PORT    	GPIOC		              /* GPIO端口 */
#define RELAY9_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO端口时钟 */
#define RELAY9_GPIO_PIN			GPIO_Pin_8			        /* RELAY9 */

#define RELAY10_GPIO_PORT    	GPIOC		              /* GPIO端口 */
#define RELAY10_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO端口时钟 */
#define RELAY10_GPIO_PIN			GPIO_Pin_9			        /* RELAY10 */




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



#define RELAY1_TOGGLE		 digitalToggle(RELAY1_GPIO_PORT,RELAY1_GPIO_PIN)
#define RELAY1_OFF		   digitalLo(RELAY1_GPIO_PORT,RELAY1_GPIO_PIN)
#define RELAY1_ON			   digitalHi(RELAY1_GPIO_PORT,RELAY1_GPIO_PIN)



#define RELAY2_TOGGLE		 digitalToggle(RELAY2_GPIO_PORT,RELAY2_GPIO_PIN)
#define RELAY2_OFF		   digitalLo(RELAY2_GPIO_PORT,RELAY2_GPIO_PIN)
#define RELAY2_ON			   digitalHi(RELAY2_GPIO_PORT,RELAY2_GPIO_PIN)

#define RELAY3_TOGGLE		 digitalToggle(RELAY3_GPIO_PORT,RELAY3_GPIO_PIN)
#define RELAY3_OFF		   digitalLo(RELAY3_GPIO_PORT,RELAY3_GPIO_PIN)
#define RELAY3_ON			   digitalHi(RELAY3_GPIO_PORT,RELAY3_GPIO_PIN)

#define RELAY4_TOGGLE		 digitalToggle(RELAY4_GPIO_PORT,RELAY4_GPIO_PIN)
#define RELAY4_OFF		   digitalLo(RELAY4_GPIO_PORT,RELAY4_GPIO_PIN)
#define RELAY4_ON			   digitalHi(RELAY4_GPIO_PORT,RELAY4_GPIO_PIN)

#define RELAY5_TOGGLE		 digitalToggle(RELAY5_GPIO_PORT,RELAY5_GPIO_PIN)
#define RELAY5_OFF		   digitalLo(RELAY5_GPIO_PORT,RELAY5_GPIO_PIN)
#define RELAY5_ON			   digitalHi(RELAY5_GPIO_PORT,RELAY5_GPIO_PIN)

#define RELAY6_TOGGLE		 digitalToggle(RELAY6_GPIO_PORT,RELAY6_GPIO_PIN)
#define RELAY6_OFF		   digitalLo(RELAY6_GPIO_PORT,RELAY6_GPIO_PIN)
#define RELAY6_ON			   digitalHi(RELAY6_GPIO_PORT,RELAY6_GPIO_PIN)

#define RELAY7_TOGGLE		 digitalToggle(RELAY7_GPIO_PORT,RELAY7_GPIO_PIN)
#define RELAY7_OFF		   digitalLo(RELAY7_GPIO_PORT,RELAY7_GPIO_PIN)
#define RELAY7_ON			   digitalHi(RELAY7_GPIO_PORT,RELAY7_GPIO_PIN)

#define RELAY8_OFF		   digitalLo(RELAY8_GPIO_PORT,RELAY8_GPIO_PIN)
#define RELAY8_ON			   digitalHi(RELAY8_GPIO_PORT,RELAY8_GPIO_PIN)
#define RELAY8_TOGGLE		 digitalToggle(RELAY8_GPIO_PORT,RELAY8_GPIO_PIN)

#define RELAY9_OFF		   digitalLo(RELAY9_GPIO_PORT,RELAY9_GPIO_PIN)
#define RELAY9_ON			   digitalHi(RELAY9_GPIO_PORT,RELAY9_GPIO_PIN)
#define RELAY9_TOGGLE		 digitalToggle(RELAY9_GPIO_PORT,RELAY9_GPIO_PIN)


//用于报警继电器输出
#define RELAY10_OFF		   digitalLo(RELAY10_GPIO_PORT,RELAY10_GPIO_PIN)
#define RELAY10_ON			   digitalHi(RELAY10_GPIO_PORT,RELAY10_GPIO_PIN)
#define RELAY10_TOGGLE		 digitalToggle(RELAY10_GPIO_PORT,RELAY10_GPIO_PIN)






typedef struct _RELAYSF
{
	volatile	uint8  dian_huo_flag;  //点火开启标志
	volatile	uint8 gas_on_flag; //燃气开启标志
	volatile	uint8 air_on_flag; //风机开启标志
	volatile	uint8 pai_wu_flag; //排污阀开启标志
	volatile	uint8 xun_huan_flag; //循环泵开启标志
	volatile	uint8 water_switch_flag; //补水泵继电器开启标志
	volatile	uint8 Water_Valve_Flag; //补水电磁阀继电器，动作标志
	

	volatile	uint8 Normal_MovieFlag; //用于主界面正常动画的操控标志
		

	volatile	uint8 Flame_Flag;
	volatile	uint8 LianXu_PaiWu_flag;  //用于连续排污继电器
	volatile	uint8 MingHuo_Flag;

	volatile	uint8 OpenWait_7ms;   //延迟7ms
	volatile	uint8 CloseWait_3ms;   //延迟3ms
		
}SWITCH_STATUS;





 
 
 

 
//extern unsigned char gas_on_flag;// =1 时，燃气阀组开启，0则关闭


extern SWITCH_STATUS Switch_Inf; 







void RELAYS_GPIO_Config(void);

void	Dian_Huo_Start(void);//控制点火继电器启动
void	Dian_Huo_OFF(void);//控制点火继电器关闭

void 	Send_Gas_Open(void);  //燃气阀组开启
void	Send_Gas_Close(void);//燃气阀组关闭


void 	Send_Air_Open(void); //控制风机开启 //燃气阀组开启
void	Send_Air_Close(void);//控制风机关闭 //燃气阀组关闭
void Feed_First_Level(void); //送料电机，速度1，小火量调节
uint8  Dian_Huo_Air_Level(void);//点火风量控制


void Feed_test_begin(void);//手动模式测试风机
void Feed_test_over(void);//停止风机测试



uint8  Feed_Main_Pump_ON(void);//打开补水泵
uint8  Feed_Main_Pump_OFF(void); //关闭补水泵

uint8  Second_Water_Valve_Open(void);
uint8  Second_Water_Valve_Close(void);



void  Pai_Wu_Door_Open(void);

void  Pai_Wu_Door_Close(void);

void WTS_Gas_One_Open(void);

void WTS_Gas_One_Close(void);

uint8 Alarm_Out_Function(void);

uint8 Special_Water_Open(void);

uint8 Special_Water_OFF(void);


uint8 LianXu_Paiwu_Open(void);

uint8 LianXu_Paiwu_Close(void);

void Close_All_Realys_Function(void);

uint8 Logic_Pump_On(void);
uint8 Logic_Pump_OFF(void);



#endif /* __RELAYS_H */



