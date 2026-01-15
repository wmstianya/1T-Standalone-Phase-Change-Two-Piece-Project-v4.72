#ifndef __ADC_H
#define	__ADC_H


#include "stm32f10x.h"
#include "main.h"

#define N 50    //����50��
#define M 4     //ͨ����Ϊ3



#define ABS(X)  ((X)>0? (X): -(X))


// ע�⣺����ADC�ɼ���IO����û�и��ã�����ɼ���ѹ����Ӱ��
/********************ADC1����ͨ�������ţ�����**************************/
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    ADC_CLK                       RCC_APB2Periph_ADC1

#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOA  
#define    ADC_PORT                      GPIOA


/*******************�¶�ADC����**************************/

#define    ADC_GPIO_APBxClock_FUN1        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK1                 RCC_APB2Periph_GPIOC  
#define    ADC_PORT1                      GPIOC






/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		 {p->BRR=i;}	 //����͵�ƽ


/* �������IO�ĺ� */





// ת��ͨ������
#define    NOFCHANEL	 4

#define    ADC_PIN1                      GPIO_Pin_0
#define    ADC_CHANNEL1                  ADC_Channel_0

#define    ADC_PIN2                      GPIO_Pin_1
#define    ADC_CHANNEL2                  ADC_Channel_1



#define    ADC_PIN3                      GPIO_Pin_0
#define    ADC_CHANNEL3                  ADC_Channel_10

#define    ADC_PIN4                      GPIO_Pin_1
#define    ADC_CHANNEL4                 ADC_Channel_11




// ADC1 ��Ӧ DMA1ͨ��1��ADC3��ӦDMA2ͨ��5��ADC2û��DMA����
#define    ADC_x                         ADC1
#define    ADC_DMA_CHANNEL               DMA1_Channel1
#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA1


/*==============================================================================
 * 压力变送器4-20mA参数定义（100Ω采样电阻）
 * 4mA = 400mV, 20mA = 2000mV
 *============================================================================*/
#define PRESSURE_ZERO_MV          400     /* 4mA对应电压(mV) */
#define PRESSURE_SPAN_MV          1600    /* 电压跨度(mV): 2000-400 */
#define PRESSURE_FAULT_LOW_MV     300     /* 低于此值认为断线 */
#define PRESSURE_FAULT_HIGH_MV    2200    /* 高于此值认为短路 */

/* 校准参数结构体 */
typedef struct {
    uint16 zeroMv;           /* 零点电压(mV)，默认400 */
    uint16 spanMv;           /* 电压跨度(mV)，默认1600 */
    uint16 fullScale;        /* 满量程压力值(0.01MPa)，如160=1.6MPa */
    uint8  calibValid;       /* 校准有效标志 0x5A=有效 */
} PressureCalib_t;

extern PressureCalib_t pressureCalib;


//��·���½ṹ��
typedef struct _TEMPURE_
{
		  uint16 Main_Tem ;
		  uint16 Out_Water_Tem ;
		  uint16 Back_Water_Tem ;
		  uint16 Smoke_Tem ;
			uint16 Pressure_Value;//ϵͳѹ��
			uint16 steam_value; //�����¶�
			uint16 inside_water_value;  //�ڲ�ˮѭ�����¶�
			uint16 Inside_High_Pressure;  //�ڲ�
}TEM_VALUE;

extern TEM_VALUE Temperature_Data;

// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue[N][M];

extern float ADC_ConvertedValueLocal[NOFCHANEL];
extern	uint16 ADC_Convert_true[NOFCHANEL];



extern uint16 Main_Tem ;
extern uint16 Sub_Tem ;
extern uint16 Back_Water_Tem ;
extern uint16 Smoke_Tem ;


/**************************��������********************************/
void 	ADCx_Init(void);
void filter(void);
uint16 GetVolt(uint16 advalue);
uint16 Pressure25KG_GetVolt(uint16 advalue) ;

uint8  ADC_Process(void);
void Send_Adc_Lcd(void);
uint16 get_dot(uint16 value,uint16 percent);
uint8 pressure_to_temperature(uint8 pressure);

uint8 Check_ADS_Unconnect(uint16 New_Value);

/*==============================================================================
 * 压力IIR滤波与校准函数
 *============================================================================*/
uint16 Pressure_IIR_Filter(uint16 rawPressure);
void Pressure_Filter_Reset(void);
void Pressure_Calibrate_Zero(uint16 currentAdcMv);
void Pressure_Calibrate_Full(uint16 currentAdcMv, uint16 actualPressure);
void Pressure_Calib_Init(void);
uint16 GetVoltCalibrated(uint16 advalue);

uint16 find_true_temperature(uint16 value);

/*==============================================================================
 * PT1000温度转换函数（重构后）
 *============================================================================*/
uint16 Adc_to_temperature(uint16 value);
uint16 Pt1000ToTemperature(uint16 resistance);



#endif /* __ADC_H */


