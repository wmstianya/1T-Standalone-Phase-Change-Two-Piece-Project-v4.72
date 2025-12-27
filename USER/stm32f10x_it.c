/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "main.h"
#include "fan_speed.h"


//extern		uint8 re4_time		 ;	



 

void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
		
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
			
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
	/**
  * @brief  PVD???????��
  * @param  None
  * @retval None
  */
void PVD_IRQHandler(void)
{
		/*?��???????��?��??PVD????????*/
		if(PWR_GetFlagStatus (PWR_FLAG_PVDO)==SET)			
		{
			/* ???��???????????????????????����??????�� */
			//LED_RED; 
			
		}
    /* ????????????*/
    EXTI_ClearITPendingBit(EXTI_Line16);

}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        /* 风机脉冲计数（带消抖滤波） */
        Fan_Pulse_ISR_Handler();
        
        if(sys_flag.test_OFF == 0)
        {
            sys_flag.test_OFF = OK;
            sys_flag.test_Result = sys_flag.test_Ms;
        }
        
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

//�ⲿ�ж�4������� 
void EXTI4_IRQHandler(void)		//4����ͨ��6
{
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET)
	  {
		
	  
	  	EXTI_ClearITPendingBit(EXTI_Line4); //���LINE4�ϵ��жϱ�־λ

	  }  
}
 
//�ⲿ�ж�9_5�������
void EXTI9_5_IRQHandler(void)  //5����ͨ��5
{		 
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line5);  //���LINE5�ϵ��жϱ�־λ

	  }
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line6);  //���LINE6�ϵ��жϱ�־λ
	  
	  }
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line7);  //���LINE7�ϵ��жϱ�־λ
	  
	  }
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line8);  //���LINE8�ϵ��жϱ�־λ

	  }
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line9);  //���LINE9�ϵ��жϱ�־λ

	  }  
}
//�ⲿ�ж�15_10�������
void EXTI15_10_IRQHandler(void)		//15����ͨ��1,14����ͨ��2,13����ͨ��3��12����ͨ��4
{		 
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line10);  //���LINE10�ϵ��жϱ�־λ

	  }
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line11);  //���LINE11�ϵ��жϱ�־λ

	  }
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
	  {
			sys_flag.Interrupt_Alive = OK; //�������жϴ���

			Switch_Inf.OpenWait_7ms = 0;

			Switch_Inf.CloseWait_3ms = 0;
			
	  
	  		 if(Switch_Inf.air_on_flag)
	  		 	{
	  		 		if(Switch_Inf.OpenWait_7ms >= 7)
	  		 			{
	  		 				
	  		 			}
					
RELAY6_ON;  //�ͷ�������
					RELAY3_ON; //ֱ���̵����ſ���
	  		 		
	  		 	}
			 else
			 	{
			 		if(Switch_Inf.CloseWait_3ms >= 3)
			 			{
			 				

			 			}
					RELAY6_OFF;  //�ͷ�������
					RELAY3_OFF; //ֱ���̵����ſ���
			 		
			 	}

			 if(Switch_Inf.gas_on_flag)
			 	{
			 		RELAY7_ON;
			 	}
			 else
			 	{
			 		RELAY7_OFF;
			 	}

			 if(Switch_Inf.dian_huo_flag)
			 	{
			 		RELAY8_ON;
			 	}
			 else
			 	{
			 		RELAY8_OFF;
			 	}


			 if(Switch_Inf.water_switch_flag )
			 	{
			 		
			 		RELAY5_ON;
			 	}
			 else
			 	{
			 		RELAY5_OFF;
					
			 	}

			 if(Switch_Inf.Water_Valve_Flag)
			 	{
			 		RELAY4_ON;
			 	}
			 else
			 	{
			 		RELAY4_OFF;
			 	}

			 if(Switch_Inf.pai_wu_flag)
			 	{
			 		RELAY9_ON;
			 	}
			 else
			 	{
			 		RELAY9_OFF;
			 	}

			 if(Switch_Inf.MingHuo_Flag)
			 	{
			 		RELAY2_ON;
			 	}
			 else
			 	{
			 		RELAY2_OFF;
			 	}

			  if(Switch_Inf.LianXu_PaiWu_flag)
			 	{
			 		RELAY10_ON;
			 	}
			 else
			 	{
			 		RELAY10_OFF;
			 	}

			
	 	 EXTI_ClearITPendingBit(EXTI_Line12);  //���LINE12�ϵ��жϱ�־λ

	  }
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line13);  //���LINE13�ϵ��жϱ�־λ

	  }
	if(EXTI_GetITStatus(EXTI_Line14)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line14);  //���LINE14�ϵ��жϱ�־λ

	  }
 	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)
	  {
	  EXTI_ClearITPendingBit(EXTI_Line15);  //���LINE15�ϵ��жϱ�־λ

	  }
}



/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	  {
	    /* Clear the RTC Second interrupt */
	    RTC_ClearITPendingBit(RTC_IT_SEC);
	
	    /* Enable time update */
	    TimeDisplay = 1;
	
			
			
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
	  }
}
/****
void USART1_IRQHandler(void)
{

  uint8 Res = 0;
	
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־
		Res =USART_ReceiveData(USART1);//(USART2->DR);	//��ȡ���յ�������
		if(U1_Inf.Rx_temp_length == 0&&U1_Inf.Recive_Ok_Flag == 0)
		  {
		  	U1_Inf.RX_Data[0]=Res;
		  	U1_Inf.Rx_temp_length++;
		  	U1_Inf.Recive_Flag=1;
		  	U1_Inf.Recive_Time=0;
		  }
  		else if(U1_Inf.Rx_temp_length > 0 && U1_Inf.Recive_Ok_Flag == 0)
		  {		if(U1_Inf.Rx_temp_length >250)
		  			U1_Inf.Rx_temp_length = 0;
				U1_Inf.RX_Data[U1_Inf.Rx_temp_length]=Res;
				U1_Inf.Rx_temp_length++;
				U1_Inf.Recive_Time=0;
		  }
	}

  if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET)//���ORE��־
  	{
  		USART_ClearFlag(USART1,USART_FLAG_ORE);
		USART_ReceiveData(USART1);
  	}

  	if (USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
    {
				
        USART_ReceiveData(USART1);
      USART_ClearFlag(USART1, USART_FLAG_PE);
    }
 
 	if (USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
    {
				
       USART_ReceiveData(USART1);
       USART_ClearFlag(USART1, USART_FLAG_FE);
    }
}
	
	
****/

// ��ȫ�滻���е�USART1_IRQHandler����
void USART1_IRQHandler(void)
{
    uint16_t data_len = 0;
    
    // �������ж�
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        // ���IDLE��־�������ȶ�SR�ٶ�DR�������IDLE��־��
        volatile uint16_t sr = USART1->SR;
        volatile uint16_t dr = USART1->DR;
        (void)sr; (void)dr;  // �������������
        
        // ֹͣDMA����
        DMA_Cmd(DMA1_Channel5, DISABLE);
        
        // ������յ������ݳ���
        data_len = U1_Inf.DMA_RX_Size - DMA_GetCurrDataCounter(DMA1_Channel5);
        
        // ��������ݽ����ҵ�ǰû�д�����������
        if(data_len > 0 && data_len < U1_Inf.DMA_RX_Size && U1_Inf.Recive_Ok_Flag == 0)
        {
            // ��DMA���������ݸ��Ƶ�����������
            memcpy(U1_Inf.RX_Data, U1_Inf.DMA_RX_Buffer, data_len);
            U1_Inf.RX_Length = data_len;
            U1_Inf.Recive_Ok_Flag = 1;  // ������ݽ������
            
            // �����ԣ�����ԭ�б���
            U1_Inf.Rx_temp_length = 0;
            U1_Inf.Recive_Flag = 0;
            U1_Inf.Recive_Time = 0;
        }
        
        // ����DMA����
        uart1_dma_restart();
    }
    
    // �����������
    if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(USART1, USART_FLAG_ORE);
        USART_ReceiveData(USART1);  // ��ȡDR�Ĵ������ORE��־
    }
    
    // ������żУ�����
    if(USART_GetFlagStatus(USART1, USART_FLAG_PE) == SET)
    {
        USART_ClearFlag(USART1, USART_FLAG_PE);
        USART_ReceiveData(USART1);
    }
    
    // ����֡����
    if(USART_GetFlagStatus(USART1, USART_FLAG_FE) == SET)
    {
        USART_ClearFlag(USART1, USART_FLAG_FE);
        USART_ReceiveData(USART1);
    }
}

void USART2_IRQHandler(void)				  //����2�жϷ������
{
 uint8 Res = 0;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //����жϱ�־
		Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
	
		if(U2_Inf.Rx_temp_length == 0&&U2_Inf.Recive_Ok_Flag == 0)
		  {
		  	U2_Inf.RX_Data[0]=Res;
		  	U2_Inf.Rx_temp_length++;
		  	U2_Inf.Recive_Flag=1;
		  	U2_Inf.Recive_Time=0;
		  }
  		else if(U2_Inf.Rx_temp_length > 0 && U2_Inf.Recive_Ok_Flag == 0)
		  {
		  		if(U2_Inf.Rx_temp_length >250)
		  			U2_Inf.Rx_temp_length = 0;
				U2_Inf.RX_Data[U2_Inf.Rx_temp_length]=Res;
				U2_Inf.Rx_temp_length++;
				U2_Inf.Recive_Time=0;
		  }
	}

   if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)//���ORE��־
  	{
  		USART_ClearFlag(USART2,USART_FLAG_ORE);
		USART_ReceiveData(USART2);
  	}
}


void USART3_IRQHandler(void)				  //����2�жϷ������
{
  uint8 Res = 0;
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); //����жϱ�־
		Res =USART_ReceiveData(USART3);//(USART2->DR);	//��ȡ���յ�������
		if(U3_Inf.Rx_temp_length == 0&&U3_Inf.Recive_Ok_Flag == 0)
		  {
		  	U3_Inf.RX_Data[0]=Res;
		  	U3_Inf.Rx_temp_length++;
		  	U3_Inf.Recive_Flag=1;
		  	U3_Inf.Recive_Time=0;
		  }
  		else if(U3_Inf.Rx_temp_length > 0 && U3_Inf.Recive_Ok_Flag == 0)
		  {
		  		if(U3_Inf.Rx_temp_length >250)
		  			U3_Inf.Rx_temp_length = 0;
				U3_Inf.RX_Data[U3_Inf.Rx_temp_length]=Res;
				U3_Inf.Rx_temp_length++;
				U3_Inf.Recive_Time=0;
		  }
	}

  if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)//���ORE��־
  	{
  		USART_ClearFlag(USART3,USART_FLAG_ORE);
		USART_ReceiveData(USART3);
  	}

	if (USART_GetFlagStatus(USART3, USART_FLAG_PE) != RESET)
    {
				
        USART_ReceiveData(USART3);
      USART_ClearFlag(USART3, USART_FLAG_PE);
    }
 
 	if (USART_GetFlagStatus(USART3, USART_FLAG_FE) != RESET)
    {
				
       USART_ReceiveData(USART3);
       USART_ClearFlag(USART3, USART_FLAG_FE);
    }
  	
}



void UART4_IRQHandler(void)  //����4�жϷ������
{
	uint8 Res = 0;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE); //����жϱ�־
		Res =USART_ReceiveData(UART4);//(USART2->DR);	//��ȡ���յ�������
		if(U4_Inf.Rx_temp_length == 0&&U4_Inf.Recive_Ok_Flag == 0)
		  {
		  	U4_Inf.RX_Data[0]=Res;
		  	U4_Inf.Rx_temp_length++;
		  	U4_Inf.Recive_Flag=1;
		  	U4_Inf.Recive_Time=0;
		  }
  		else if(U4_Inf.Rx_temp_length > 0 && U4_Inf.Recive_Ok_Flag == 0)
		  {
		  		if(U4_Inf.Rx_temp_length >250)
		  			U4_Inf.Rx_temp_length = 0;
				U4_Inf.RX_Data[U4_Inf.Rx_temp_length]=Res;
				U4_Inf.Rx_temp_length++;
				U4_Inf.Recive_Time=0;
		  }	
	}

  	if(USART_GetFlagStatus(UART4, USART_FLAG_ORE) == SET)//���ORE��־
  	{
  		USART_ClearFlag(UART4,USART_FLAG_ORE);
		USART_ReceiveData(UART4);
  	}
		
	if (USART_GetFlagStatus(UART4, USART_FLAG_PE) != RESET)
    {
				
        USART_ReceiveData(UART4);
      USART_ClearFlag(UART4, USART_FLAG_PE);
    }
 
 	if (USART_GetFlagStatus(UART4, USART_FLAG_FE) != RESET)
    {
				
       USART_ReceiveData(UART4);
       USART_ClearFlag(UART4, USART_FLAG_FE);
    }
}	

void UART5_IRQHandler(void)  //����5�жϷ������
{
	uint8 Res;
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearITPendingBit(UART5,USART_IT_RXNE); //����жϱ�־
		
		Res =USART_ReceiveData(UART5);	//��ȡ���յ�������
		if(U5_Inf.Rx_temp_length == 0&&U5_Inf.Recive_Ok_Flag == 0)
		  {
		  	U5_Inf.RX_Data[0]=Res;
		  	U5_Inf.Rx_temp_length++;
		  	U5_Inf.Recive_Flag=1;
		  	U5_Inf.Recive_Time=0;
		  }
  		else if(U5_Inf.Rx_temp_length > 0 && U5_Inf.Recive_Ok_Flag == 0)
		  {
		  		if(U5_Inf.Rx_temp_length >250)
		  			U5_Inf.Rx_temp_length = 0;
				U5_Inf.RX_Data[U5_Inf.Rx_temp_length]=Res;
				U5_Inf.Rx_temp_length++;
				U5_Inf.Recive_Time=0;
		  }
	}


  if(USART_GetFlagStatus(UART5, USART_FLAG_ORE) == SET)//���ORE��־
  	{
  		USART_ClearFlag(UART5,USART_FLAG_ORE);
		USART_ReceiveData(UART5);
  	}

  if (USART_GetFlagStatus(UART5, USART_FLAG_PE) != RESET)
    {
				
        USART_ReceiveData(UART5);
      USART_ClearFlag(UART5, USART_FLAG_PE);
    }
 
 if (USART_GetFlagStatus(UART5, USART_FLAG_FE) != RESET)
    {
				
       USART_ReceiveData(UART5);
       USART_ClearFlag(UART5, USART_FLAG_FE);
    }
}	



void TIM5_IRQHandler(void)
{
	static uint8 Hcount = 0;
	static uint8 H_Flag = 0;
	static uint8 Lcount = 0;

	//PortPin_Scan(GPIOE,GPIO_Pin_7)
	
	//���ڼ�����������ŵ��ź�Ƶ��
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
		{
			TIM_ClearITPendingBit(TIM5, TIM_IT_Update ); 

			

		}
}




void TIM4_IRQHandler(void)   //TIM4_____5ms�ж�
{

	
	static uint16 time_100ms = 0;
	static uint16 time_200ms = 0;
	static uint16 time_300ms = 0;
	static uint16 time_500ms = 0;
	static uint16 time_1s = 0;
	static uint8  Sys_1flag;
	static uint16 time_2s = 0;
	static uint8  time_3s = 0;
	static uint16 time_5s = 0;
	static uint32 time_20s = 0;
	static uint8 time_60s = 0;
	static uint32 time_120s = 0;
	
	uint8 index = 0;
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
		{
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //���TIMx�����жϱ�־

			if(Water_State.Cstate_Flag ||Water_State.Zstate_Flag) //˫��������ˮ��������
				Water_State.ZC_Open_Time ++;
			else
				Water_State.ZC_Open_Time = 0;

			if(Switch_Inf.water_switch_flag == FALSE)
				Water_State.PUMP_Close_Time++;
			else
				Water_State.PUMP_Close_Time = 0;



			if(Switch_Inf.air_on_flag)
				{
					sys_flag.test_Ms = 0;
					sys_flag.test_OFF = 0;
				}
			else
				{
					if(sys_flag.test_OFF == 0)
						sys_flag.test_Ms ++;
					
				}

			time_500ms++;
		if(time_500ms > 500)
			{
				time_500ms = 0;
				sys_flag.Power_1_Sec = OK;

				WDI_TOGGLE;
			}

		time_100ms ++;  //100ms
		if(time_100ms >= 100)
			{
				time_100ms = 0;
				sys_flag.FlameFilter ++;
				sys_flag.LianXu_100msFlag = OK;
			}
		//++++++++++200msʱ�䣬��������+++++++++++++++++++++++++++	
			time_200ms++;
			if(time_200ms > 200)
				{
					time_200ms = 0;
					sys_flag.ADC_100msFlag = OK;
					sys_flag.flag_200ms = 1;
					

					sys_flag.WaterAJ_Flag = OK;
					
					if(sys_flag.Pressure_HFlag)
						sys_flag.Pressure_HCount ++;
					else
						sys_flag.Pressure_HCount = 0;
					
					if(sys_flag.WenDu_HFlag)
						sys_flag.WenDu_HCount ++;
					else
						sys_flag.WenDu_HCount = 0;

					sys_flag.Special_100msFlag = OK;
				}


			//++++++++++300msʱ�䣬��������+++++++++++++++++++++++++++	
			time_300ms++;
			if(time_300ms > 750)
				{
					time_300ms = 0;
					sys_flag.flag_300ms = 1;

				

					if(Switch_Inf.Water_Valve_Flag) //��ˮ��ŷ���������ʼ��ʱ
						sys_flag.WaterOut_Time++;
					else
						sys_flag.WaterOut_Time = 0;

					if(Switch_Inf.water_switch_flag == FALSE )  //��ˮ�ùرտ�ʼ��ʱ
						sys_flag.WaterClose_Time++;
					else
						sys_flag.WaterClose_Time = 0;

					sys_flag.Tx_3Sec_Flag = OK;
				}

	//++++++++++1��ʱ�䣬��������+++++++++++++++++++++++++++
			time_1s++;
			if(time_1s >= 1000)
				{
					time_1s = 0;

					sys_flag.UnTalk_Time++;  //��ƴ���ͣ��ӻ�������ʧ��

					if(sys_flag.XB_WaterLow_Flag)
						{
							sys_flag.XB_WaterLow_Count ++;
						}
					else
						{
							sys_flag.XB_WaterLow_Count = 0;
						}

					sys_flag.Work_1S_Flag = OK;
					
					sys_flag.Union_1_Sec = OK;
					
					sys_flag.Pressure_1sFlag = OK;
					sys_flag.Lcd_Unconnect_Time ++; //LCDδͨ�ż�ʱ

					WORK_LED_TOGGLE;

					sys_flag.SPI_ReadRX_Flag = OK;

					sys_flag.LianXu_1sFlag = OK;
					
					sys_flag.Work_1sec_Flag = OK; //���ڹ���ʱ��ͳ��

					if(sys_flag.FlameOut_Count && sys_flag.flame_state)
						sys_flag.FlameRecover_Time++;
					else
						sys_flag.FlameRecover_Time = 0;

					if(IO_Status.Target.XB_WaterLow == OK)
						{
							//���ˮλ����״̬�Իָ�ʱ��
							sys_flag.XB_WaterLowAB_RecoverTime ++;
						}
					else
						{
							sys_flag.XB_WaterLowAB_RecoverTime = 0;
						}

					sys_flag.WaterUnsupply_Count++;

					sys_flag.Old_Error_Count ++;

					time_3s++;
					if(time_3s >= 3)
						{
							time_3s = 0;
							sys_flag.Relays_3Secs_Flag = 1;
						}


					
					

					if(Beep_Data.beep_start_flag)
						{
							BEEP_TIME(300);
						}
					else
						{
							
						}


					if(sys_flag.waterSend_Flag)
						sys_flag.waterSend_Count++;
					else
						sys_flag.waterSend_Count = 0;


					
				
					
					if(Union_Flag.Big_Timer_Flag)
						Union_Flag.Big_Timer_Count++;
					else
						Union_Flag.Big_Timer_Count = 0;

					if(Union_Flag.Small_Timer_Flag)
						Union_Flag.Small_Timer_Count++;
					else
						Union_Flag.Small_Timer_Count = 0;

					
					sys_flag.Paiwu_Secs++;
					 

					sys_flag.Lcd_Value_Check1S_Flag = OK;

					sys_flag.Rpm_1_Sec = OK;

					if(sys_flag.Force_Flag )
						sys_flag.Force_Count++;
					else
						sys_flag.Force_Count = 0;
			
					if(sys_flag.Error16_Flag)
						sys_flag.Error16_Count++;
					else
						sys_flag.Error16_Count = 0;

					if(sys_flag.Error15_Flag)
						sys_flag.Error15_Count++;
					else
						sys_flag.Error15_Count = 0;

					if(sys_flag.Error1_Flag)
						sys_flag.Error1_Count++;
					else
						sys_flag.Error1_Count = 0;

					if(sys_flag.Error5_Flag)
						sys_flag.Error5_Count++;
					else
						sys_flag.Error5_Count = 0;
				 
					
					sys_flag.two_sec_flag = 1;//���ϰ汾�õı�־
					if(Sys_1flag)
						{
							sys_flag.Sys_1_Sec = OK;
							Sys_1flag = 0;
						}
					else
						{
							sys_flag.Sys_1_Sec = 0;
							Sys_1flag = 1;
						}
					

					if(Switch_Inf.water_switch_flag == FALSE)//ˮ�ùر�ʱ����
	 						sys_flag.Pump_Close_Time ++;
					else
							sys_flag.Pump_Close_Time = 0;
					

					if(Switch_Inf.water_switch_flag == OK)
							sys_flag.Pump_Open_Time ++;
					else
							sys_flag.Pump_Open_Time = 0;

					

						
					/*�ۼ�ϵͳ����ʱ��*/
					if(sys_data.Data_10H == 0x02)//ϵͳ����״̬
						{
				 			sys_time_inf.sec++;
							if(sys_flag.flame_state)
								{
									sys_flag.LianxuWorkTime ++;  //�������ʱ
								}
						}
					else
						{
							sys_time_inf.sec = 0;
						}
				
					
					
					
				}
			

			
	//++++++++++++++2sʱ�䣬��������++++++++++++++++++++++++++
			time_2s++;
			if(time_2s >= 2000)
				{
					time_2s = 0;
					sys_flag.Power_2_Sec = 1;
					
					
					sys_flag.Water_Relay_2_Sec_Flag = OK; //����ˮ�̵���ʹ��
				}

	//++++++++++++++3sʱ�䣬���ڼ̵���6��һ���ڣ��ⲿӲ����ֹ����������++++++++++++++++++++++++++		

	//++++++++++++++5sʱ�䣬��������++++++++++++++++++++++++++	
			time_5s++;
			if (time_5s >= 5000)
				{
					time_5s = 0;
					sys_flag.Power_5_Sec = 1;
					//����ʱ�����������ٶȿ�Щ
					sys_flag.Union_1A_Sec = OK;  //���ؼ�ʱʹ��
					
					
				}

			
				
	//++++++++++++++30sʱ�䣬��������++++++++++++++++++++++++++	
			time_20s++;
			if (time_20s >= 15000)
				{
					time_20s = 0;
					//����ʱ�����������ٶ���Щ
					if(sys_data.Data_10H != 2)
						sys_flag.sec_20_flag = 1;
				}
	//++++++++++++++120sʱ�䣬��������++++++++++++++++++++++++++	
			time_120s++;
			if (time_120s >= 120000)
				{
					time_120s = 0;
					sys_flag.sec_120_flag = 1;
					sys_flag.freeze_protect_check_begin = 1;
				}

			
			
				

		
/****************************����ѭ����������ͨѶ��ʱ END**************************************************/			
		if(U1_Inf.Recive_Flag)
				U1_Inf.Recive_Time++;
		if((U1_Inf.Recive_Time > 10)&&(U1_Inf.Recive_Ok_Flag ==0))
		{
			U1_Inf.Recive_Ok_Flag = 1;
			U1_Inf.RX_Length = U1_Inf.Rx_temp_length;
			U1_Inf.Rx_temp_length = 0;
			U1_Inf.Recive_Flag = 0;
			U1_Inf.Recive_Time = 0;
		}		
			

		/********���ô���2,����ʱ�䣬10ms�����ݣ���OK*******/		
		if(U2_Inf.Recive_Flag)
			U2_Inf.Recive_Time++;
		if((U2_Inf.Recive_Time > 10)&&(U2_Inf.Recive_Ok_Flag ==0))
		{
			U2_Inf.Recive_Ok_Flag = 1;
			U2_Inf.RX_Length = U2_Inf.Rx_temp_length;
			U2_Inf.Rx_temp_length = 0;
			U2_Inf.Recive_Flag = 0;
			U2_Inf.Recive_Time = 0;
		}
			
/********���ô���4,����ʱ�䣬5ms�����ݣ���OK*******/			
		if(U4_Inf.Recive_Flag)
				U4_Inf.Recive_Time++;
		if((U4_Inf.Recive_Time > 10)&&(U4_Inf.Recive_Ok_Flag ==0))
		{
			U4_Inf.Recive_Ok_Flag = 1;
			U4_Inf.RX_Length = U4_Inf.Rx_temp_length;
			U4_Inf.Rx_temp_length = 0;
			U4_Inf.Recive_Flag = 0;
			U4_Inf.Recive_Time = 0;
		}
			
/*       ���ô���3,����ʱ�䣬5ms�����ݣ���OK    */				
		if(U3_Inf.Recive_Flag)
				U3_Inf.Recive_Time++;
		if((U3_Inf.Recive_Time > 10)&&(U3_Inf.Recive_Ok_Flag ==0))
		{
			U3_Inf.Recive_Ok_Flag = 1;
			U3_Inf.RX_Length = U3_Inf.Rx_temp_length;
			U3_Inf.Rx_temp_length = 0;
			U3_Inf.Recive_Flag = 0;
			U3_Inf.Recive_Time = 0;
		}

/********���ô���5,����ʱ�䣬10ms�����ݣ���OK*******/		
		if(U5_Inf.Recive_Flag)
				U5_Inf.Recive_Time++;
		if((U5_Inf.Recive_Time > 10)&&(U5_Inf.Recive_Ok_Flag ==0))
		{
			U5_Inf.Recive_Ok_Flag = 1;
			U5_Inf.RX_Length = U5_Inf.Rx_temp_length;
			U5_Inf.Rx_temp_length = 0;
			U5_Inf.Recive_Flag = 0;
			U5_Inf.Recive_Time = 0;
		}

// ����һ��ȫ�ֱ�������ϵͳ������ʱ			
			
			if((sys_control_time <= 20) && (sys_time_start == 1 ))
			{
							sys_time_up = 1;  //��������
				      sys_time_start = false; //���幦�ܹر�
			}
			else
			{
					sys_control_time = sys_control_time - 1;//ע������ط��ж���1ms������ж�ʱ���޸�����ط�Ҳ��Ҫ�䶯
			}


		if((sys_flag.sys_delay_time <= 20)&&(sys_flag.sys_delay_start))
			{
				sys_flag.sys_delay_up = OK ;
				sys_flag.sys_delay_start = FALSE;
			}
		else
			sys_flag.sys_delay_time  = sys_flag.sys_delay_time - 1; //ע������ط��ж���1ms������ж�ʱ���޸�����ط�Ҳ��Ҫ�䶯



			
//���ӷ������ε�����ʱ�����
		//���ӷ������ε�����ʱ�����
		
		
	  //  TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //���TIMx�����жϱ�־
		}
}



void TIM2_IRQHandler(void)   //TIM2�ж� Լ376us
{	
	uint8 New_Version = FALSE;//�°汾�������ǵ�ƽ�������ϰ汾��Ƶ������
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
		{
			if (Beep_Data.beep_on_flag)
					{
						Beep_Data.beep_time_set--;
						if(Beep_Data.beep_time_set > 5)
							{
								
									BEEP_TOGGLE;
							}
						else
							{
								Beep_Data.beep_on_flag = 0;
								BEEP_OFF;
							}	
					}
			
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update );  //���TIMx�����жϱ�־
			
		 
		}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
