#include "main.h"
#include "bsp_adc.h"


float ADC_ConvertedValueLocal[NOFCHANEL];
uint16 ADC_Convert_true[M] = {0,0,0,0};

 //��4·�¶�ת��ͨ��SET0��SET1�����ƣ���Ҫһ��ʱ���������л�
extern uint8		re5_time_flag	;	//ʱ�����ɶ�ʱ��4�������ƣ�10ms�л�һ�Σ�
//�л����ص�ͬʱ������ADC_ConvertedValueLocal��ADC_Convert_true��ֵ ����ֹ����

__IO uint16_t ADC_ConvertedValue[N][M];


TEM_VALUE Temperature_Data;

/*==============================================================================
 * ����100Ω����
 * �认��4mA=400mV, 20mA=2000mV, 满��1.6MPa
 *============================================================================*/
PressureCalib_t pressureCalib = {
    .zeroMv = PRESSURE_ZERO_MV,      /* 400mV */
    .spanMv = PRESSURE_SPAN_MV,      /* 1600mV */
    .fullScale = 160,                /* 1.6MPa = 160 * 0.01 */
    .calibValid = 0x5A
};

/* IIR滤波����Q8容�� */
static uint32 pressureIirState = 0;
static uint8 pressureIirInit = 0;



 uint16 Main_Tem ;
 uint16 Sub_Tem ;
 uint16 Back_Water_Tem ;
 uint16 Smoke_Tem ;

uint8 Temperature_Channel = 0;
uint8 Channel_Now = 0;





/*==============================================================================
 * PT1000温度-阻值查找表 (0°C - 500°C)
 * 索引 = 温度(°C), 值 = 电阻×10 (单位0.1Ω)
 * 例: pt1000Table[100] = 13851 表示 100°C时电阻为1385.1Ω
 * 公式: R(t) = R0 × (1 + A×t + B×t²)
 *       R0=1000Ω, A=3.9083e-3, B=-5.775e-7
 *============================================================================*/
#define PT1000_TABLE_SIZE  501

static const uint16 pt1000Table[PT1000_TABLE_SIZE] = {
    /* 0-9°C */
    10000, 10039, 10078, 10117, 10156, 10195, 10234, 10273, 10312, 10351,
    /* 10-19°C */
    10390, 10429, 10468, 10507, 10546, 10585, 10624, 10663, 10702, 10740,
    /* 20-29°C */
    10779, 10818, 10857, 10896, 10935, 10973, 11012, 11051, 11090, 11129,
    /* 30-39°C */
    11167, 11206, 11245, 11283, 11322, 11361, 11400, 11438, 11477, 11515,
    /* 40-49°C */
    11554, 11593, 11631, 11670, 11708, 11747, 11786, 11824, 11863, 11901,
    /* 50-59°C */
    11940, 11978, 12017, 12055, 12094, 12132, 12171, 12209, 12247, 12286,
    /* 60-69°C */
    12324, 12363, 12401, 12439, 12478, 12516, 12554, 12593, 12631, 12669,
    /* 70-79°C */
    12708, 12746, 12784, 12822, 12861, 12899, 12937, 12975, 13013, 13050,
    /* 80-89°C */
    13090, 13128, 13166, 13204, 13242, 13280, 13318, 13357, 13395, 13433,
    /* 90-99°C */
    13471, 13509, 13547, 13585, 13623, 13661, 13699, 13737, 13775, 13813,
    /* 100-109°C */
    13851, 13888, 13926, 13964, 14002, 14040, 14078, 14116, 14154, 14191,
    /* 110-119°C */
    14229, 14267, 14305, 14343, 14380, 14418, 14456, 14494, 14531, 14569,
    /* 120-129°C */
    14607, 14644, 14682, 14720, 14757, 14795, 14833, 14870, 14908, 14946,
    /* 130-139°C */
    14983, 15021, 15058, 15096, 15133, 15171, 15208, 15246, 15283, 15321,
    /* 140-149°C */
    15358, 15396, 15433, 15471, 15508, 15546, 15583, 15620, 15658, 15695,
    /* 150-159°C */
    15733, 15770, 15807, 15845, 15882, 15919, 15956, 15994, 16031, 16068,
    /* 160-169°C */
    16105, 16143, 16180, 16217, 16254, 16291, 16329, 16366, 16403, 16440,
    /* 170-179°C */
    16477, 16514, 16551, 16589, 16626, 16663, 16700, 16737, 16774, 16811,
    /* 180-189°C */
    16848, 16885, 16922, 16959, 16996, 17033, 17070, 17107, 17143, 17180,
    /* 190-199°C */
    17217, 17254, 17291, 17328, 17365, 17402, 17438, 17475, 17512, 17549,
    /* 200-209°C */
    17586, 17622, 17659, 17696, 17733, 17769, 17806, 17843, 17879, 17916,
    /* 210-219°C */
    17953, 17989, 18026, 18063, 18099, 18136, 18172, 18209, 18246, 18282,
    /* 220-229°C */
    18319, 18355, 18392, 18428, 18465, 18501, 18538, 18574, 18611, 18647,
    /* 230-239°C */
    18684, 18720, 18756, 18793, 18829, 18866, 18902, 18938, 18975, 19011,
    /* 240-249°C */
    19047, 19084, 19120, 19156, 19193, 19229, 19265, 19301, 19338, 19374,
    /* 250-259°C */
    19410, 19446, 19482, 19519, 19555, 19591, 19627, 19663, 19699, 19735,
    /* 260-269°C */
    19772, 19808, 19844, 19880, 19916, 19952, 19988, 20024, 20060, 20096,
    /* 270-279°C */
    20132, 20168, 20204, 20240, 20276, 20312, 20347, 20383, 20419, 20455,
    /* 280-289°C */
    20491, 20527, 20563, 20598, 20634, 20670, 20706, 20742, 20777, 20813,
    /* 290-299°C */
    20849, 20885, 20920, 20956, 20992, 21027, 21063, 21099, 21134, 21170,
    /* 300-309°C */
    21206, 21241, 21277, 21312, 21348, 21384, 21419, 21455, 21490, 21526,
    /* 310-319°C */
    21561, 21597, 21632, 21668, 21703, 21739, 21774, 21810, 21845, 21881,
    /* 320-329°C */
    21916, 21951, 21987, 22022, 22058, 22093, 22128, 22164, 22199, 22234,
    /* 330-339°C */
    22270, 22305, 22340, 22376, 22411, 22446, 22481, 22517, 22552, 22587,
    /* 340-349°C */
    22622, 22658, 22693, 22728, 22763, 22798, 22834, 22869, 22904, 22939,
    /* 350-359°C */
    22974, 23009, 23044, 23079, 23115, 23150, 23185, 23220, 23255, 23290,
    /* 360-369°C */
    23325, 23360, 23395, 23430, 23465, 23500, 23535, 23570, 23605, 23640,
    /* 370-379°C */
    23675, 23710, 23745, 23779, 23814, 23849, 23884, 23919, 23954, 23989,
    /* 380-389°C */
    24024, 24058, 24093, 24128, 24163, 24198, 24232, 24267, 24302, 24337,
    /* 390-399°C */
    24371, 24406, 24441, 24476, 24510, 24545, 24580, 24614, 24649, 24684,
    /* 400-409°C */
    24718, 24753, 24788, 24822, 24857, 24891, 24926, 24961, 24995, 25030,
    /* 410-419°C */
    25064, 25099, 25133, 25168, 25202, 25237, 25271, 25306, 25340, 25375,
    /* 420-429°C */
    25409, 25444, 25478, 25513, 25547, 25581, 25616, 25650, 25685, 25719,
    /* 430-439°C */
    25753, 25788, 25822, 25856, 25891, 25925, 25959, 25994, 26028, 26062,
    /* 440-449°C */
    26096, 26131, 26165, 26199, 26233, 26268, 26302, 26336, 26370, 26405,
    /* 450-459°C */
    26439, 26473, 26507, 26541, 26575, 26610, 26644, 26678, 26712, 26746,
    /* 460-469°C */
    26780, 26814, 26848, 26882, 26916, 26950, 26984, 27018, 27052, 27086,
    /* 470-479°C */
    27120, 27154, 27188, 27222, 27256, 27290, 27324, 27358, 27392, 27426,
    /* 480-489°C */
    27460, 27494, 27527, 27561, 27595, 27629, 27663, 27697, 27730, 27764,
    /* 490-499°C */
    27798, 27832, 27866, 27899, 27933, 27967, 28001, 28034, 28068, 28102,
    /* 500°C */
    28135
};


/**
  * @brief  ADC GPIO ��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

//================��·4-20mA��� PA0 ,PA1===============================================//

	// �� ADC IO�˿�ʱ��
	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
	
	// ���� ADC IO ����ģʽ
	GPIO_InitStructure.GPIO_Pin = 	ADC_PIN1 | ADC_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// ��ʼ�� ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);			
	
//================��·4-20mA��� PC4��PC5===============================================//
	// �� ADC IO�˿�ʱ��
		/*����PC0 �� PC1  ���ڼ��ADS��û�����ߵ�*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 



	
	
}

/**
  * @brief  ����ADC����ģʽ
  * @param  ��
  * @retval ��
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// ��DMAʱ��
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	// ��ADCʱ��
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	
	// ��λDMA������
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// ���� DMA ��ʼ���ṹ��
	// �����ַ���ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( ADC_x->DR ) );
	
	// �洢����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	
	// ����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	DMA_InitStructure.DMA_BufferSize = N*M;
	
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// �洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	
	// �������ݴ�СΪ���֣��������ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	// �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	// ��ʼ��DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	
	// ʹ�� DMA ͨ��
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	
	// ADC ģʽ����
	// ֻʹ��һ��ADC�����ڵ�ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 

	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	// �����ⲿ����ת����������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
		
	// ��ʼ��ADC
	ADC_Init(ADC_x, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	
	// ����ADC ͨ����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL1, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL4, 4, ADC_SampleTime_239Cycles5);

	
 
	
	
	// ʹ��ADC DMA ����
	ADC_DMACmd(ADC_x, ENABLE);
	
	// ����ADC ������ʼת��
	ADC_Cmd(ADC_x, ENABLE);
	
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADC_x);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADC_x));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC_x);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC_x));
	
	// ����û�в����ⲿ����������ʹ����������ADCת�� 
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

/**
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}






/**
 * @brief  4-20mAADC轼���100Ω����
 * @param  advalue: ADC�� (0-4095)
 * @retval ���0.01MPa��160表示1.6MPa�
 * @note   100Ω����4mA=400mV, 20mA=2000mV
 *         ��
 */
uint16 GetVolt(uint16 advalue) 
{
	uint16 voltageMv = 0;
	int32 pressure = 0;
	uint16 zeroMv, spanMv, fullScale;
	
	/* ADC�转��(mV): voltage = adc * 3300 / 4096 */
	voltageMv = (uint16)((uint32)advalue * 3300 / 4096);
	sys_flag.Value_Buffer = voltageMv;
	
	/* 使�����认� */
	if(pressureCalib.calibValid == 0x5A)
	{
		zeroMv = pressureCalib.zeroMv;
		spanMv = pressureCalib.spanMv;
		fullScale = pressureCalib.fullScale;
	}
	else
	{
		/* �认��100Ω�� */
		zeroMv = PRESSURE_ZERO_MV;       /* 400mV */
		spanMv = PRESSURE_SPAN_MV;       /* 1600mV */
		fullScale = 160;                 /* 1.6MPa */
	}
	
	/* ��设���满�� */
	if(Sys_Admin.DeviceMaxPressureSet > 0 && Sys_Admin.DeviceMaxPressureSet <= 250)
	{
		fullScale = Sys_Admin.DeviceMaxPressureSet;
	}
	
	/* 线�����300mV(�3mA)认为伨� */
	if(voltageMv < PRESSURE_FAULT_LOW_MV)
	{
		return 999;  /*  */
	}
	
	/* ����容差�Ｅ��4mA�强�为0 */
	if(voltageMv >= PRESSURE_FAULT_LOW_MV && voltageMv < zeroMv)
	{
		return 0;
	}
	
	/* 线���: pressure = (voltage - zero) * fullScale / span */
	pressure = ((int32)voltageMv - zeroMv) * fullScale / spanMv;
	
	/* �� */
	if(pressure < 0)
	{
		pressure = 0;
	}
	if(pressure > 0x7FFF)
	{
		pressure = 999;
	}
	
	return (uint16)pressure;
}

/**
 * @brief  25kg�ADC轼�100Ω����
 * @param  advalue: ADC�� (0-4095)
 * @retval ���0.01MPa��250表示2.5MPa�
 */
uint16 Pressure25KG_GetVolt(uint16 advalue) 
{
	uint16 voltageMv = 0;
	int32 pressure = 0;
	
	/* ADC�转��(mV) */
	voltageMv = (uint16)((uint32)advalue * 3300 / 4096);
	sys_flag.Value_Buffer = voltageMv;
	
	/* 线�� */
	if(voltageMv < PRESSURE_FAULT_LOW_MV)
	{
		return 999;
	}
	
	/* ���差 */
	if(voltageMv < PRESSURE_ZERO_MV)
	{
		return 0;
	}
	
	/* 25kg�计�: pressure = (voltage - 400) * 250 / 1600 */
	pressure = ((int32)voltageMv - PRESSURE_ZERO_MV) * 250 / PRESSURE_SPAN_MV;
	
	if(pressure < 0) pressure = 0;
	if(pressure > 0x7FFF) pressure = 999;
	
	return (uint16)pressure;
}


void filter(void)
{
	uint32 sum = 0;
	uint8 count;
	uint8 i;
	for(i=0;i < M;i++)
	{
		for ( count=0; count < N; count++)
		{
			sum = sum + ADC_ConvertedValue[count][i];
		}
		ADC_ConvertedValueLocal[i] = sum / N;
		sum=0;
	}

}


/**
  * @brief  ����·�¶���ֵ���͵�LCD ������ͳ��ҳ����
  * @param  �¶ȱ���0--1000��equ 0-100��
  * @retval ��
  */

void Send_Adc_Lcd(void)
{

		uint16  Buffer16 = 0;
//????????  	  ??????����?????��??***************************************


	
		


		Temperature_Data.steam_value = pressure_to_temperature( Temperature_Data.Pressure_Value);

		

		
		
		
		
		
	
		
		
		


		
		
}





uint8 pressure_to_temperature(uint8 pressure)
{
	static uint8 Table[101] ={

	0,0,0,0,0, 0,0,0,0,0,100,   //0.1Mpa
	102,104,107,109,111,	113,115,116,118,120, //0.2Mpa
	121,123,124,126,127,	128,129,131,132,133,//0.3Mpa
	134,135,136,137,138,	139,140,141,143,143,//0,4Mpa
	144,145,146,147,148,	148,149,150,151,151,//0.5Mpa
	152,153,154,154,155,	156,157,157,158,158,//0.6Mpa
	160,160,161,161,162,	162,163,163,164,164,//0.7Mpa
	166,166,167,167,168,	168,169,169,170,170,//0.8Mpa
	171,171,172,172,173,	173,174,174,175,175,//0.9Mpa
	176,176,177,177,178,	178,179,179,180,180,//1.0Mpa
		};
	if(pressure <= 100)
		return Table[pressure];

	
	 
	if(pressure <= 110 && pressure > 100)
			return 184;
	if(pressure <= 120 && pressure > 110)
			return 187;
	if(pressure <= 130 && pressure > 120)
			return 191;
	if(pressure <= 140 && pressure > 130)
			return 195;
	if(pressure <= 150 && pressure > 140)
			return 198;

	if(pressure <= 160 && pressure > 150)
			return 201;
	

	 return 0 ;
}



uint16 Pressure_Filter_Function(uint16 Pressure)
{
	static uint16 Value_Buffer[21] = {0};
	uint16 Total_Value = 0;
	uint8 kdex = 0;
	static uint8 Index = 0;
	uint16 Return_Value = 0;
	 
	//��ǰѹ������ͣ¯ѹ���� �����˲����̣�������������1�룬����ʵ��ֵ��ֵ
	if(Pressure > sys_config_data.Auto_stop_pressure)
		{
			
			if(sys_flag.Pressure_HFlag == FALSE)
				{
					sys_flag.Pressure_HFlag = OK;
					sys_flag.Pressure_HCount = 0;
				}

			if(sys_flag.Pressure_HCount >= 6) //��λΪ300ms,1.8��
				Return_Value = Pressure;
			else
				Return_Value = Temperature_Data.Pressure_Value; //��ֹ����ǰѹ����ֵΪ0 
			
				
				
		}
	else
		{
			sys_flag.Pressure_HFlag = 0;
			sys_flag.Pressure_HCount = 0;
			if(Index > 4)
				Index = 0;  //��������߽�
				
			Value_Buffer[Index] = Pressure;
			Index++;
			for(kdex = 0; kdex <= 4; kdex++)  //������2���ڵ�ƽ���˲�
				Total_Value =  Total_Value + Value_Buffer[kdex];

			Total_Value = Total_Value / 5;  //Ҫע��ADCƽ���ֻ�����ʱ�䣬��ֹ������̫��
			
			Return_Value = Total_Value;
		}


	return Return_Value;
}


/*==============================================================================
 * IIR滤波����＿代�平�滤波�
 * �IIR�滤波�y[n] = 0.75*y[n-1] + 0.25*x[n]
 * 滤波系��=0.25Ｖ�常�约4丷��
 *============================================================================*/

/**
 * @brief  �IIR�滤波�信���
 * @param  rawPressure: 姼�0.01MPa�
 * @retval 滤波
 * @note   使�Q8容���浹��
 *         �����迶���
 */
uint16 Pressure_IIR_Filter(uint16 rawPressure)
{
	uint16 filteredValue;
	
	/* ���模���迶跳�滤波 */
	if(rawPressure > sys_config_data.Auto_stop_pressure)
	{
		if(sys_flag.Pressure_HFlag == FALSE)
		{
			sys_flag.Pressure_HFlag = OK;
			sys_flag.Pressure_HCount = 0;
		}
		
		/* ���认���� */
		if(sys_flag.Pressure_HCount >= 6)
		{
			pressureIirState = (uint32)rawPressure << 8;
			return rawPressure;
		}
		else
		{
			return Temperature_Data.Pressure_Value;
		}
	}
	
	/* 正常模��IIR滤波 */
	sys_flag.Pressure_HFlag = 0;
	sys_flag.Pressure_HCount = 0;
	
	/* �次�� */
	if(!pressureIirInit)
	{
		pressureIirState = (uint32)rawPressure << 8;
		pressureIirInit = 1;
		return rawPressure;
	}
	
	/* IIR滤波计��Q8容�
	 * filtered = 0.75 * filtered + 0.25 * new
	 * �移�: 0.75  192/256, 0.25  64/256
	 */
	pressureIirState = (pressureIirState * 192 + (uint32)rawPressure * 64) >> 8;
	
	/* 轢�� */
	filteredValue = (uint16)(pressureIirState >> 8);
	
	return filteredValue;
}

/**
 * @brief  �IIR滤波���
 * @note   系����
 */
void Pressure_Filter_Reset(void)
{
	pressureIirState = 0;
	pressureIirInit = 0;
}

/*==============================================================================
 * ����
 *============================================================================*/

/**
 * @brief  姡��使��认��
 */
void Pressure_Calib_Init(void)
{
	/* �Flash両���此�使��认� */
	if(pressureCalib.calibValid != 0x5A)
	{
		pressureCalib.zeroMv = PRESSURE_ZERO_MV;
		pressureCalib.spanMv = PRESSURE_SPAN_MV;
		pressureCalib.fullScale = 160;
		pressureCalib.calibValid = 0x5A;
	}
}

/**
 * @brief  �����4mA�����
 * @param  currentAdcMv: �ADC浵�(mV)
 * @note   ���4mA��0Ｖ�
 */
void Pressure_Calibrate_Zero(uint16 currentAdcMv)
{
	/* �������300-600mV */
	if(currentAdcMv >= 300 && currentAdcMv <= 600)
	{
		pressureCalib.zeroMv = currentAdcMv;
		pressureCalib.calibValid = 0x5A;
		
		/* TODO: ��Flash */
		// SaveCalibToFlash(&pressureCalib);
		
		/* �滤波 */
		Pressure_Filter_Reset();
	}
}

/**
 * @brief  满�稡��20mA�����
 * @param  currentAdcMv: �ADC浵�(mV)
 * @param  actualPressure: 宼�0.01MPa�
 * @note   ���20mA�满��Ｖ�
 */
void Pressure_Calibrate_Full(uint16 currentAdcMv, uint16 actualPressure)
{
	uint16 newSpan;
	
	/* ���满�稵��1500-2500mV */
	if(currentAdcMv >= 1500 && currentAdcMv <= 2500 && actualPressure > 0)
	{
		newSpan = currentAdcMv - pressureCalib.zeroMv;
		
		/* 跨度�� */
		if(newSpan >= 1000 && newSpan <= 2000)
		{
			pressureCalib.spanMv = newSpan;
			pressureCalib.fullScale = actualPressure;
			pressureCalib.calibValid = 0x5A;
			
			/* TODO: ��Flash */
			// SaveCalibToFlash(&pressureCalib);
		}
	}
}

/**
 * @brief  使����轢����IIR滤波�
 * @param  advalue: ADC��
 * @retval 滤波��0.01MPa�
 */
uint16 GetVoltCalibrated(uint16 advalue)
{
	uint16 rawPressure;
	
	/* � */
	rawPressure = GetVolt(advalue);
	
	/* 滤波 */
	return Pressure_IIR_Filter(rawPressure);
}


uint16 WenDu_Filter_Function(uint16 New_Value)
{
	static uint16 Value_Buffer[21] = {0};
	uint16 Total_Value = 0;
	uint16 Return_Value = 0;
	uint8 kdex = 0;
	static uint8 Index = 0;
	 
	//���¶ȴ���400�ȣ� �����˲����̣�������������1�룬����ʵ��ֵ��ֵ
	if(New_Value > 4000)
		{
			if(sys_flag.WenDu_HFlag == FALSE)
				{
					sys_flag.WenDu_HFlag = OK;
					sys_flag.WenDu_HCount = 0;
				}

			if(sys_flag.WenDu_HCount >= 6) //��λΪ300ms,1.8��
				Return_Value = New_Value;
				
		}
	else
		{
			sys_flag.WenDu_HFlag = 0;
			sys_flag.WenDu_HCount = 0;
			if(Index >= 20)
				Index = 0;  //��������߽�
				
			Value_Buffer[Index] = New_Value;
			Index++;
			for(kdex = 0; kdex <= 19; kdex++)  //������2���ڵ�ƽ���˲�
				Total_Value =  Total_Value + Value_Buffer[kdex];

			Total_Value = Total_Value / 20;  //Ҫע��ADCƽ���ֻ�����ʱ�䣬��ֹ������̫��
			Return_Value = Total_Value;
		}
	

	return Return_Value;
}


uint8 Check_ADS_Unconnect(uint16 New_Value)
{
	//	static uint16 Old_State = OK;
//	static uint16 Old_Value = 0;
	
	/*	
	if(ABS(New_Value - Old_Value) > 30)
		{
			Old_Value = New_Value;
			sys_flag.AdsUnconnect_Count++ ;
			
		}

	if(sys_flag.AdsUnconnect_Flag)
		{
			sys_flag.AdsUnconnect_Flag = 0;
			
			if(sys_flag.AdsUnconnect_Count > 9)
				Old_State = FALSE;  //״̬������
			else
				Old_State = OK;

			sys_flag.AdsUnconnect_Count = 0;
				
		}
	
	*/
	
		return 0;
		
	
}



/**
  * @brief  ADC��ֵƽ���˲�������
  * @param  �¶ȱ���0--1000��equ 0-100��
  * @retval ��
  */
uint8  ADC_Process(void)
{
		uint8 i = 0;
		//uint16 temp = 0;
		uint16 Value_Buffer = 0;
		uint32 ADC1220_Value = 0;
		float ResData = 0;
		 uint32 Data =0;
		
	
			
		if(sys_flag.ADC_100msFlag)
			{

				sys_flag.ADC_100msFlag = 0;
				
				filter();  //ADC�˲�  //�¶�Ĭ��ͨ����
				for(i = 0; i < M; i++)
				{
					
					ADC_Convert_true[i] = ADC_ConvertedValueLocal[i];
				}

			
				
				//ADS1220 �¶ȵ����ⷽʽ
				 //PE13  DRDY��Ϊ�͵�ƽ�������
				 if(PortPin_Scan(GPIOE,GPIO_Pin_13) == 0)
				 	{
				 		sys_flag.Value_Buffer2++;
				 		Data = ADS1220ReadData();
						ResData = Data / 8388607.000 * 2000 ;//��һLSB���㹫ʽ
				 		ResData = ResData /16/0.5;
				 		Data = 	ResData * 100;//��100�������в��
				 		ADC1220_Value = Adc_to_temperature(Data);
						Temperature_Data.Smoke_Tem  = WenDu_Filter_Function(ADC1220_Value);
						if(Temperature_Data.Smoke_Tem > 100)
							Temperature_Data.Smoke_Tem = Temperature_Data.Smoke_Tem - 25; //��2.5��
				 	}
				 ResData = Temperature_Data.Smoke_Tem; //�����С��
				 LCD10D.DLCD.Smoke_WenDu = ResData / 10;
				
				
				
				

				// ADC_ConvertedValueLocal[1] 对�1, ADC_ConvertedValueLocal[0] 对�2
				// 使��IIR滤波���100Ω����4mA=400mV�
				Value_Buffer = GetVolt(ADC_ConvertedValueLocal[1]);
				Temperature_Data.Pressure_Value = Pressure_IIR_Filter(Value_Buffer);
				Temperature_Data.steam_value = pressure_to_temperature( Temperature_Data.Pressure_Value);

				if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3 )  //������������ж�
					{
						Temperature_Data.Inside_High_Pressure = Pressure25KG_GetVolt(ADC_ConvertedValueLocal[0]);
						ResData = Temperature_Data.Inside_High_Pressure;
						LCD10D.DLCD.Inside_High_Pressure = ResData / 100; //�����С��
					}
				else
					Temperature_Data.Inside_High_Pressure = 0;
				

				ResData = Temperature_Data.Pressure_Value;
				LCD10D.DLCD.Steam_Pressure = ResData / 100; //�����С��
				LCD10D.DLCD.Steam_WenDu = Temperature_Data.steam_value;
				
				
				/*���PC0 ��PC1 ��ֵ�ж� ADS��û������ȷ��ȡ�����ֵ��δ�ӵ�ʱ������ֵ����С������������500���ڣ�PC0 С��PC1 ��ֵ

				��һ�֣�*�����¶ȵ�ֵ= 247

				*PC__00 ADC ��ֵ= 2510

				*PC__11 ADC ��ֵ= 2578

				�ڶ��֣����������û��PC0��PC1��ֵ�ӽ�4096

				�����֣����������߽����ߵ�����
				*PC__00 ADC ��ֵ= 1260
				*PC__11 ADC ��ֵ= 4077

				*/

				sys_flag.PC0_ADC = ADC_ConvertedValueLocal[2];
				sys_flag.PC1_ADC = ADC_ConvertedValueLocal[3];


				Send_Adc_Lcd();

			}

		
			
   	return 0;
	/***************************************/	 
}



uint16 find_true_temperature(uint16 value)
{
		float V_adc = 0 ;
		float V_RT  = 0;
		float RT = 0;
		float V_ref = 3.3;
		float adc = 0;
		float temp = 0;

		uint16 RT_value = 0;

		adc = (float)value;
		V_adc = (float)(adc/4096 *V_ref);	

		temp = (float)(364)/(float)(2091);
		V_RT = (float)((V_adc / 20) + temp);

		RT = (float)(2 * V_RT)/(4 - V_RT);

		RT_value = RT *100000; //�Ե���ֵ�Ŵ�ʮ�򱶣�ȡС��������
		//printf("\r\nPT100����ֵΪ = %d\r\n",RT_value);
	
	
		return  RT_value;
		
}

/**
 * @brief  ADC��转温度�对���
 * @param  value: PT1000����10
 * @retval 温度��10 (�250表示25.0°C), ���9999
 */
uint16 Adc_to_temperature(uint16 value)
{
    /* 线��［���� */
    if(value < pt1000Table[0] - 500)
    {
        return 9999;
    }
    
    return Pt1000ToTemperature(value);
}




/**
 * @brief  线�����
 * @param  value: 彼���
 * @param  span: ��跨�
 * @retval �� (0-10)
 */
static uint16 getInterpolation(uint16 value, uint16 span)
{
    if(span == 0) return 0;
    return (value * 10) / span;
}

/**
 * @brief  PT1000阻值转温度（二分查找+线性插值）
 * @param  resistance: 电阻值×10 (如10000表示1000.0Ω)
 * @retval 温度值×10 (如2500表示250.0°C)
 * @note   二分查找O(log n)，最多9次比较（501个元素）
 */
uint16 Pt1000ToTemperature(uint16 resistance)
{
    uint16 low = 0;
    uint16 high = PT1000_TABLE_SIZE - 1;  /* 500 */
    uint16 mid;
    uint16 span;
    
    /* 边界检查：低于0°C */
    if(resistance < pt1000Table[0])
    {
        return 0;
    }
    
    /* 边界检查：高于500°C */
    if(resistance >= pt1000Table[PT1000_TABLE_SIZE - 1])
    {
        return 5000;
    }
    
    /* 二分查找：找到resistance所在区间 */
    while(low < high)
    {
        mid = (low + high) / 2;
        
        if(resistance < pt1000Table[mid])
        {
            high = mid;
        }
        else if(resistance >= pt1000Table[mid + 1])
        {
            low = mid + 1;
        }
        else
        {
            /* 找到区间: pt1000Table[mid] <= resistance < pt1000Table[mid+1] */
            low = mid;
            break;
        }
    }
    
    /* 线性插值 */
    span = pt1000Table[low + 1] - pt1000Table[low];
    
    return low * 10 + getInterpolation(resistance - pt1000Table[low], span);
}



/*********************************************END OF FILE**********************/

