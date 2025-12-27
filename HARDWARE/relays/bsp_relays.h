#ifndef __BSP_RELAYS_H
#define __BSP_RELAYS_H


#include "stm32f10x.h"







#define RELAY1_GPIO_PORT    	GPIOD		              /* GPIO�˿� */
#define RELAY1_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO�˿�ʱ�� */
#define RELAY1_GPIO_PIN			GPIO_Pin_2			        /* RELAY1 */



#define RELAY2_GPIO_PORT    	GPIOD		              /* GPIO�˿� */
#define RELAY2_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO�˿�ʱ�� */
#define RELAY2_GPIO_PIN			GPIO_Pin_3		        /* RELAY2 */


#define RELAY3_GPIO_PORT    	GPIOD		              /* GPIO�˿� */
#define RELAY3_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO�˿�ʱ�� */
#define RELAY3_GPIO_PIN			GPIO_Pin_1			        /* RELAY3 */

	
#define RELAY4_GPIO_PORT    	GPIOA		              /* GPIO�˿� */
#define RELAY4_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO�˿�ʱ�� */
#define RELAY4_GPIO_PIN			GPIO_Pin_8			        /* RELAY4  ԭD13*/


#define RELAY5_GPIO_PORT    	GPIOD		              /* GPIO�˿� */
#define RELAY5_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO�˿�ʱ�� */
#define RELAY5_GPIO_PIN			GPIO_Pin_14			        /* RELAY5 */



#define RELAY6_GPIO_PORT    	GPIOD		              /* GPIO�˿� */
#define RELAY6_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO�˿�ʱ�� */
#define RELAY6_GPIO_PIN			GPIO_Pin_15			        /* RELAY6 */


#define RELAY7_GPIO_PORT    	GPIOC		              /* GPIO�˿� */
#define RELAY7_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO�˿�ʱ�� */
#define RELAY7_GPIO_PIN			GPIO_Pin_6			        /* RELAY7 */


#define RELAY8_GPIO_PORT    	GPIOC		              /* GPIO�˿� */
#define RELAY8_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO�˿�ʱ�� */
#define RELAY8_GPIO_PIN			GPIO_Pin_7			        /* RELAY8 */



#define RELAY9_GPIO_PORT    	GPIOC		              /* GPIO�˿� */
#define RELAY9_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO�˿�ʱ�� */
#define RELAY9_GPIO_PIN			GPIO_Pin_8			        /* RELAY9 */

#define RELAY10_GPIO_PORT    	GPIOC		              /* GPIO�˿� */
#define RELAY10_GPIO_CLK 	    RCC_APB2Periph_GPIOC		/* GPIO�˿�ʱ�� */
#define RELAY10_GPIO_PIN			GPIO_Pin_9			        /* RELAY10 */




/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		 {p->BRR=i;}	 //����͵�ƽ
#define digitalToggle(p,i) {p->ODR ^=i;} //�����ת״̬


/* �������IO�ĺ� */



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


//���ڱ����̵������
#define RELAY10_OFF		   digitalLo(RELAY10_GPIO_PORT,RELAY10_GPIO_PIN)
#define RELAY10_ON			   digitalHi(RELAY10_GPIO_PORT,RELAY10_GPIO_PIN)
#define RELAY10_TOGGLE		 digitalToggle(RELAY10_GPIO_PORT,RELAY10_GPIO_PIN)






typedef struct _RELAYSF
{
	volatile	uint8  dian_huo_flag;  //�������־
	volatile	uint8 gas_on_flag; //ȼ��������־
	volatile	uint8 air_on_flag; //���������־
	volatile	uint8 pai_wu_flag; //���۷�������־
	volatile	uint8 xun_huan_flag; //ѭ���ÿ�����־
	volatile	uint8 water_switch_flag; //��ˮ�ü̵���������־
	volatile	uint8 Water_Valve_Flag; //��ˮ��ŷ��̵�����������־
	

	volatile	uint8 Normal_MovieFlag; //�������������������Ĳٿر�־
		

	volatile	uint8 Flame_Flag;
	volatile	uint8 LianXu_PaiWu_flag;  //�����������ۼ̵���
	volatile	uint8 MingHuo_Flag;

	volatile	uint8 OpenWait_7ms;   //�ӳ�7ms
	volatile	uint8 CloseWait_3ms;   //�ӳ�3ms
		
}SWITCH_STATUS;





 
 
 

 
//extern unsigned char gas_on_flag;// =1 ʱ��ȼ�����鿪����0��ر�


extern SWITCH_STATUS Switch_Inf; 







void RELAYS_GPIO_Config(void);

void	Dian_Huo_Start(void);//���Ƶ��̵�������
void	Dian_Huo_OFF(void);//���Ƶ��̵����ر�

void 	Send_Gas_Open(void);  //ȼ�����鿪��
void	Send_Gas_Close(void);//ȼ������ر�


void 	Send_Air_Open(void); //���Ʒ������ //ȼ�����鿪��
void	Send_Air_Close(void);//���Ʒ���ر� //ȼ������ر�
void Feed_First_Level(void); //���ϵ�����ٶ�1��С��������
uint8  Dian_Huo_Air_Level(void);//����������


void Feed_test_begin(void);//�ֶ�ģʽ���Է��
void Feed_test_over(void);//ֹͣ�������



uint8  Feed_Main_Pump_ON(void);//�򿪲�ˮ��
uint8  Feed_Main_Pump_OFF(void); //�رղ�ˮ��

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



#endif /* __BSP_RELAYS_H */



