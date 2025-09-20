/**********************************************
 * 文件名  ：pwm_output.c
 * 描述    ：         
 *  
 * 硬件连接：---------------------
 *          |  PA.6: (TIM3_CH1)  |
 *           			
**********************************************************************************/
#include "main.h"


 const uint16 Frequence_PWM = 500; //风机频率2Khz 占空比可调

 uint16 CCR1_Val = 495;  //       
 uint16 CCR2_Val = 400;  
/*
 * 函数名：TIM5_GPIO_Config
 * 描述  ：配置TIM5复用输出PWM时用到的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void TIM3_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;	 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 	/* TIM3时钟使能 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); /* GPIOB 时钟使能 */

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    //  PB1 作为PWM使能输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


}

/*
 * 函数名：TIM3_Mode_Config
 * 描述  ：配置TIM3输出的PWM信号的模式，如周期、极性、占空比
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void TIM3_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM信号电平跳变值 */

	
/* -----------------------------------------------------------------------
		按照以下公式计算
    Frequency= TIMxCLK/( TIM_Prescaler +1)/ (TIM_Period + 1)
    TIMx Channelx duty cycle = (TIMx_Pulse/ ( TIM_Prescaler +1))* 100%
    TIM3CLK = 72 MHz, Prescaler = 0x0, TIM3 counter clock = TIM3CLK/(Prescaler+1)=72M
    TIM3 ARR Register = 999 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
    TIM3 Frequency = 72 KHz.
    TIM3 Channel1 duty cycle = 50%
   
  ----------------------------------------------------------------------- */

  /* Time base configuration */		 
  TIM_TimeBaseStructure.TIM_Period = 500;       //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 71;	    //设置预分频：不预分频，即为72MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	

  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);	 //使能通道1
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
 

  TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   //使能定时器3	
}

/*
 * 函数名：TIM5_PWM_Init
 * 描述  ：TIM5 输出PWM信号初始化，只要调用这个函数
 *         TIM5的四个通道就会有PWM信号输出
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void TIM3_PWM_Init(void)
{
	TIM3_GPIO_Config();	//TIM3定时器IO口配置
	TIM3_Mode_Config();	//TIM3定时器模式配置
}



void PWM_Adjust(uint8 value)
{
	//注意通道4 CRR后面的数字要改成4
	if(value > 100)
		value = 100;
	
	TIM3->CCR4 = (Frequence_PWM + 1) - (5 * value);
		
	sys_data.Data_1FH = value;
	LCD10D.DLCD.Air_Power = value;
	now_percent =value;

	//取个风机功率运行的时间，然后计数，用于点火过程中的吹扫时间控制
	 
}






