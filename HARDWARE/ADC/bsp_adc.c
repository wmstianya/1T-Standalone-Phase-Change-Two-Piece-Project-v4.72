#include "main.h"
#include "bsp_adc.h"


float ADC_ConvertedValueLocal[NOFCHANEL];
uint16 ADC_Convert_true[M] = {0,0,0,0};

 //因4路温度转换通过SET0，SET1来控制，需要一个时间量进行切换
extern uint8		re5_time_flag	;	//时间量由定时器4，来控制，10ms切换一次，
//切换开关的同时需清零ADC_ConvertedValueLocal和ADC_Convert_true的值 ，防止误用

__IO uint16_t ADC_ConvertedValue[N][M];


TEM_VALUE Temperature_Data;



 uint16 Main_Tem ;
 uint16 Sub_Tem ;
 uint16 Back_Water_Tem ;
 uint16 Smoke_Tem ;

uint8 Temperature_Channel = 0;
uint8 Channel_Now = 0;





const uint16 adc_00 = 10000;//零度为推断值
	const uint16 adc_01 = 10039;
	const uint16 adc_02 = 10078;
	const uint16 adc_03 = 10117;
	const uint16 adc_04 = 10156;
	const uint16 adc_05 = 10195;
	const uint16 adc_06 = 10234;
	const uint16 adc_07 = 10273;
	const uint16 adc_08 = 10312;
	const uint16 adc_09 = 10351;
	
	
const uint16 adc_10 = 10390;
	const uint16 adc_11 = 10429;
	const uint16 adc_12 = 10468;
	const uint16 adc_13 = 10507;
	const uint16 adc_14 = 10546;
	const uint16 adc_15 = 10585;
	const uint16 adc_16 = 10624;
	const uint16 adc_17 = 10663;
	const uint16 adc_18 = 10702;
	const uint16 adc_19 = 10740;
const uint16 adc_20 = 10779;
	const uint16 adc_21 = 10818;
	const uint16 adc_22 = 10857;
	const uint16 adc_23 = 10896;
	const uint16 adc_24 = 10935;
	const uint16 adc_25 = 10973;
	const uint16 adc_26 = 11012;
	const uint16 adc_27 = 11051;
	const uint16 adc_28 = 11090;
	const uint16 adc_29 = 11129;
const uint16 adc_30 = 11167;
	const uint16 adc_31 = 11206;
	const uint16 adc_32 = 11245;
	const uint16 adc_33 = 11283;
	const uint16 adc_34 = 11322;
	const uint16 adc_35 = 11361;
	const uint16 adc_36 = 11400;
	const uint16 adc_37 = 11438;
	const uint16 adc_38 = 11477;
	const uint16 adc_39 = 11515;
const uint16 adc_40 = 11554;
	const uint16 adc_41 = 11593;
	const uint16 adc_42 = 11631;
	const uint16 adc_43 = 11670;
	const uint16 adc_44 = 11708;
	const uint16 adc_45 = 11747;
	const uint16 adc_46 = 11786;
	const uint16 adc_47 = 11824;
	const uint16 adc_48 = 11863;
	const uint16 adc_49 = 11901;
const uint16 adc_50 = 11940;
	const uint16 adc_51 = 11978;
	const uint16 adc_52 = 12017;
	const uint16 adc_53 = 12055;
	const uint16 adc_54 = 12094;
	const uint16 adc_55 = 12132;
	const uint16 adc_56 = 12171;
	const uint16 adc_57 = 12209;
	const uint16 adc_58 = 12247;
	const uint16 adc_59 = 12286;
const uint16 adc_60 = 12324;
	const uint16 adc_61 = 12363;
	const uint16 adc_62 = 12401;
	const uint16 adc_63 = 12439;
	const uint16 adc_64 = 12478;
	const uint16 adc_65 = 12516;
	const uint16 adc_66 = 12554;
	const uint16 adc_67 = 12593;
	const uint16 adc_68 = 12631;
	const uint16 adc_69 = 12669;
const uint16 adc_70 = 12708;
	const uint16 adc_71 = 12746;
	const uint16 adc_72 = 12784;
	const uint16 adc_73 = 12822;
	const uint16 adc_74 = 12861;
	const uint16 adc_75 = 12899;
	const uint16 adc_76 = 12937;
	const uint16 adc_77 = 12975;
	const uint16 adc_78 = 13013;
	const uint16 adc_79 = 13050;
const uint16 adc_80 = 13090;
	const uint16 adc_81 = 13128;
	const uint16 adc_82 = 13166;
	const uint16 adc_83 = 13204;
	const uint16 adc_84 = 13242;
	const uint16 adc_85 = 13280;
	const uint16 adc_86 = 13318;
	const uint16 adc_87 = 13357;
	const uint16 adc_88 = 13395;
	const uint16 adc_89 = 13433;
const uint16 adc_90 = 13471;
	const uint16 adc_91 = 13509;
	const uint16 adc_92 = 13547;
	const uint16 adc_93 = 13585;
	const uint16 adc_94 = 13623;
	const uint16 adc_95 = 13661;
	const uint16 adc_96 = 13699;
	const uint16 adc_97 = 13737;
	const uint16 adc_98 = 13775;
	const uint16 adc_99 = 13813;
const uint16 adc_100 =13851;
	const uint16 adc_101 = 13888;
	const uint16 adc_102 = 13926;
	const uint16 adc_103 = 13964;
	const uint16 adc_104 = 14002;
	const uint16 adc_105 = 14040;
	const uint16 adc_106 = 14078;
	const uint16 adc_107 = 14116;
	const uint16 adc_108 = 14154;
	const uint16 adc_109 = 14191;
const uint16 adc_110 =14229;
	const uint16 adc_111 = 14267;
	const uint16 adc_112 = 14305;
	const uint16 adc_113 = 14343;
	const uint16 adc_114 = 14380;
	const uint16 adc_115 = 14418;
	const uint16 adc_116 = 14456;
	const uint16 adc_117 = 14494;
	const uint16 adc_118 = 14531;
	const uint16 adc_119 = 14569;
const uint16 adc_120 =14607;
	const uint16 adc_121 = 14644;
	const uint16 adc_122 = 14682;
	const uint16 adc_123 = 14720;
	const uint16 adc_124 = 14757;
	const uint16 adc_125 = 14795;
	const uint16 adc_126 = 14833;
	const uint16 adc_127 = 14870;
	const uint16 adc_128 = 14908;
	const uint16 adc_129 = 14946;
const uint16 adc_130 =14983;
	const uint16 adc_131 = 15021;
	const uint16 adc_132 = 15058;
	const uint16 adc_133 = 15096;
	const uint16 adc_134 = 15133;
	const uint16 adc_135 = 15171;
	const uint16 adc_136 = 15208;
	const uint16 adc_137 = 15246;
	const uint16 adc_138 = 15283;
	const uint16 adc_139 = 15321;
const uint16 adc_140 =15358;
	const uint16 adc_141 = 15396;
	const uint16 adc_142 = 15433;
	const uint16 adc_143 = 15471;
	const uint16 adc_144 = 15508;
	const uint16 adc_145 = 15546;
	const uint16 adc_146 = 15583;
	const uint16 adc_147 = 15620;
	const uint16 adc_148 = 15658;
	const uint16 adc_149 = 15695;
const uint16 adc_150 =15733;
	const uint16 adc_151 = 15770;
	const uint16 adc_152 = 15807;
	const uint16 adc_153 = 15845;
	const uint16 adc_154 = 15882;
	const uint16 adc_155 = 15919;
	const uint16 adc_156 = 15956;
	const uint16 adc_157 = 16031;
	const uint16 adc_158 = 16068;
	const uint16 adc_159 = 16105;
const uint16 adc_160 =16105;
	const uint16 adc_161 = 16143;
	const uint16 adc_162 = 16180;
	const uint16 adc_163 = 16217;
	const uint16 adc_164 = 16254;
	const uint16 adc_165 = 16291;
	const uint16 adc_166 = 16329;
	const uint16 adc_167 = 16366;
	const uint16 adc_168 = 16403;
	const uint16 adc_169 = 16440;
const uint16 adc_170 =16477;
	const uint16 adc_171 = 16514;
	const uint16 adc_172 = 16551;
	const uint16 adc_173 = 16589;
	const uint16 adc_174 = 16626;
	const uint16 adc_175 = 16663;
	const uint16 adc_176 = 16700;
	const uint16 adc_177 = 16737;
	const uint16 adc_178 = 16774;
	const uint16 adc_179 = 16811;
const uint16 adc_180 =16848;
	const uint16 adc_181 = 16885;
	const uint16 adc_182 = 16922;
	const uint16 adc_183 = 16959;
	const uint16 adc_184 = 16996;
	const uint16 adc_185 = 17033;
	const uint16 adc_186 = 17070;
	const uint16 adc_187 = 17107;
	const uint16 adc_188 = 17143;
	const uint16 adc_189 = 17180;
const uint16 adc_190 =17217;
	const uint16 adc_191 = 17254;
	const uint16 adc_192 = 17291;
	const uint16 adc_193 = 17328;
	const uint16 adc_194 = 17365;
	const uint16 adc_195 = 17402;
	const uint16 adc_196 = 17438;
	const uint16 adc_197 = 17475;
	const uint16 adc_198 = 17512;
	const uint16 adc_199 = 17549;

const uint16 adc_200 =17586;
	const uint16 adc_201 = 17622;
	const uint16 adc_202 = 17659;
	const uint16 adc_203 = 17696;
	const uint16 adc_204 = 17733;
	const uint16 adc_205 = 17769;
	const uint16 adc_206 = 17806;
	const uint16 adc_207 = 17843;
	const uint16 adc_208 = 17879;
	const uint16 adc_209 = 17916;

const uint16 adc_210 =17953;
	const uint16 adc_211 = 17989;
	const uint16 adc_212 = 18026;
	const uint16 adc_213 = 18063;
	const uint16 adc_214 = 18099;
	const uint16 adc_215 = 18136;
	const uint16 adc_216 = 18172;
	const uint16 adc_217 = 18209;
	const uint16 adc_218 = 18246;
	const uint16 adc_219 = 18282;
	
const uint16 adc_220 =18319;


/**
  * @brief  ADC GPIO 初始化
  * @param  无
  * @retval 无
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

//================两路4-20mA检测 PA0 ,PA1===============================================//

	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
	
	// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Pin = 	ADC_PIN1 | ADC_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化 ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);			
	
//================两路4-20mA检测 PC4，PC5===============================================//
	// 打开 ADC IO端口时钟
		/*配置PC0 和 PC1  用于检测ADS有没有在线的*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 



	
	
}

/**
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// 打开DMA时钟
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	// 打开ADC时钟
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	
	// 复位DMA控制器
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// 配置 DMA 初始化结构体
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( ADC_x->DR ) );
	
	// 存储器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	
	// 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	// 缓冲区大小，应该等于数据目的地的大小
	DMA_InitStructure.DMA_BufferSize = N*M;
	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	
	// 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	// 内存数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	// 初始化DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	
	// 使能 DMA 通道
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	
	// ADC 模式配置
	// 只使用一个ADC，属于单模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 

	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
		
	// 初始化ADC
	ADC_Init(ADC_x, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	
	// 配置ADC 通道的转换顺序和采样时间
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL1, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL4, 4, ADC_SampleTime_239Cycles5);

	
 
	
	
	// 使能ADC DMA 请求
	ADC_DMACmd(ADC_x, ENABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(ADC_x, ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC_x);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC_x));
	
	// ADC开始校准
	ADC_StartCalibration(ADC_x);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC_x));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

/**
  * @brief  ADC初始化
  * @param  无
  * @retval 无
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}






uint16 GetVolt(uint16 advalue) 

{

	
	uint16 pressure =0;
	
	//参考电压
	pressure = (uint16)(advalue * 3300 / 4096);//对ADC值扩大1000倍，方便取小数

	sys_flag.Value_Buffer = pressure;
	 
	if(pressure > 380 && pressure < 559)//针对压力变送器可能损坏，最大误差0.03Mpa
		pressure = 560;

	//满量程1.60Mpa, = 160,精确到0.01Mpa  这个14 =14 = （1679 -559）/ 80
	
	if(Sys_Admin.DeviceMaxPressureSet <= 160)
		pressure = (pressure - 559) * 10/140;  //16公斤以内的换算

	//sys_flag.Pressure_buffer = (pressure - 559) * 100/140;  //精确到0.001

	if(Sys_Admin.DeviceMaxPressureSet > 160  && Sys_Admin.DeviceMaxPressureSet <= 250)
		{
			pressure = (pressure - 559)* 25/224 ;//25公斤的换算
		}
		
	  if(pressure > 0x8000)
	  	{
	  		pressure = 999;
			
	  	}
			  	
	return pressure; 

}

uint16 Pressure25KG_GetVolt(uint16 advalue) 

{
	uint16 pressure =0;
	
	//参考电压
	pressure = (uint16)(advalue * 3300 / 4096);//对ADC值扩大1000倍，方便取小数

	sys_flag.Value_Buffer = pressure;
	 
	if(pressure > 380 && pressure < 559)//针对压力变送器可能损坏，最大误差0.03Mpa
		pressure = 560;

	//满量程1.60Mpa, = 160,精确到0.01Mpa  这个14 =14 = （1679 -559）/ 80
	
	
	pressure = (pressure - 559)* 25/224 ;//25公斤的换算
			 

	  if(pressure > 0x8000)
	  	{
	  		pressure = 999;
			
	  	}
			  	
	return pressure; 

}


uint16 New_GetVolt(uint16 advalue) 

{

	
	uint16 pressure =0;
	
	//参考电压
	pressure = (uint16)(advalue * 3300 / 4096);//对ADC值扩大1000倍，方便取小数

	//sys_flag.Value_Buffer2 = advalue;

	


	 
	if(pressure > 550 && pressure < 670)//针对压力变送器可能损坏，最大误差0.03Mpa
		pressure = 674;

	
	
	pressure = (pressure - 673) * 10/138;//满量程1.60Mpa, = 160,精确到0.01Mpa  这个13.8 =13.78 = （1776 -673）/ 80

			  

	  if(pressure > 0x8000)
	  	{
	  		pressure = 999;
			
	  	}
			  	
	return pressure; 

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
  * @brief  将四路温度数值发送到LCD 和数据统计页面上
  * @param  温度变量0--1000，equ 0-100度
  * @retval 无
  */

void Send_Adc_Lcd(void)
{

		uint16  Buffer16 = 0;
//????????  	  ??????±í?????ú??***************************************


	
		


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
	 
	//当前压力大于停炉压力， 启动滤波流程，若果持续超过1秒，则将真实的值赋值
	if(Pressure > sys_config_data.Auto_stop_pressure)
		{
			
			if(sys_flag.Pressure_HFlag == FALSE)
				{
					sys_flag.Pressure_HFlag = OK;
					sys_flag.Pressure_HCount = 0;
				}

			if(sys_flag.Pressure_HCount >= 6) //单位为300ms,1.8秒
				Return_Value = Pressure;
			else
				Return_Value = Temperature_Data.Pressure_Value; //防止给当前压力赋值为0 
			
				
				
		}
	else
		{
			sys_flag.Pressure_HFlag = 0;
			sys_flag.Pressure_HCount = 0;
			if(Index > 4)
				Index = 0;  //控制溢出边界
				
			Value_Buffer[Index] = Pressure;
			Index++;
			for(kdex = 0; kdex <= 4; kdex++)  //控制在2秒内的平滑滤波
				Total_Value =  Total_Value + Value_Buffer[kdex];

			Total_Value = Total_Value / 5;  //要注意ADC平均轮回周期时间，防止敏感性太差
			
			Return_Value = Total_Value;
		}


	return Return_Value;
}


uint16 WenDu_Filter_Function(uint16 New_Value)
{
	static uint16 Value_Buffer[21] = {0};
	uint16 Total_Value = 0;
	uint16 Return_Value = 0;
	uint8 kdex = 0;
	static uint8 Index = 0;
	 
	//当温度大于400度， 启动滤波流程，若果持续超过1秒，则将真实的值赋值
	if(New_Value > 4000)
		{
			if(sys_flag.WenDu_HFlag == FALSE)
				{
					sys_flag.WenDu_HFlag = OK;
					sys_flag.WenDu_HCount = 0;
				}

			if(sys_flag.WenDu_HCount >= 6) //单位为300ms,1.8秒
				Return_Value = New_Value;
				
		}
	else
		{
			sys_flag.WenDu_HFlag = 0;
			sys_flag.WenDu_HCount = 0;
			if(Index >= 20)
				Index = 0;  //控制溢出边界
				
			Value_Buffer[Index] = New_Value;
			Index++;
			for(kdex = 0; kdex <= 19; kdex++)  //控制在2秒内的平滑滤波
				Total_Value =  Total_Value + Value_Buffer[kdex];

			Total_Value = Total_Value / 20;  //要注意ADC平均轮回周期时间，防止敏感性太差
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
				Old_State = FALSE;  //状态出错拉
			else
				Old_State = OK;

			sys_flag.AdsUnconnect_Count = 0;
				
		}
	
	*/
	
		return 0;
		
	
}



/**
  * @brief  ADC数值平均滤波，处理
  * @param  温度变量0--1000，equ 0-100度
  * @retval 无
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
				
				filter();  //ADC滤波  //温度默认通道零
				for(i = 0; i < M; i++)
				{
					
					ADC_Convert_true[i] = ADC_ConvertedValueLocal[i];
				}

			
				
				//ADS1220 温度电阻检测方式
				 //PE13  DRDY脚为低电平，则读，
				 if(PortPin_Scan(GPIOE,GPIO_Pin_13) == 0)
				 	{
				 		sys_flag.Value_Buffer2++;
				 		Data = ADS1220ReadData();
						ResData = Data / 8388607.000 * 2000 ;//单一LSB计算公式
				 		ResData = ResData /16/0.5;
				 		Data = 	ResData * 100;//乘100倍，进行查表
				 		ADC1220_Value = Adc_to_temperature(Data);
						Temperature_Data.Smoke_Tem  = WenDu_Filter_Function(ADC1220_Value);
						if(Temperature_Data.Smoke_Tem > 100)
							Temperature_Data.Smoke_Tem = Temperature_Data.Smoke_Tem - 25; //减2.5度
				 	}
				 ResData = Temperature_Data.Smoke_Tem; //换算成小数
				 LCD10D.DLCD.Smoke_WenDu = ResData / 10;
				
				
				
				

				// ADC_ConvertedValueLocal[1] 对应压力变送器1的值 ，ADC_ConvertedValueLocal[0] 对应压力变送器2值
				Value_Buffer = GetVolt(ADC_ConvertedValueLocal[1]);
				Temperature_Data.Pressure_Value = Pressure_Filter_Function(Value_Buffer);
				Temperature_Data.steam_value = pressure_to_temperature( Temperature_Data.Pressure_Value);

				if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3 )  //相变蒸汽机组判定
					{
						Temperature_Data.Inside_High_Pressure = Pressure25KG_GetVolt(ADC_ConvertedValueLocal[0]);
						ResData = Temperature_Data.Inside_High_Pressure;
						LCD10D.DLCD.Inside_High_Pressure = ResData / 100; //换算成小数
					}
				else
					Temperature_Data.Inside_High_Pressure = 0;
				

				ResData = Temperature_Data.Pressure_Value;
				LCD10D.DLCD.Steam_Pressure = ResData / 100; //换算成小数
				LCD10D.DLCD.Steam_WenDu = Temperature_Data.steam_value;
				
				
				/*检测PC0 和PC1 的值判定 ADS有没有在正确读取电阻的值，未接的时候，两个值相差很小，如果接了相差500以内，PC0 小于PC1 的值

				第一种：*测试温度的值= 247

				*PC__00 ADC 的值= 2510

				*PC__11 ADC 的值= 2578

				第二种：如果两个都没接PC0和PC1的值接近4096

				第三种：下面是三线接两线的数据
				*PC__00 ADC 的值= 1260
				*PC__11 ADC 的值= 4077

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

		RT_value = RT *100000; //对电阻值放大十万倍，取小数点后五位
		//printf("\r\nPT100的阻值为 = %d\r\n",RT_value);
	
	
		return  RT_value;
		
}

//采用查表的方法获取温度
#if 1

uint16 Adc_to_temperature(uint16 value)
{
	if((value > 9126) && (value < adc_00))
		return 0;

	if((value >= adc_00) && (value < adc_10))
		return go_00_process(value);
	if((value >= adc_10) && (value < adc_20))
		return go_10_process(value);

	if((value >= adc_20) && (value < adc_30))
		return go_20_process(value);

	if((value >= adc_30) && (value < adc_40))
		return go_30_process(value);
	
	if((value >= adc_40) && (value < adc_50))
		return go_40_process(value);

	if((value >= adc_50) && (value < adc_60))
		return go_50_process(value);
	
	if((value >= adc_60) && (value < adc_70))
		return go_60_process(value);

	if((value >= adc_70) && (value < adc_80))
		return go_70_process(value);
	
	if((value >= adc_80) && (value < adc_90))
		return go_80_process(value);
	
	if((value >= adc_90) && (value < adc_100))
		return go_90_process(value);

	if((value >= adc_100) && (value < adc_110))
		return go_100_process(value);
	
	if((value >= adc_110) && (value < adc_120))
		return go_110_process(value);
	
	if((value >= adc_120) && (value < adc_130))
		return go_120_process(value);

	if((value >= adc_130) && (value < adc_140))
		return go_130_process(value);

	if((value >= adc_140) && (value < adc_150))
		return go_140_process(value);

	if((value >= adc_150) && (value < adc_160))
		return go_150_process(value);

	if((value >= adc_160) && (value < adc_170))
		return go_160_process(value);

	if((value >= adc_170) && (value < adc_180))
		return go_170_process(value);

	if((value >= adc_180) && (value < adc_190))
		return go_180_process(value);

	if((value >= adc_190) && (value < adc_200))
		return go_190_process(value);

	if((value >= adc_200) && (value < adc_210))
		return go_200_process(value);
		
		return 9999 ;
}




#endif




#if 0 //暂时不采用温度标定的方法
uint16 Adc_to_temperature(uint16 value)
{
	const uint16 adc_00 = 405;//492;//零度为推断值
	const uint16 adc_10 = 595;//558;
	const uint16 adc_20 = 770;//724;
	const uint16 adc_30 = 951;//890;
	const uint16 adc_40 = 1124;//1063;
	const uint16 adc_50 = 1295;//1232;
	const uint16 adc_60 = 1470;//1399;
	const uint16 adc_70 = 1640;//1563;
	const uint16 adc_80 = 1812;//1719;
	const uint16 adc_90 = 1968;//1877;
	const uint16 adc_100 = 2107;

	
	if((value >= adc_00) && (value < adc_10))
		return (0 + get_dot((value - adc_00),(adc_10 - adc_00)));
	
	if((value >= adc_10) && (value < adc_20))
		return (100 + get_dot((value - adc_10),(adc_20 - adc_10)));

	if((value >= adc_20) && (value < adc_30))
		return (200 + get_dot((value - adc_20),(adc_30 - adc_20)));

	if((value >= adc_30) && (value < adc_40))
		return (300 + get_dot((value - adc_30),(adc_40 - adc_30)));
	
	if((value >= adc_40) && (value < adc_50))
		return (400 + get_dot((value - adc_40),(adc_50 - adc_40)));

	if((value >= adc_50) && (value < adc_60))
		return (500 + get_dot((value - adc_50),(adc_60 - adc_50)));
	
	if((value >= adc_60) && (value < adc_70))
		return (600 + get_dot((value - adc_60),(adc_70 - adc_60)));

	if((value >= adc_70) && (value < adc_80))
		return (700 + get_dot((value - adc_70),(adc_80 - adc_70)));
	
	if((value >= adc_80) && (value < adc_90))
		return (800 + get_dot((value - adc_80),(adc_90 - adc_80)));
	
	if((value >= adc_90) && (value < adc_100))
		return (900 + get_dot((value - adc_90),(adc_100 - adc_90)));
	
	return 0;
}

#endif


uint16 get_dot(uint16 value,uint16 percent)
{
	//get 1--10
	return  (value*10)/percent ;
}



uint16 go_00_process(uint16 value)
{

	 uint16 zero_value = 0; //0度为起始值

	  uint16 adc_0 = adc_00;  //10度
	  uint16 adc_1 = adc_01; //11度
	  uint16 adc_2 = adc_02;
	  uint16 adc_3 = adc_03;
	  uint16 adc_4 = adc_04;
	  uint16 adc_5 = adc_05;
	  uint16 adc_6 = adc_06;
	  uint16 adc_7 = adc_07;
	  uint16 adc_8 = adc_08;
	  uint16 adc_9 = adc_09;


	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_10))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_10 - adc_9)));


	return  0;
	
}





uint16 go_10_process(uint16 value)
{
	 uint16 adc_0 = adc_10;  //10度
	  uint16 adc_1 = adc_11; //11度
	  uint16 adc_2 = adc_12;
	  uint16 adc_3 = adc_13;
	  uint16 adc_4 = adc_14;
	  uint16 adc_5 = adc_15;
	  uint16 adc_6 = adc_16;
	  uint16 adc_7 = adc_17;
	  uint16 adc_8 = adc_18;
	  uint16 adc_9 = adc_19;

	const uint16 zero_value = 100; //10度为起始值


	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_20 ))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_20 - adc_9)));


	return  0;
	
}


uint16 go_20_process(uint16 value)
{
	 uint16 adc_0 = adc_20;   
	  uint16 adc_1 = adc_21;  
	  uint16 adc_2 = adc_22;
	  uint16 adc_3 = adc_23;
	  uint16 adc_4 = adc_24;
	  uint16 adc_5 = adc_25;
	  uint16 adc_6 = adc_26;
	  uint16 adc_7 = adc_27;
	  uint16 adc_8 = adc_28;
	  uint16 adc_9 = adc_29;

	const uint16 zero_value = 200; //20度为起始值


	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_30))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_30 - adc_9)));


	return  0;
	
}


uint16 go_30_process(uint16 value)
{
	 uint16 adc_0 = adc_30;   
	  uint16 adc_1 = adc_31;  
	  uint16 adc_2 = adc_32;
	  uint16 adc_3 = adc_33;
	  uint16 adc_4 = adc_34;
	  uint16 adc_5 = adc_35;
	  uint16 adc_6 = adc_36;
	  uint16 adc_7 = adc_37;
	  uint16 adc_8 = adc_38;
	  uint16 adc_9 = adc_39;

	const uint16 zero_value = 300; //30度为起始值


	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_40))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_40 - adc_9)));


	return  0;
	
}




uint16 go_40_process(uint16 value)
{
	  uint16 adc_0 = adc_40;   
	  uint16 adc_1 = adc_41;  
	  uint16 adc_2 = adc_42;
	  uint16 adc_3 = adc_43;
	  uint16 adc_4 = adc_44;
	  uint16 adc_5 = adc_45;
	  uint16 adc_6 = adc_46;
	  uint16 adc_7 = adc_47;
	  uint16 adc_8 = adc_48;
	  uint16 adc_9 = adc_49;

	const uint16 zero_value = 400; //40度为起始值


	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_50))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_50 - adc_9)));


	return  0;
	
}

uint16 go_50_process(uint16 value)
{
	  uint16 adc_0 = adc_50;   
	  uint16 adc_1 = adc_51;  
	  uint16 adc_2 = adc_52;
	  uint16 adc_3 = adc_53;
	  uint16 adc_4 = adc_54;
	  uint16 adc_5 = adc_55;
	  uint16 adc_6 = adc_56;
	  uint16 adc_7 = adc_57;
	  uint16 adc_8 = adc_58;
	  uint16 adc_9 = adc_59;

	const uint16 zero_value = 500; //50度为起始值


	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_60))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_60 - adc_9)));


	return  0;
	
}








uint16 go_60_process(uint16 value)
{
	uint16 adc_0 = adc_60;   
	  uint16 adc_1 = adc_61;  
	  uint16 adc_2 = adc_62;
	  uint16 adc_3 = adc_63;
	  uint16 adc_4 = adc_64;
	  uint16 adc_5 = adc_65;
	  uint16 adc_6 = adc_66;
	  uint16 adc_7 = adc_67;
	  uint16 adc_8 = adc_68;
	  uint16 adc_9 = adc_69;

	const uint16 zero_value = 600; //60度为起始值


	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_70))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_70 - adc_9)));

	return 0;
}

uint16 go_70_process(uint16 value)
{
	uint16 adc_0 = adc_70;   
	  uint16 adc_1 = adc_71;  
	  uint16 adc_2 = adc_72;
	  uint16 adc_3 = adc_73;
	  uint16 adc_4 = adc_74;
	  uint16 adc_5 = adc_75;
	  uint16 adc_6 = adc_76;
	  uint16 adc_7 = adc_77;
	  uint16 adc_8 = adc_78;
	  uint16 adc_9 = adc_79;

	const uint16 zero_value = 700; //80度为起始值

		if ((value >= adc_0) && (value < adc_1))
				return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));
	
		if ((value >= adc_1) && (value < adc_2))
				return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
		
		if ((value >= adc_2) && (value < adc_3))
				return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
			
		if ((value >= adc_3) && (value < adc_4))
				return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
		
		if ((value >= adc_4) && (value < adc_5))
				return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));
	
		if ((value >= adc_5) && (value < adc_6))
				return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));
	
		if ((value >= adc_6) && (value < adc_7))
				return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));
	
		if ((value >= adc_7) && (value < adc_8))
				return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));
	
		if ((value >= adc_8) && (value < adc_9))
				return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));
	
	
		if ((value >= adc_9) && (value < adc_80))
				return (zero_value + 90 + get_dot((value - adc_9),(adc_80 - adc_9)));

		return 0;
	
		

}


uint16 go_80_process(uint16 value)
{
	  uint16 adc_0 = adc_80;   
	  uint16 adc_1 = adc_81;  
	  uint16 adc_2 = adc_82;
	  uint16 adc_3 = adc_83;
	  uint16 adc_4 = adc_84;
	  uint16 adc_5 = adc_85;
	  uint16 adc_6 = adc_86;
	  uint16 adc_7 = adc_87;
	  uint16 adc_8 = adc_88;
	  uint16 adc_9 = adc_89;


	const uint16 zero_value = 800; //80度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_90))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_90 - adc_9)));

	return 0;
}


uint16 go_90_process(uint16 value)
{
	  uint16 adc_0 = adc_90;   
	  uint16 adc_1 = adc_91;  
	  uint16 adc_2 = adc_92;
	  uint16 adc_3 = adc_93;
	  uint16 adc_4 = adc_94;
	  uint16 adc_5 = adc_95;
	  uint16 adc_6 = adc_96;
	  uint16 adc_7 = adc_97;
	  uint16 adc_8 = adc_98;
	  uint16 adc_9 = adc_99;


	const uint16 zero_value = 900; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_100))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_100 - adc_9)));

	return 0;
}


uint16 go_100_process(uint16 value)
{
	  uint16 adc_0 = adc_100;   
	  uint16 adc_1 = adc_101;  
	  uint16 adc_2 = adc_102;
	  uint16 adc_3 = adc_103;
	  uint16 adc_4 = adc_104;
	  uint16 adc_5 = adc_105;
	  uint16 adc_6 = adc_106;
	  uint16 adc_7 = adc_107;
	  uint16 adc_8 = adc_108;
	  uint16 adc_9 = adc_109;


	const uint16 zero_value = 1000; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_110))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_110 - adc_9)));

	return 0;
}




uint16 go_110_process(uint16 value)
{
	  uint16 adc_0 = adc_110;   
	  uint16 adc_1 = adc_111;  
	  uint16 adc_2 = adc_112;
	  uint16 adc_3 = adc_113;
	  uint16 adc_4 = adc_114;
	  uint16 adc_5 = adc_115;
	  uint16 adc_6 = adc_116;
	  uint16 adc_7 = adc_117;
	  uint16 adc_8 = adc_118;
	  uint16 adc_9 = adc_119;


	const uint16 zero_value = 1100; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_120))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_120 - adc_9)));

	return 0;
}



uint16 go_120_process(uint16 value)
{
	  uint16 adc_0 = adc_120;   
	  uint16 adc_1 = adc_121;  
	  uint16 adc_2 = adc_122;
	  uint16 adc_3 = adc_123;
	  uint16 adc_4 = adc_124;
	  uint16 adc_5 = adc_125;
	  uint16 adc_6 = adc_126;
	  uint16 adc_7 = adc_127;
	  uint16 adc_8 = adc_128;
	  uint16 adc_9 = adc_129;


	const uint16 zero_value = 1200; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_130))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_130 - adc_9)));

	return 0;
}


uint16 go_130_process(uint16 value)
{
	  uint16 adc_0 = adc_130;   
	  uint16 adc_1 = adc_131;  
	  uint16 adc_2 = adc_132;
	  uint16 adc_3 = adc_133;
	  uint16 adc_4 = adc_134;
	  uint16 adc_5 = adc_135;
	  uint16 adc_6 = adc_136;
	  uint16 adc_7 = adc_137;
	  uint16 adc_8 = adc_138;
	  uint16 adc_9 = adc_139;


	const uint16 zero_value = 1300; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_140))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_140 - adc_9)));

	return 0;
}


uint16 go_140_process(uint16 value)
{
	  uint16 adc_0 = adc_140;   
	  uint16 adc_1 = adc_141;  
	  uint16 adc_2 = adc_142;
	  uint16 adc_3 = adc_143;
	  uint16 adc_4 = adc_144;
	  uint16 adc_5 = adc_145;
	  uint16 adc_6 = adc_146;
	  uint16 adc_7 = adc_147;
	  uint16 adc_8 = adc_148;
	  uint16 adc_9 = adc_149;


	const uint16 zero_value = 1400; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_150))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_150 - adc_9)));

	return 0;
}




uint16 go_150_process(uint16 value)
{
	  uint16 adc_0 = adc_150;   
	  uint16 adc_1 = adc_151;  
	  uint16 adc_2 = adc_152;
	  uint16 adc_3 = adc_153;
	  uint16 adc_4 = adc_154;
	  uint16 adc_5 = adc_155;
	  uint16 adc_6 = adc_156;
	  uint16 adc_7 = adc_157;
	  uint16 adc_8 = adc_158;
	  uint16 adc_9 = adc_159;


	const uint16 zero_value = 1500; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_160))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_160 - adc_9)));

	return 0;
}



uint16 go_160_process(uint16 value)
{
	  uint16 adc_0 = adc_160;   
	  uint16 adc_1 = adc_161;  
	  uint16 adc_2 = adc_162;
	  uint16 adc_3 = adc_163;
	  uint16 adc_4 = adc_164;
	  uint16 adc_5 = adc_165;
	  uint16 adc_6 = adc_166;
	  uint16 adc_7 = adc_167;
	  uint16 adc_8 = adc_168;
	  uint16 adc_9 = adc_169;


	const uint16 zero_value = 1600; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_170))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_170 - adc_9)));

	return 0;
}




uint16 go_170_process(uint16 value)
{
	  uint16 adc_0 = adc_170;   
	  uint16 adc_1 = adc_171;  
	  uint16 adc_2 = adc_172;
	  uint16 adc_3 = adc_173;
	  uint16 adc_4 = adc_174;
	  uint16 adc_5 = adc_175;
	  uint16 adc_6 = adc_176;
	  uint16 adc_7 = adc_177;
	  uint16 adc_8 = adc_178;
	  uint16 adc_9 = adc_179;


	const uint16 zero_value = 1700; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_180))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_180 - adc_9)));

	return 0;
}


uint16 go_180_process(uint16 value)
{
	  uint16 adc_0 = adc_180;   
	  uint16 adc_1 = adc_181;  
	  uint16 adc_2 = adc_182;
	  uint16 adc_3 = adc_183;
	  uint16 adc_4 = adc_184;
	  uint16 adc_5 = adc_185;
	  uint16 adc_6 = adc_186;
	  uint16 adc_7 = adc_187;
	  uint16 adc_8 = adc_188;
	  uint16 adc_9 = adc_189;


	const uint16 zero_value = 1800; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_190))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_190 - adc_9)));

	return 0;
}


uint16 go_190_process(uint16 value)
{
	  uint16 adc_0 = adc_190;   
	  uint16 adc_1 = adc_191;  
	  uint16 adc_2 = adc_192;
	  uint16 adc_3 = adc_193;
	  uint16 adc_4 = adc_194;
	  uint16 adc_5 = adc_195;
	  uint16 adc_6 = adc_196;
	  uint16 adc_7 = adc_197;
	  uint16 adc_8 = adc_198;
	  uint16 adc_9 = adc_199;


	const uint16 zero_value = 1900; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_200))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_200 - adc_9)));

	return 0;
}

uint16 go_200_process(uint16 value)
{
	  uint16 adc_0 = adc_200;   
	  uint16 adc_1 = adc_201;  
	  uint16 adc_2 = adc_202;
	  uint16 adc_3 = adc_203;
	  uint16 adc_4 = adc_204;
	  uint16 adc_5 = adc_205;
	  uint16 adc_6 = adc_206;
	  uint16 adc_7 = adc_207;
	  uint16 adc_8 = adc_208;
	  uint16 adc_9 = adc_209;


	const uint16 zero_value = 2000; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_210))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_210 - adc_9)));

	return 0;
}


uint16 go_210_process(uint16 value)
{
	  uint16 adc_0 = adc_210;   
	  uint16 adc_1 = adc_211;  
	  uint16 adc_2 = adc_212;
	  uint16 adc_3 = adc_213;
	  uint16 adc_4 = adc_214;
	  uint16 adc_5 = adc_215;
	  uint16 adc_6 = adc_216;
	  uint16 adc_7 = adc_217;
	  uint16 adc_8 = adc_218;
	  uint16 adc_9 = adc_219;


	const uint16 zero_value = 2100; //90度为起始值

	if ((value >= adc_0) && (value < adc_1))
			return (zero_value + get_dot((value - adc_0),(adc_1 - adc_0)));

	if ((value >= adc_1) && (value < adc_2))
			return (zero_value + 10 + get_dot((value - adc_1),(adc_2 - adc_1)));
	
	if ((value >= adc_2) && (value < adc_3))
			return (zero_value + 20 + get_dot((value - adc_2),(adc_3 - adc_2)));
		
	if ((value >= adc_3) && (value < adc_4))
			return (zero_value + 30 + get_dot((value - adc_3),(adc_4 - adc_3)));
	
	if ((value >= adc_4) && (value < adc_5))
			return (zero_value + 40 + get_dot((value - adc_4),(adc_5 - adc_4)));

	if ((value >= adc_5) && (value < adc_6))
			return (zero_value + 50 + get_dot((value - adc_5),(adc_6 - adc_5)));

	if ((value >= adc_6) && (value < adc_7))
			return (zero_value + 60 + get_dot((value - adc_6),(adc_7 - adc_6)));

	if ((value >= adc_7) && (value < adc_8))
			return (zero_value + 70 + get_dot((value - adc_7),(adc_8 - adc_7)));

	if ((value >= adc_8) && (value < adc_9))
			return (zero_value + 80 + get_dot((value - adc_8),(adc_9 - adc_8)));


	if ((value >= adc_9) && (value < adc_220))
			return (zero_value + 90 + get_dot((value - adc_9),(adc_220 - adc_9)));

	return 0;
}





/*********************************************END OF FILE**********************/

