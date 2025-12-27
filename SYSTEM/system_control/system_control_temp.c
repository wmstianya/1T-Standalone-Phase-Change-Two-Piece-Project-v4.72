
#include "main.h"
#include "error_handler.h"



uint32	sys_control_time = 0;  //��ʱ����ʱ��
 uint8	   sys_time_up	   = false ;   //������?
 uint8	   sys_time_start = false;	//��������ʱ����?0 = not ,1 = yes

 
uint8 target_percent = 0; //�����趨Ŀ��������
uint8 now_percent = 0; //�����趨���ڵ�ʵʱ����

uint8 adc_sample_flag = 0; //adc ����ʱ����?

uint8 T_PERCENT = 0;
uint32_t BJ_TimeVar;//����ʱ�������?

/*ʱ��ṹ�壬Ĭ��ʱ��?000-01-01 00:00:00*/
struct rtc_time systmtime=
{
	0,0,0,1,1,2000,0
};


UART_DATA U1_Inf;
UART_DATA U2_Inf;
UART_DATA U3_Inf;

UART_DATA U4_Inf;

UART_DATA U5_Inf;



SYS_INF sys_data;
SYS_COPY copy_sys;

LCD_MEM lcd_data;



Lcd_Read_Data read_lcd_data;//���ڼ�¼�˹����õ�ϵͳ����
SYS_WORK_TIME sys_time_inf;//��¯ϵͳ�ۼ�����ʱ�����?
SYS_WORK_TIME Start_End_Time; //���ڼ�¼������ͣ������ʱ�����������������?

SYS_WORK_TIME big_time_inf;//��¯С��������ʱ��
SYS_WORK_TIME small_time_inf;//��¯���������ʱ��?

sys_flags sys_flag; //ϵͳ��Ҫʹ�õı�־������


SYS_CONFIG sys_config_data;//����ϵͳ���ô�С����ʱ�Ȳ���

SYS_ADMIN  Sys_Admin; //�������ù���Ա����

AB_EVENTS  Abnormal_Events;//����ϵͳ����ʱ���쳣��¼
BYTE_WORD4 Secret_uint; //���ڶ�4���ֽ�ת��Ϊ32λ����
BYTE_WORD1 Data_wordtobyte;//����1WORD    2���ֽ�ת��

FLP_INT  Float_Int;//���ڵ����ȸ��������ݵ�ת��
BYTE_INT32 Byte_To_Duint32;  //����4���ֽڵ�uint32������ת��


LCD_QuXian lcd_quxian_data;//����ˢ������ͳ�Ƶ�����
ERR_LCD  Err_Lcd_Code;//����ˢ��lcd��������
LCD_FLASH_STRUCT  Lcd_FlashD;


LCD_E_M  Err_Lcd_Memory[8];//���ڼ�¼8��������Ϣ��ʱ��͹���ԭ��?
ERROR_DATE_STRUCT SPI_Error_Data;



IO_DATA IO_Status;
 Login_TT Login_D; //�����¼��Ϣ�����ṹ��?

 Logic_Water Water_State;





 JUMP_TYPE  Jump_Step = First;



 










uint8  Air_Door_Index = 0;//���ڴ��Է����쳣����ת״̬ʹ��
uint8  ab_index =0 ;
 



//ADC_VrefintCmd()
 
 





uint8 Self_Index = 0;
uint8 Sys_Staus = 0;
uint8	Sys_Launch_Index = 0;
uint8 Ignition_Index = 0;
uint8	Pressure_Index = 0;
uint8 IDLE_INDEX = 0;





uint8 cmd_string111[7] = {0x5A,0xA5,0x04,0x80,0x03,0x00,0x4B}; //��ҳָ��л�����76ҳ


/* Get_IO_Inf() 已移动到 SYSTEM/error/error_handler.c */
#if 0  /* 原函数已移动 */
void Get_IO_Inf_OLD(void)
{
	uint8  Error16_Time = 8;
	
	uint8  Error_Buffer = 0;
		//�̶�һֱ����źţ�?ȼ��ѹ������еʽѹ���������ź�
	
		Error_Buffer = FALSE ;
		if (IO_Status.Target.water_high== WATER_OK)
			{
				if(IO_Status.Target.water_mid== WATER_LOSE ||IO_Status.Target.water_protect == WATER_LOSE)
					Error_Buffer = OK ;
			}
	
	
		if (IO_Status.Target.water_mid== WATER_OK)
			{
				if(IO_Status.Target.water_protect == WATER_LOSE)
					Error_Buffer = OK ;
			}
		
		if(Error_Buffer)
			{
				if(sys_flag.flame_state)
					{
						sys_flag.Force_Supple_Water_Flag = OK;
						sys_flag.Force_Flag =OK;
					}
					
				else
					sys_flag.Force_Supple_Water_Flag = 0;

				sys_flag.Error16_Flag = OK;
			}
		else
			{
				sys_flag.Force_Flag = FALSE; // 22.07.12����û�м�ʱ���ǿ�Ʋ�ˮ����?
				sys_flag.Error16_Flag = 0;
				sys_flag.Error16_Count = 0;
			}
		//ǿ�Ʋ�ˮ12�룬Ȼ�����ǿ�Ʋ�ˮ�ı��?
		if(sys_flag.Force_Count >= 5)
			{
				 sys_flag.Force_Supple_Water_Flag = 0;
				 sys_flag.Force_Flag = FALSE;
				 sys_flag.Force_Count = 0;
			}
			
			
		if(sys_data.Data_10H == 0)
			Error16_Time = 5; 
		if(sys_flag.flame_state && sys_data.Data_10H == 2)
			{
				//����ˮλ��
				if(IO_Status.Target.water_protect)
					Error16_Time = 8; //ԭ12�룬�ָĳ�15��2022��5��6��10:19:38
				else
					Error16_Time =5;//2022��7��12��14:18:33  ��8�ĳ�12�����ӵ�ʱ��    2024��8��24��08:51:17 �޸ĳ�5�룬����8 ��
			}
		else
			Error16_Time = 10;
		
		if(sys_flag.Error16_Count >= Error16_Time)  //8��
			{
				//sys_data.Data_l5H = SET_BIT_N(sys_data.Data_l5H,8);//������ˮλ�߼�����
				if(sys_flag.Error_Code == 0)
					sys_flag.Error_Code = Error8_WaterLogic;
				
				sys_flag.Error16_Flag = FALSE;
				sys_flag.Error16_Count = 0;
			}

	//�̶�һֱ����źţ�?ȼ��ѹ������еʽѹ���������ź�
		 
		if(IO_Status.Target.hot_protect == THERMAL_BAD)
			{
				if(sys_flag.Error15_Flag == 0)
					sys_flag.Error15_Count = 0;
				
				sys_flag.Error15_Flag = OK;

				if(sys_flag.Error15_Count > 1)
					{
						if(sys_flag.Error_Code == 0 )
							sys_flag.Error_Code = Error15_RebaoBad;
					}
				
			}
		else
			{
				sys_flag.Error15_Flag = 0;
				sys_flag.Error15_Count = 0;
			}

		
		//��еʽѹ������ź�?
		if(IO_Status.Target.hpressure_signal == PRESSURE_ERROR)
			{
				if(sys_flag.Error1_Flag == 0)
					sys_flag.Error1_Count = 0;
				
				sys_flag.Error1_Flag = OK;		
				//������ѹ��������ȫ��Χ�����ϣ�����
				if(sys_flag.Error1_Count > 1)
					{
						 if(sys_flag.Error_Code == 0 )
							sys_flag.Error_Code = Error1_YakongProtect; //����ѹ��������ȫ��Χ����	
					}
			}
		else
			{
				sys_flag.Error1_Flag = 0;
				sys_flag.Error1_Count = 0;
			}
			
	

}
#endif  /* Get_IO_Inf_OLD */



/**
  * @brief  �������ǰ�����׼������
  * @param  sys_flag.before_ignition_index
  * @retval ׼���÷���1�����򷵻�0
  */

uint8 Before_Ignition_Prepare(void)
{
		//1��ˮλ�źű�����                2�������źű�����
		//sys_flag.before_ignition_index
		uint8 func_state = 0;

		func_state = 0;
		switch (sys_flag.before_ignition_index)
			{
				case 0 :
						//������ŷ���ѭ���ã�sys_flag.Pai_Wu_Already���ˮλ�źž����Ƿ�����������Ʒ�
							 Send_Air_Open();  //�򿪷��ǰ���?
							 
							 PWM_Adjust(0); //�ȴ�5�����?
							 Pai_Wu_Door_Close();
							 delay_sys_sec(12000);
							 if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
							 	{
							 		if(Temperature_Data.Inside_High_Pressure <= 1)
							 			{
							 				LianXu_Paiwu_Open();
							 			}
							 	}
							 
							sys_flag.before_ignition_index = 1;//��ת���¸�״̬
							sys_flag.FlameOut_Count = 0;
							sys_flag.XB_WaterLowAB_Count = 0;
							
							break;

				case 1: 

						if(sys_time_start == 0)
							sys_time_up = 1;
						if(sys_time_up)
							{
								sys_time_up = 0;
								sys_flag.Wts_Gas_Index =0;
								sys_flag.before_ignition_index = 2;//��ת���¸�״̬
								Feed_First_Level();
								if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
							 		{
							 			LianXu_Paiwu_Close();
							 		}
							}	
								
							break;

				case 2:
					if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
				 		{
				 			//�����飬�ٴμ��÷���û�йر�
				 			LianXu_Paiwu_Close();
				 		}
					if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet-1))
						{
							sys_flag.Error_Code =  Error2_YaBianProtect; //����ѹ��������ȫ��Χ����
						}

					sys_flag.before_ignition_index = 0;
					func_state = SUCCESS;	

					break;

			   default:
			   	sys_flag.before_ignition_index = 0;//�ָ�Ĭ��״̬
			   			sys_close_cmd();
			   			break;
			}

		

		return func_state ;//���ǰ׼����׼�����ˣ�����?
}



/* Self_Check_Function() 已移动到 SYSTEM/error/error_handler.c */
#if 0
 void Self_Check_Function_OLD()
{
	Get_IO_Inf();
	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
	{
		sys_flag.Error_Code = Error16_SmokeValueHigh;
	}
}
#endif

/**
  * @brief  ϵͳ������
* @param   �����ɷ���1�����򷵻�0
  * @retval ��
  */
uint8  Sys_Ignition_Fun(void)
{
		
		sys_data.Data_12H = 0x00; //�������У�û�ж��쳣���м��?
		Abnormal_Events.target_complete_event = 0;
		switch(Ignition_Index)
		{
			case 0 : //  
						sys_flag.Ignition_Count = 0;
						sys_flag.FlameRecover_Time = 0; //�Ը�λʱ���������?
						sys_flag.LianxuWorkTime = 0;  //�Ա��׶ι���ʱ������
						WTS_Gas_One_Close();
					
						/*******************PWM����*һ��������ɨ***********************************/
						Send_Air_Open();  //���ǰ���?		
						//Feed_First_Level();

						delay_sys_sec(10000);
						
						Ignition_Index = 10; //�л����̣�
							
						
					break;

			case 10:
					Send_Air_Open();
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							
						}
					if(sys_time_up)
						{
							
							delay_sys_sec(500);
							Ignition_Index = 1; //�л�����
						}
					else
						{
							
						}

					break;

		case 1:
					Feed_First_Level();
					if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) //�������Ҫ��������
						{
							if(IO_Status.Target.water_mid == WATER_LOSE)
								{
									sys_flag.Force_Supple_Water_Flag = OK;
								}
							if(IO_Status.Target.water_mid == WATER_OK)
								{
									sys_flag.Force_Supple_Water_Flag = FALSE;
									
								}
						}
					else
						{

							if(IO_Status.Target.water_shigh == WATER_LOSE)
								{
									sys_flag.Force_Supple_Water_Flag = OK;
								}
							if(IO_Status.Target.water_shigh == WATER_OK)
								{
									sys_flag.Force_Supple_Water_Flag = FALSE;
									
								}
						}
					
					//ʱ�䵽��Ҳ�����ִ�г���?
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							
						}
					if(sys_time_up)
						{
							sys_time_up = 0;
						
							delay_sys_sec(Sys_Admin.First_Blow_Time);  //��ʽ��ɨʱ�� 

							Ignition_Index = 20; //�л����̣�,����е�����?
						}
					else
						{
							
						}

					
					break;

		case 20:
					Send_Air_Open();
					/*2024��11��28��09:26:13 �����ǰ��ɨ�����У�δ�ж�����ˮλ����ǿ�Ʋ�ˮ������?/
					if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
						{
							if(IO_Status.Target.water_mid == WATER_OK)
								{
									sys_flag.Force_Supple_Water_Flag = FALSE;
									
								}
						}
					else
						{
							if(IO_Status.Target.water_shigh == WATER_OK)
								{
									sys_flag.Force_Supple_Water_Flag = FALSE;
									
								}
						}
					
		
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							
						}

					if(sys_time_up)
						{
							sys_time_up = 0;

							delay_sys_sec(500);
						
							if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
								{
									if(IO_Status.Target.water_mid == WATER_LOSE)
										{
											sys_flag.Force_Supple_Water_Flag = OK;
											delay_sys_sec(90000);
										}
								}
							else
								{
									if(IO_Status.Target.water_shigh == WATER_LOSE)
										{
											sys_flag.Force_Supple_Water_Flag = OK;
											delay_sys_sec(90000);
										}
									
								}

							
							
							Ignition_Index = 2; //�л����̣�,����е�����?
						}
					else
						{
							
						}

					

					break;

		
		 
		case 2://���з����л�,����������任��ע���鳬ѹͣ¯����?
					Send_Air_Open();
					Send_Gas_Close();//ȼ������رգ��رգ��ر�?
					Feed_First_Level();//���ʰٷ�֮60
					
					if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
						{
							if(IO_Status.Target.water_mid == WATER_OK)
								{
									if(sys_flag.Force_Supple_Water_Flag)
										{

											sys_flag.Force_Supple_Water_Flag = FALSE;
											sys_time_up = 1;  //ֱ�ӽ����¸�����
										}
									
								}
						}
					else
						{
							if(IO_Status.Target.water_shigh == WATER_OK)
								{
									sys_flag.Force_Supple_Water_Flag = FALSE;
									sys_time_up = 1;  //ֱ�ӽ����¸�����
								}
						}

					
					if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet-2))
						{
							sys_flag.Error_Code =  Error2_YaBianProtect; //����ѹ��������ȫ��Χ����
						}
			
					if(sys_time_start == 0)
						sys_time_up = 1;
					if(sys_time_up)
						{
							sys_time_up = 0;
								
							if(IO_Status.Target.Air_Door == AIR_CLOSE)//���Źر��򱨾����ߵ�ƽ����
								sys_flag.Error_Code = Error9_AirPressureBad; //���Է��Ź���
							else
								{
									//NOP
								}
								
							//Զ��һ����ͣ��ֱ�ӹ�����ʾ
							if(sys_config_data.Sys_Lock_Set)
								sys_flag.Error_Code = Error9_AirPressureBad; //���Է��Ź���
							else
								{
								//NOP
								}

							
								
							PWM_Adjust(30);//��⹦��?
							if(Sys_Admin.Fan_Speed_Check)
								delay_sys_sec(20000);  //�ȴ����ٱ仯ʱ�䣬��ʱ�򱨾�
							else
								delay_sys_sec(3000);
							Ignition_Index = 3;
							//Mcu_mail_Wifi();
						}

					break;
						
	case 3://��ʽ��ʼ��𣬵�����ȿ�1.5s
					Send_Air_Open();  //���ű����?
					Send_Gas_Close();//ȼ������رգ��رգ��ر�?
					PWM_Adjust(30);//��⹦��?
					Dian_Huo_OFF();  //�رյ��̵���
					sys_flag.Force_Supple_Water_Flag = FALSE;
					//���ǰȷ�ϣ�?
					if (IO_Status.Target.water_protect== WATER_LOSE)
 						{
							sys_flag.Error_Code  = Error5_LowWater;
 						}
					if(Sys_Admin.Fan_Speed_Check)
						{
							if(sys_flag.Fan_Rpm > 200 && sys_flag.Fan_Rpm < Sys_Admin.Fan_Fire_Value)//��������1000ת��2500ת֮��
								{
									delay_sys_sec(8000);//���ӷ����ȶ���ʱ��
									Dian_Huo_Air_Level();//���Ƶ����ٳ���
									Ignition_Index = 4;
								}
							else
								{
									//NOP
								}

							if(sys_time_start == 0)
								{
									sys_time_up = 1;
								}
							else
								{
									//NOP
								}
							if(sys_time_up)
								{
									sys_time_up = 0;
									//���ٿ���ʧ�鱨���ϣ�����
									sys_flag.Error_Code = Error13_AirControlFail; //���Է��Ź���//ϵͳ������־
								}
							else
								{
								//NOP
								}
						}
					else  //�������з��ټ��?
						{
							if(sys_time_start == 0)
								{
									sys_time_up = 1;
								}
							else
								{
									//NOP
								}
							if(sys_time_up)
								{
									Dian_Huo_Air_Level();//���Ƶ����ٳ���
									delay_sys_sec(8000); 
									Ignition_Index = 4;
								}
							else
								{
									//NOP
								}
						}
					
					
					
					
					break;

	case 4:
					Send_Air_Open();  //���ű����?
					Send_Gas_Close();//ȼ������رգ��رգ��ر�?
					Dian_Huo_Air_Level();//���Ƶ����ٳ���
					Dian_Huo_OFF();  //�رյ��̵���
		
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							//NOP
						}
					if(sys_time_up)
						{
							sys_time_up = 0;
							
							
							Dian_Huo_Start();//�������?
							delay_sys_sec(1500);// 
							Ignition_Index = 5;
						}
					else
						{
							
						}
					
					break;
	case 5://��ȼ��2.5s
					Send_Air_Open();  //���ű����?
					Dian_Huo_Air_Level();//���Ƶ����ٳ���
				
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							
						}
					if(sys_time_up)
						{
							 
							sys_time_up = 0;
							// Send_Gas_Open();
							WTS_Gas_One_Open();//ȼ����1
							delay_sys_sec(3500);
							
							Ignition_Index = 6;
						}
					else
						{
							
						}

				break;
					 
	case 6: //�����رգ��ȴ�3��������޻��棬��Ӳ���ӳ�?
					Send_Air_Open();  //���ű����?
					Dian_Huo_Air_Level();//���Ƶ����ٳ���
					 
					 
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							//NOP
						}
					
					if(sys_time_up)
						{
							sys_time_up = 0;

							//Dian_Huo_OFF(); //2023��10��17��12:21:58 ע�͵����д���
							Send_Gas_Open();
							delay_sys_sec(4800);  //�޸� ��1����?.5��
							Ignition_Index = 7;
						}
					else
						{
							
						}
		
					break;
	case 7://3��ʱ�䵽���л������»�һ��ʱ�䣬�޻����򱨾�
					 
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							//NOP
						}
					if(sys_time_up)
						{
							sys_time_up = 0;
							Dian_Huo_OFF();  //�رյ��̵���
							WTS_Gas_One_Close();
							 
							if(sys_flag.flame_state == FLAME_OK )  //�л���
							{
								 //���ɹ����������ԭ״̬���ȴ������ȶ�?
								  delay_sys_sec(Sys_Admin.Wen_Huo_Time);  //�趨�ȶ�����ʱ��10sec��
								 Ignition_Index = 8;//�л����̣����ɹ�������ϵͳ��������״̬	
 
							}
							else  //�޻���
							{
								
								sys_flag.Ignition_Count ++;
								Send_Gas_Close();//�ر�ȼ������
								WTS_Gas_One_Close();
								
								Dian_Huo_OFF();  //�رյ��̵������������ͼ��תΪ���?
								if(sys_flag.Ignition_Count < Max_Ignition_Times)
									{
										//ִ�еڶ��ε��?
										Ignition_Index = 9;
										Feed_First_Level();//
										delay_sys_sec(Sys_Admin.First_Blow_Time);  //�趨�´ε��ʱ�����?0sec + 10������٣�?
				  					}
								else
									{
										sys_flag.Error_Code = Error11_DianHuo_Bad;//ϵͳ������־
										Ignition_Index = 0;
									}
									
							}
						}
					else
						{
							//NOP
						}
					
						
				break;
			case 8: //�ȴ��»���ʱ

					
					Dian_Huo_OFF();
					sys_flag.Force_UnSupply_Water_Flag = FALSE ;  //���Բ�ˮ
					 //��ֹû����ˮλ���ٿ�һ��
					if(sys_flag.flame_state == FLAME_OUT)//�Ȼ���̻���Ϩ��?
						{ 
							sys_flag.Ignition_Count ++;
								Send_Gas_Close();//�ر�ȼ������
								WTS_Gas_One_Close();
								Dian_Huo_OFF();  //�رյ��̵������������ͼ��תΪ���?
								if(sys_flag.Ignition_Count < Max_Ignition_Times)
									{
										//ִ�еڶ��ε��?
										Ignition_Index = 9;
										Feed_First_Level();//
										delay_sys_sec(Sys_Admin.First_Blow_Time);  //�趨�´ε��ʱ�����?0sec + 10������٣�?
									}
								else
									{
										sys_flag.Error_Code = Error11_DianHuo_Bad;//ϵͳ������־
										Ignition_Index = 0;
									}
									
						}

					
						if(sys_time_start == 0)
							{
								sys_time_up = 1;
							}
						else
							{
							
							}
						if(sys_time_up)
						{
							sys_time_up = 0;//�����ȶ�ʱ�䵽
							
	/**************************************��ת���ڶ��׶β�������***START********************************************/
							 sys_flag.Ignition_Count = 0;
				
							return 1;
	/**************************************��ת���ڶ��׶β�������***END********************************************/
						}
						else
						{
							
						}
				
					break;

			case 9://���ʧ�ܣ��л���������?
					Send_Gas_Close();//�ر�ȼ������
					Dian_Huo_OFF();
					WTS_Gas_One_Close();
					
					Feed_First_Level(); 
					if(sys_time_start == 0)
						{
							sys_time_up = 1;
						}
					else
						{
							
						}
					if(sys_time_up)
						{
							sys_time_up = 0;
							Dian_Huo_Air_Level();//���Ƶ����ٳ���
							delay_sys_sec(6000);  //�趨�´ε��ʱ���?5sec��
							Ignition_Index =4;//�л����̣�׼���ٴε��?������?
						}  

					break;
			

			default:
					sys_close_cmd();
					break;
		}

		return 0;
		
}




/**
* @brief  �������ʱ������״̬������δ�����л��棬���Է��ţ�ȼ�ջ��ȱ�����¯�峬�µȽ��ǣ��쳣�������쳣������������?
* @param   �����Ϻ��쳣���з��룬ȼ��ѹ����ϵͳ���к͵���м��
  * @retval ��
  */
/* Auto_Check_Fun() 已移动到 SYSTEM/error/error_handler.c */
#if 0
void Auto_Check_Fun_OLD(void)
{

	uint8 Error_Buffer = 0;
		//***********��ȡ��ת���ڵ�����*************//
		Get_IO_Inf(); //��ȡIO��Ϣ
	
		//����ʱ����Ӧ�ñպϣ��������쳣��˵������û������
		 if(IO_Status.Target.Air_Door == AIR_CLOSE) 
		 	if(sys_flag.Error_Code  == 0 )
				sys_flag.Error_Code  = Error9_AirPressureBad;
		
		if(IO_Status.Target.gas_low_pressure == GAS_OUT)
			{
				//��ȼ��ѹ���ͣ����ϣ�����
				
				if(sys_flag.Error_Code  == 0 )
					sys_flag.Error_Code  = Error3_LowGas; //ȼ��ѹ���͹��ϱ���
			}
		
//��⼫��ˮλ����?
		if (IO_Status.Target.water_protect== WATER_LOSE)
 			{
					Error_Buffer = OK;		
 			}

		if(Error_Buffer)
			sys_flag.Error5_Flag = OK;
		else
			{
				sys_flag.Error5_Flag = 0;
				sys_flag.Error5_Count = 0;
			}
			
	
		if(sys_flag.Error5_Count >= 5)  //ԭ�趨 7�룬 �ָĳ�10�� 2022��5��6��10:11:58
			{
				 //�����У�����ˮλȱˮ����	
				 sys_data.Data_12H = 6; // �¶ȸ����û��趨ֵ0.01kg
				Abnormal_Events.target_complete_event = 1;//�쳣�¼���¼

				//sys_flag.Error_Code  = Error5_LowWater;  //ת����ѹͣ¯�����ٴγ���ˮλ�����⣬ֱ��ͣ¯
				sys_flag.Error5_Flag = 0;
				sys_flag.Error5_Count = 0;
			}
								
//����̽�������?

		if(sys_flag.flame_state == FLAME_OUT) //����0ʱ�����޻����ź�
			{
					Send_Gas_Close();//ȼ������ر�?

					sys_flag.FlameOut_Count++;
					if(sys_flag.FlameOut_Count >= 3)
						{
							sys_flag.Error_Code  = Error12_FlameLose;
							// #region agent log
							// 设备自身触发“运行中火焰熄灭(12)”（用于验证H1/H3：是真检测到无火焰，还是数据被覆盖导致误触发�?
							U5_Printf("{\"sessionId\":\"debug-session\",\"runId\":\"run1-pre\",\"hypothesisId\":\"H1\",\"location\":\"system_control.c:Auto_Check_Fun\",\"message\":\"Error12_FlameLose set\",\"data\":{\"boardAddr\":%d,\"deviceStyle\":%d,\"workState\":%d,\"flameSignal\":%d,\"flameState\":%d,\"flameFilter\":%d,\"flameOutCount\":%d},\"timestamp\":%lu}\r\n",
							          sys_flag.Address_Number,
							          (int)Sys_Admin.Device_Style,
							          (int)sys_data.Data_10H,
							          (int)IO_Status.Target.Flame_Signal,
							          (int)sys_flag.flame_state,
							          (int)sys_flag.FlameFilter,
							          (int)sys_flag.FlameOut_Count,
							          (unsigned long)sys_time_inf.sec * 1000UL);
							// #endregion
						}
					else
						{
							sys_data.Data_12H |= Set_Bit_5; // �¶ȸ����û��趨ֵ0.01kg
							Abnormal_Events.target_complete_event = 1;//�쳣�¼���¼
						}
			}
		
		if(sys_flag.FlameOut_Count)
			{
				//�������ȼ�հ�Сʱ���Զ���Ϩ���¼����
				if(sys_flag.FlameRecover_Time >= 1800)
					sys_flag.FlameOut_Count = 0;
			}
		
		

	

	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
		{
			 
				sys_flag.Error_Code  = Error16_SmokeValueHigh;//�����¶ȳ���
		}
	
		
	if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 1))
		{
			 
			sys_flag.Error_Code  = Error2_YaBianProtect;
	}

		 
}
#endif /* Auto_Check_Fun_OLD */

	
/**
* @brief  �����ʱ������״̬����ȼ�ջ��ȱ�����ȼ��ѹ��״̬���Ƚ��ǹ��ϣ����뱨����ʾ��?
* @param  ���������¯�峬��?���Է���
  * @retval ��
  */
void Ignition_Check_Fun(void)
{
		
		Get_IO_Inf(); //��ȡIO��Ϣ

		if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
		{
			
				sys_flag.Error_Code  = Error16_SmokeValueHigh;//�����¶ȳ���
		}

	 	//ȼ��ѹ��״̬���?
		if(IO_Status.Target.gas_low_pressure == GAS_OUT)
		{
				
				//��ȼ��ѹ���ͣ����ϣ�����
				
				sys_flag.Error_Code = Error3_LowGas;
				
		}
		
	


	

}
	
	

		

/* Idel_Check_Fun() 已移动到 SYSTEM/error/error_handler.c */
#if 0
uint8 Idel_Check_Fun_OLD(void)
{
	if(sys_flag.Error_Code) return 0;
	Get_IO_Inf();
	if (IDLE_INDEX == 0) {
		if(sys_flag.flame_state == FLAME_OK) {
			if(sys_flag.Error_Code == 0)
				sys_flag.Error_Code = Error7_FlameZiJian;
		}
	}
	return 0;
}
#endif







uint8 System_Pressure_Balance_Function(void)
 	{
		

		static	uint16  Man_Set_Pressure = 0;  //1kg = 0.1Mpa  ����ϵͳȫ�ֱ������û��ɵ���,����ʾ�¶�ʱ��300 = 30.0��
		static  uint8 	air_min = 0;//��С����
		static  uint8   air_max = 0;//������
		static	uint16  	stop_wait_pressure = 0; //���ڴﵽĿ���趨ֵʱ��������ʼ��?
		uint8  Tp_value = 0; //���ڷ�������м��?

/*************************����������**************************************************/
		uint16 Real_Pressure = 0;
		static uint8   Yacha_Value = 65;  //�̶�ѹ��0.45Mpa��ԭ��65�����ڵ�����
		uint16 Max_Pressure = 150;  //15����  1.50Mpa
/******************************************************************************************/
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) 
		{
			//�����������ѹ�������Ǹ����û����趨ѹ��������?
			
			Yacha_Value = 65;
			
			Real_Pressure = Temperature_Data.Inside_High_Pressure;
		}
	else
		{
			//��������
			Yacha_Value = 0;
			Real_Pressure = Temperature_Data.Pressure_Value;
		}

	
		air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);//ȡ�����Ϊ��С���й���?

		air_max = Sys_Admin.Max_Work_Power;  //���������й��ʽ��б߽籣��
		if(air_max >= 100)
			air_max = 100;

		if(air_max < 20)
			air_max = 25;


		
		Man_Set_Pressure =sys_config_data.zhuan_huan_temperture_value + Yacha_Value;   // 
		stop_wait_pressure = sys_config_data.Auto_stop_pressure + Yacha_Value;
		
	
		
		Tp_value = now_percent;	

		if(Temperature_Data.Pressure_Value < sys_config_data.zhuan_huan_temperture_value)

			{
				
				if(Real_Pressure < Man_Set_Pressure ) 
					{
					
						if(sys_flag.Pressure_ChangeTime > 6) //8�����ϱ仯0.01����0.1Mpa ��Ҫ100������ʱ��̫���� ����仯ʱ��̫�̣�����С��?���?.01��
							{
								sys_flag.get_60_percent_flag = OK; //��Ҫ����
							}
		
						if(sys_flag.Pressure_ChangeTime <= 5)
							{
								sys_flag.get_60_percent_flag = 0;  //����仯���ʷ���?
							} 
		
						
						if(sys_flag.get_60_percent_flag)
							{
								if(sys_flag.Power_1_Sec)
									{
										sys_flag.Power_1_Sec = 0;
										Tp_value = Tp_value + 1;
									}
							}
						else
							{
								if(sys_flag.Power_5_Sec)
									{
										sys_flag.Power_5_Sec = 0;
										Tp_value = Tp_value + 1;
									}
							}
						
					}
					
			}


		
		
		if(Real_Pressure == Man_Set_Pressure)
			{
		
				if(now_percent > 80)//ǰ���Ǳ������?0
					{
						Tp_value = 80;
					}
			/*��������д����ȱ�ݣ����������С���?/	

				
				sys_flag.get_60_percent_flag = 1;//ȼ��Ԥ�������?
			}

		/********************�������ͣ���ѹ���ⲿѹ������Ҫ�Ƚ��趨ѹ��**********************************************/
		if(Real_Pressure > Man_Set_Pressure  || Temperature_Data.Pressure_Value > sys_config_data.zhuan_huan_temperture_value)
			{
				//˥���ٶ�Ϊÿ���?

				if(Temperature_Data.Pressure_Value > (sys_config_data.zhuan_huan_temperture_value ))
					{
						if(Real_Pressure > Man_Set_Pressure)
							{
								//�������ߣ�Ҫ������
								if(now_percent > 80)//ǰ���Ǳ������?0
									{
										Tp_value = 70;
									}
								if(sys_flag.Power_1_Sec)
									{
										sys_flag.Power_1_Sec = 0;
										Tp_value = Tp_value - 1;
									}
								
							}
						else
							{
								//�ڲ�ѹ��С�����ֵ�����ѹ���ߵ����ޣ����ʵ�������
								if(Real_Pressure <= (Man_Set_Pressure - 5) )
									{
										if(Temperature_Data.Pressure_Value < (sys_config_data.zhuan_huan_temperture_value + 1 ))
											{
												//���ʵ������ʣ���֤��ѹ���ȶ�
												if(sys_flag.Power_1_Sec)
													{
														sys_flag.Power_1_Sec = 0;
														Tp_value = Tp_value + 1;
													}
											}
										else
											{
												//���ѹ���Ѿ��߳������ˣ�ֻ�ܽ�����?
												if(sys_flag.Power_1_Sec)
													{
														sys_flag.Power_1_Sec = 0;
														Tp_value = Tp_value - 1;
													}
											}
										
									}
								else
									{
										//��ѹ����Է�Χ�ڣ���ѹ�����ڵ����趨ѹ��ҲҪ���͹���?
										if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value + 1 ))
											{
												//���ʵ����͹��ʣ���֤��ѹ���ȶ�
												if(sys_flag.Power_1_Sec)
													{
														sys_flag.Power_1_Sec = 0;
														Tp_value = Tp_value - 1;
													}
											}
										
									}


								
							}
						
							
					}
				else
					{
						//û�е����û��趨ѹ���������ڲ�ѹ���Ѿ������趨ֵ����Ҳ��Ҫ������
							if(Real_Pressure > Man_Set_Pressure)
								{
									if(sys_flag.Power_1_Sec)
										{
											sys_flag.Power_1_Sec = 0;
											Tp_value = Tp_value - 1;
										}
								}
					}

				
			}	
			

		now_percent = Tp_value;

		if(now_percent >air_max)
			now_percent = air_max;

		if(now_percent < air_min)
			now_percent = air_min;

						
	 

		if(now_percent >= 70)
			sys_flag.get_60_percent_flag = 1;//ȼ��Ԥ�������?

		
		PWM_Adjust(now_percent);

		


		//�������ѹ�������趨ѹ��?.05Mpa���ϣ���ͣ¯
		if(Real_Pressure >= stop_wait_pressure  || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
			{
				sys_data.Data_12H |= Set_Bit_4; // �¶ȸ����û��趨ֵ0.01kg
				Abnormal_Events.target_complete_event = 1;//�쳣�¼���¼
			 
			}
		
 		return  OK;
 	}


uint8 XB_System_Pressure_Balance_Function(void)
 	{
		

		static	uint16  Man_Set_Pressure = 0;  //1kg = 0.1Mpa  ����ϵͳȫ�ֱ������û��ɵ���,����ʾ�¶�ʱ��300 = 30.0��
		static  uint8 	air_min = 0;//��С����
		static  uint8   air_max = 0;//������
		static	uint16  	stop_wait_pressure = 0; //���ڴﵽĿ���趨ֵʱ��������ʼ��?
		uint8  Tp_value = 0; //���ڷ�������м��?

/*************************����������**************************************************/
		uint16 Real_Pressure = 0;
		static uint8   Yacha_Value = 65;  //�̶�ѹ��0.45Mpa��ԭ��65�����ڵ�����
		uint16 Max_Pressure = 150;  //15����  1.50Mpa
/******************************************************************************************/
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) 
		{
			//�����������ѹ�������Ǹ����û����趨ѹ��������?
			
			Yacha_Value = 65;
			
			Real_Pressure = Temperature_Data.Inside_High_Pressure;
		}
	else
		{
			//��������
			Yacha_Value = 0;
			Real_Pressure = Temperature_Data.Pressure_Value;
		}

	
		air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);//ȡ�����Ϊ��С���й���?

		air_max = Sys_Admin.Max_Work_Power;  //���������й��ʽ��б߽籣��
		if(air_max >= 100)
			air_max = 100;

		if(air_max < 20)
			air_max = 25;


		
		Man_Set_Pressure =sys_config_data.zhuan_huan_temperture_value + Yacha_Value;   // 
		stop_wait_pressure = sys_config_data.Auto_stop_pressure + Yacha_Value;
		
	
		
		Tp_value = now_percent;	

		if(Temperature_Data.Pressure_Value < sys_config_data.zhuan_huan_temperture_value)

			{
				
				if(Real_Pressure < Man_Set_Pressure ) 
					{
					
						if(sys_flag.Pressure_ChangeTime > 6) //8�����ϱ仯0.01����0.1Mpa ��Ҫ100������ʱ��̫���� ����仯ʱ��̫�̣�����С��?���?.01��
							{
								sys_flag.get_60_percent_flag = OK; //��Ҫ����
							}
		
						if(sys_flag.Pressure_ChangeTime <= 5)
							{
								sys_flag.get_60_percent_flag = 0;  //����仯���ʷ���?
							} 
		
						
						if(sys_flag.get_60_percent_flag)
							{
								if(sys_flag.Power_1_Sec)
									{
										sys_flag.Power_1_Sec = 0;
										Tp_value = Tp_value + 1;
									}
							}
						else
							{
								if(sys_flag.Power_5_Sec)
									{
										sys_flag.Power_5_Sec = 0;
										Tp_value = Tp_value + 1;
									}
							}
						
					}
					
			}


		
		
		if(Real_Pressure == Man_Set_Pressure)
			{
		
				if(now_percent > 80)//ǰ���Ǳ������?0
					{
						Tp_value = 80;
					}
			/*��������д����ȱ�ݣ����������С���?/	

				
				sys_flag.get_60_percent_flag = 1;//ȼ��Ԥ�������?
			}

		/********************�������ͣ���ѹ���ⲿѹ������Ҫ�Ƚ��趨ѹ��**********************************************/
		if(Real_Pressure > Man_Set_Pressure  || Temperature_Data.Pressure_Value >= sys_config_data.zhuan_huan_temperture_value)
			{
				//˥���ٶ�Ϊÿ���?

				if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value ))
					{
						//��������ͬʱ���㣬����������
						if(Real_Pressure > Man_Set_Pressure)
							{
								//�������ߣ�Ҫ������
								if(now_percent > 80)//ǰ���Ǳ������?0
									{
										Tp_value = 70;
									}
								if(sys_flag.Power_1_Sec)
									{
										sys_flag.Power_1_Sec = 0;
										Tp_value = Tp_value - 1;
									}
								
							}
						else
							{
								//�ڲ�ѹ��С�����ֵ�����ѹ���ߵ����ޣ����ʵ�������
								if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value + 2 ))
									{
										if(sys_flag.Power_1_Sec)
											{
												sys_flag.Power_1_Sec = 0;
												Tp_value = Tp_value - 1;
											}
									}
								else
									{
										if(Real_Pressure <= (Man_Set_Pressure - 10) )
											{
												if(sys_flag.Power_1_Sec)
													{
														sys_flag.Power_1_Sec = 0;
														Tp_value = Tp_value + 1;
													}
											}
									}
								
								
							}
						
							
					}
				else
					{
						//û�е����û��趨ѹ���������ڲ�ѹ���Ѿ������趨ֵ����Ҳ��Ҫ������
							if(Real_Pressure > Man_Set_Pressure)
								{
									if(sys_flag.Power_1_Sec)
										{
											sys_flag.Power_1_Sec = 0;
											Tp_value = Tp_value - 1;
										}
								}
					}

				
			}	
			

		now_percent = Tp_value;

		if(now_percent >air_max)
			now_percent = air_max;

		if(now_percent < air_min)
			now_percent = air_min;

						
	 

		if(now_percent >= 70)
			sys_flag.get_60_percent_flag = 1;//ȼ��Ԥ�������?

		
		PWM_Adjust(now_percent);

		//�������ѹ�������趨ѹ��?.05Mpa���ϣ���ͣ¯
		if(Real_Pressure >= stop_wait_pressure  || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
			{
				sys_data.Data_12H |= Set_Bit_4; // �¶ȸ����û��趨ֵ0.01kg
				Abnormal_Events.target_complete_event = 1;//�쳣�¼���¼
			 
			}
		
 		return  OK;
 	}




/* Abnormal_Events_Response() 已移动到 SYSTEM/error/error_handler.c */
#if 0
void  Abnormal_Events_Response_OLD(void)
{
		
	
//�������쳣ʱ����һ����ִ�йر�ȼ�����飬�����ʱ��ɨ����ɨʱ�䣬�û��ɵ�?
	 
		
		if (sys_data.Data_12H)
			{
			switch (ab_index)
				{
					case 0:
						   Dian_Huo_OFF();
						   Send_Gas_Close();//�ر�ȼ������	
						   Feed_First_Level();

						   if(sys_flag.LianxuWorkTime < 600) //С��10���ӣ���������
						   	{
						   		sys_flag.get_60_percent_flag = 0;  //���Ż��С��������?
						   	}
						   
						   delay_sys_sec(1000);//ִ�к�ɨ��ʱ5��
						   	ab_index = 1; //��ת����
							break;
					case 1:
							//��ǿ��Ϩ��LCDͼ��
						
							Send_Gas_Close();//�ر�ȼ������
							Dian_Huo_OFF();
							Feed_First_Level();
	
							if(sys_time_start == 0)
								{
									sys_time_up = 1;
								}
							else
								{
								//NOP
								}
								
							if(sys_time_up)
								{
									sys_time_up = 0;
									ab_index = 2; //��ת����
									delay_sys_sec(Sys_Admin.Last_Blow_Time);//
								}
							else
								{
								//NOP
								}

							
							break;
					case 2:
							//��ǿ��Ϩ��LCDͼ��
						
							Send_Gas_Close();//�ر�ȼ������
							Dian_Huo_OFF();
							Feed_First_Level();
					
						if(IO_Status.Target.water_shigh == WATER_LOSE)
							{
								sys_flag.Force_Supple_Water_Flag = OK;
							}
						else
							{
							//NOP
							}
							
						
					
						if(IO_Status.Target.water_shigh == OK)
							{
								sys_flag.Force_Supple_Water_Flag = 0;
							}
						else
							{
								//NOP
							}
							
	
							if(sys_time_start == 0)
								{
									sys_time_up = 1;
								}
							else
								{
								}
								
							if(sys_time_up)
								{
									sys_time_up = 0;
									ab_index = 3; //��ת����
									delay_sys_sec(1000);//
									sys_flag.Force_Supple_Water_Flag = 0;
									/*2023��11��27��11:59:34*/
							
									/*��鼫��ˮλ��״��?/
									if (IO_Status.Target.water_protect== WATER_LOSE)
										{
											sys_flag.Error_Code  = Error5_LowWater;
										}
									else
										{
											
										}
									
								}
							else
								{
									
								}

							
							break;
					
					case 3:
						 
							Dian_Huo_OFF();
						    Send_Gas_Close();//�ر�ȼ������	
							Feed_First_Level();
						
							if(sys_flag.flame_state == FLAME_OK)
								{
									 //��ʱ���϶�û�л��棬����̽�������ϱ���
									sys_flag.Error_Code = Error7_FlameZiJian;
								}
							
							if(sys_time_start == 0)
								{
									sys_time_up = 1;	
								}
							else
								{
									
								}
								
							if(sys_time_up)
								{
									sys_time_up = 0;
									 //��ת����
									//Send_Air_Close();//�رշ��?
									 if(sys_data.Data_12H == 3)
									 		ab_index = 10;//��ת�Զ����۳���
									 else
									 	{
									 		if( Temperature_Data.Pressure_Value <= sys_config_data.Auto_start_pressure   || sys_data.Data_12H == 5 || sys_data.Data_12H == 6)
												{
													sys_data.Data_12H = 0 ;// �¶ȵ���ͣ¯ֵ
													Abnormal_Events.target_complete_event = 0;
													memset(&Abnormal_Events,0,sizeof(Abnormal_Events));//���쳣�ṹ������
											 
													ab_index = 0;  //��index��ʼ���������´ν���
													//���������ָʾҳ�棬�Ӷ������û���ϵͳ�������?
													//�ָ���ز�������������?
													sys_data.Data_10H = SYS_WORK;// ���빤��״̬
													Sys_Staus = 2; //ϵͳ������2�׶Σ���������
													Sys_Launch_Index = 1; //���е��ǰ���
													
													Ignition_Index = 0;  //******************************����ǰ��ɨ��ֱ�ӽ��з��ټ�����
													Send_Air_Open();  //���ǰ���?
													
												//	Feed_First_Level();//��������?									
													delay_sys_sec(1000);//�ӳ�12s
													
													
												}
											else
												{
													ab_index = 4; //��ת����
												}
									 		
									 	}
										 	
								}
							else
								{
									
								}
							
							break;

				case 10:
							 Send_Gas_Close();
							 Send_Air_Close();
							if(Auto_Pai_Wu_Function())
								{
									ab_index = 4; 
									Abnormal_Events.target_complete_event = OK;
									sys_flag.Paiwu_Flag = 0;
								}
							else
								{
									
								}
							
							break;
					case 4:

								Abnormal_Events.target_complete_event = OK;
								if (Abnormal_Events.target_complete_event)
									{
										//˫����Ҫ�򿪣������ر�
											Dian_Huo_OFF();
											Send_Gas_Close();//�ر�ȼ������
											
											if( Temperature_Data.Pressure_Value <= sys_config_data.Auto_start_pressure ||  sys_data.Data_12H == 5 || sys_data.Data_12H == 6)
												{
													sys_data.Data_12H = 0 ;// �¶ȵ���ͣ¯ֵ
													Abnormal_Events.target_complete_event = 0;
													memset(&Abnormal_Events,0,sizeof(Abnormal_Events));//���쳣�ṹ������
											 
													ab_index = 0;  //��index��ʼ���������´ν���
													//���������ָʾҳ�棬�Ӷ������û���ϵͳ�������?
													//�ָ���ز�������������?
													sys_data.Data_10H = SYS_WORK;// ���빤��״̬
													Sys_Staus = 2; //ϵͳ������2�׶Σ���������
													Sys_Launch_Index = 1; //���е��ǰ���
													
													Ignition_Index = 0;  //������ת��ַ�����ǰһ�׶�?
													Send_Air_Open();  //���ǰ���?
													//Feed_First_Level();//��������?									
													delay_sys_sec(1000);//�ӳ�12s
													
												}
											else
												{
													Send_Air_Close();//�رշ��?
												}

									}
							
							break;
					default:
						sys_close_cmd();
						break;
				}
			}
		else
			{
				ab_index = 0;  //��index��ʼ���������´ν���
			}
			


		
}
#endif /* Abnormal_Events_Response_OLD */

/**
  * @brief  ϵͳ���г���
* @param   Sys_Launch_Index�������л�ϵͳ���в���
  * @retval ��
  */
void Sys_Launch_Function(void)
{
		switch(Sys_Launch_Index)
		{
			case  0: //ϵͳ�Լ�
						Self_Check_Function();//���ȼ��ѹ���ͻ�еʽѹ��������?
						
						if(Before_Ignition_Prepare())
						{
								Ignition_Index = 0;
								Sys_Launch_Index = 1;//��ת���¸����̣����׶�
								
						}
						
					break;
			
			case  1: //ϵͳ������
						
						Ignition_Check_Fun();
						if(Sys_Ignition_Fun())
							{
								Sys_Launch_Index = 2;//�л�ϵͳ���̵�������ת״̬
							
								Ignition_Index = 0; //��λ��������ת����

								delay_sys_sec(2000);//����Ҫ��û�У�����һ�׶εĳ���ִ�в����� 

								sys_data.Data_12H = 0; //���쳣����¼��λ
								Abnormal_Events.airdoor_event = 0;
								Abnormal_Events.burner_heat_protect_count = 0;
								Abnormal_Events.flameout_event = 0;
								Abnormal_Events.overheat_event = 0;

								sys_flag.WaterUnsupply_Count = 0; //��ʱ��δ��ˮ��־��ʱ����
							}
						Self_Index = 0;
						ab_index =0;
						Air_Door_Index = 0;

				break;
			
			case  2: //ϵͳ����
			
						sys_flag.Force_Supple_Water_Flag = FALSE; //����״̬�ر�ǿ�Ʋ�ˮ
						sys_flag.Already_Work_On_Flag = OK ;
								
					    if(sys_data.Data_12H == 0)
					    	{
					    		Auto_Check_Fun();  //��û���쳣ʱ��ִ��IO�͸��������?
				   				System_Pressure_Balance_Function();
								
								
								if(sys_flag.Paiwu_Flag)
									sys_data.Data_12H = 3 ;//��Ҫ�������۵ı�־
					    	}
						else//�쳣״̬��һЩ״̬���ļ��?
							{
								Abnormal_Check_Fun();
							}
	
						Abnormal_Events_Response(); //�쳣���?
						
					break;
			
			default:
					sys_close_cmd();
					Sys_Launch_Index = 0;
					break;
		}	
}





/* Abnormal_Check_Fun() 已移动到 SYSTEM/error/error_handler.c */
#if 0
void Abnormal_Check_Fun_OLD(void)
{
		Get_IO_Inf();
	
		
		if(IO_Status.Target.gas_low_pressure == GAS_OUT)
		{
				
				//��ȼ��ѹ���ͣ����ϣ�����
				if(sys_flag.Error_Code  == 0 )
					sys_flag.Error_Code  = Error3_LowGas; //ȼ��ѹ���͹��ϱ���
				
		}
		


	


	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
		{
		
			sys_flag.Error_Code  = Error16_SmokeValueHigh;//�����¶ȳ���
		}
	
		
	if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 1))
		{
			

		sys_flag.Error_Code  = Error2_YaBianProtect;
	}
}
#endif /* Abnormal_Check_Fun_OLD */

/* Lcd_Err_Refresh(), Lcd_Err_Read(), Err_Response() 已移动到 error_handler.c */
#if 0
void Lcd_Err_Refresh_OLD(void) {}
void Lcd_Err_Read_OLD(void) {}

void  Err_Response_OLD(void)
{
	static uint8 Old_Error = 0;
	//����й��ϱ�����ͣ¯��?4H��15HΪ��������
	  if(sys_flag.Error_Code == 0)
	  	{
	  		if(sys_flag.Lock_Error)
				sys_flag.tx_hurry_flag = 1;//�����������ݸ�������
				
	  			sys_flag.Error_Code = 0;
	  			sys_flag.Lock_Error = 0;//�Թ��Ͻ���
				Beep_Data.beep_start_flag = 0;	//�����������?
					
	  	}

	/******************�����ظ����ϼ�¼��ʱ��************************/
	  if(sys_flag.Old_Error_Count >=1800)
	  	{
	  		Old_Error = 0; //�ٴμ�¼
	  		sys_flag.Old_Error_Count = 0;
	  	}
	  else
	  	{
	  	
	  	}

	  
	 //������ǰ���Ǳ����ȼ���
	 if(sys_flag.Lock_Error == 0)
	 	{
	 		if(sys_flag.Error_Code )
				{
			 		sys_close_cmd();
			 		sys_flag.Lock_Error = 1;  //�Թ��Ͻ�������
			 		sys_flag.Alarm_Out = OK;
			 		Beep_Data.beep_start_flag = 1;//���Ʒ���������	
					
					
					if(sys_flag.Error_Code != Old_Error)
							{
								Old_Error = sys_flag.Error_Code;
								
							}
						else
							{
							
							}

						sys_flag.Old_Error_Count = 0; //���ϼ�¼ʱ������

					
				
				}
			
	 	}
	 else
	 	{
	 		if(sys_flag.Error_Code )
	 			{
	 				if(sys_data.Data_10H == 2 )
	 					{
	 						sys_close_cmd();
							sys_flag.Alarm_Out = OK;
			 				Beep_Data.beep_start_flag = 1;//���Ʒ���������	
	 					}
	 			}
	 	}
}
#endif /* Err_Response_OLD */

/* IDLE_Err_Response() 已移动到 error_handler.c */
#if 0
void  IDLE_Err_Response_OLD(void)
{
	static uint8 Old_Error = 0;
	//����й��ϱ�����ͣ¯��?
	  if(sys_flag.Error_Code == 0)
	  	{
	  		if(sys_flag.Lock_Error)
				sys_flag.tx_hurry_flag = 1;//�����������ݸ�������

			sys_flag.Error_Code = 0;
	  			sys_flag.Lock_Error = 0;  //�Թ��Ͻ���
					Beep_Data.beep_start_flag = 0;	//�����������?
					
	  	}

	  
	/******************�����ظ����ϼ�¼��ʱ��************************/
	  if(sys_flag.Old_Error_Count >=1800)
	  	{
	  		Old_Error = 0; //�ٴμ�¼
	  		sys_flag.Old_Error_Count = 0;
	  	}
	  else
	  	{
	  	
	  	}


		//����й��ϱ�����ͣ¯��?
		 if (sys_flag.Lock_Error == 0)
 		 	{	
		  		
  				if(sys_flag.Error_Code && sys_flag.Error_Code != 0xFF)
  					{
  						
						Sys_Staus = 0;  //ϵͳ���뱨������
						
						
						
						if(sys_data.Data_10H == 2)
							{
								sys_close_cmd();
							}
						else
							{
								
							}
						
						
						Beep_Data.beep_start_flag = 1;	
						sys_flag.Lock_Error = 1;  //�Թ��Ͻ�������
						sys_flag.Alarm_Out = OK;
						sys_flag.tx_hurry_flag = 1;//�����������ݸ�������
						//ˢ��LCD������Ϣ��¼����
						if(sys_flag.Error_Code != Old_Error)
							{
								Old_Error = sys_flag.Error_Code;
							
							}
						else
							{
							
							}

						sys_flag.Old_Error_Count = 0; //���ϼ�¼ʱ������
						
					
						
						
  					}
		  				
		  			
				
			}
	
	
	  
#endif /* IDLE_Err_Response_OLD */



/**
* @brief  ϵͳ�������ر����е�����ȴ������������������ʹ���ָ��
* @param   
  * @retval ��
  */
void System_Idel_Function(void)
{
	//1��	�ùص�ȫ���ص� 
		
		Send_Air_Close();
 		Dian_Huo_OFF();//���Ƶ��̵����ر�
		Send_Gas_Close();//ȼ������ر�?
		WTS_Gas_One_Close();
		
		Auto_Pai_Wu_Function();
	
		 
}

/**
* @brief  ϵͳ�ܿس���
* @param   
  * @retval ��
  */
void System_All_Control()
{
		Sys_Staus = sys_data.Data_10H;

		switch (Sys_Admin.Device_Style)
			{
				case 0:
				case 1:
						if(Sys_Admin.Water_BianPin_Enabled)
							{
								
								Water_BianPin_Function();//��Ƶ��ˮģʽ
							}
						else
							{
								Water_Balance_Function();//���油ˮģʽ
							}
						
						break;
				case 2:
				case 3:
						//�Ǳ�Ƶ��ˮ
						if(Sys_Admin.Water_BianPin_Enabled)
							{
								Double_Water_BianPin_Function();
							}
						else
							{
								ShuangPin_Water_Balance_Function();
								if(sys_flag.Address_Number == 0)
									{
										Double_WaterPump_LogicFunction();
									}
							}
					
						break;

				default:

						break;
					
			}
	//��ˮ����

		if(sys_flag.Work_1S_Flag)
			{
				//ȡ������������е�ʱ�䣬Ȼ����������ڵ������еĴ�ɨʱ�����?
				sys_flag.Work_1S_Flag = 0;
				if(sys_data.Data_1FH > 0)
					{
						sys_flag.AirWork_Time++;
					}
				else
					{
						sys_flag.AirWork_Time = 0;
					}
			}
		
		

		switch(Sys_Staus)
			{

					case 0 :	//ϵͳ����

						 switch(IDLE_INDEX)
						 	{
						 		case  0 : //��������״̬  ,, ע�����״̬ѭ��ˮ�õĿ��������ݻ�ˮ�¶�?
						 				
						 				sys_flag.Ignition_Count = 0;//����ʱ�Ե���������
										sys_flag.last_blow_flag = 0;//��ɨ״̬������־
									
										 System_Idel_Function( );//�������ܴ���
										//��������������ʵʱ��ʾ�����ϣ�ֻ���Ѳ�ִ��
										 Idel_Check_Fun();
										 IDLE_Err_Response();
										 Sys_Launch_Index = 0;
										break;

								case  1: //�ȴ���ɨ��ʱ
									 
										Send_Gas_Close();//ȼ������ر�?
									 	Dian_Huo_OFF();//���Ƶ��̵����ر�
										sys_data.Data_14H &= Rst_Bit_0;//���״̬���⣬���ű���һ����������ٱ���
										Get_IO_Inf();
										sys_flag.Force_Supple_Water_Flag = 0;
										if(sys_time_start == 0)
											{
												sys_time_up = 1;
											}
										else
											{
												
											}
										if(sys_time_up)
										{
											sys_time_up = 0;
											IDLE_INDEX = 2;//������������״̬
											//�رշ������ɨ�������������
											Send_Air_Close();
										}
										break;
								case 2: //�ȴ������������£���ֹ���⣬���?0������
									  Send_Air_Close();//�����Դ�ر�?
									  Send_Gas_Close();//ȼ������ر�?
									  Dian_Huo_OFF();//���Ƶ��̵����ر�
									 
									  Get_IO_Inf();
									  IDLE_Err_Response();
									
	 									sys_time_up = 0;
	 									IDLE_INDEX = 0;//������������״̬
	 									Last_Blow_End_Fun();//��ɨ����ִ�е�λ
	 									sys_flag.Force_Supple_Water_Flag = 0;
										sys_flag.Force_UnSupply_Water_Flag = FALSE ;
	
										break;

								default :
										Sys_Staus = 0;
										IDLE_INDEX = 0;
										break;
						 	}
					
							
						break;
					
					case 2:		//ϵͳ����
						
						Sys_Launch_Function();
						 //���ڿ����ٶ�
						Err_Response();//���д���״̬��Ӧ
						break;
			
					case 3://�ֶ�����״̬
							//�ֶ�ģʽ��1�� ��ˮλ����ʧ���Զ���ˮ����ˮλ��ͣ
							
							//��ת�ٵ�ֵ����LCD��ʾ
							
							if(IO_Status.Target.hot_protect == THERMAL_BAD)
								{
									if(sys_flag.Error_Code == 0 )
										sys_flag.Error_Code = Error15_RebaoBad;
								}

		
							//��еʽѹ������ź�?
							if(IO_Status.Target.hpressure_signal == PRESSURE_ERROR)
								{
											
									//������ѹ��������ȫ��Χ�����ϣ�����
									 if(sys_flag.Error_Code == 0 )
										sys_flag.Error_Code = Error1_YakongProtect; //����ѹ��������ȫ��Χ����			
								}
							Send_Gas_Close();//ȼ������ر�?
							
							IDLE_Err_Response();
			
							break;


					case 4://���ϱ���ģʽ
							
							if(sys_flag.Error_Code == 0)
									{
										if(sys_flag.Lock_Error)
											sys_flag.tx_hurry_flag = 1;//�����������ݸ�������
							
										sys_flag.Error_Code = 0;
										sys_flag.Lock_Error = 0;  //�Թ��Ͻ���
										Beep_Data.beep_start_flag = 0;	//�����������?

										//Ҫ����״̬��ת
									}

							break;


			
					
					default:
						Sys_Staus = 0;
						IDLE_INDEX = 0;
						break;
				
			}
			
			
}
 


uint8   sys_work_time_function(void)
{
//ϵͳ�ۼ�����ʱ��,��¯����ʱ��
	 uint16  data = 0;
	static uint8 Work_State = 0;
	static uint8 Main_secs = 0;
	
		
			 
	data = sys_time_inf.All_Minutes / 60;
	 
	 

	
	lcd_data.Data_21H = data >> 8;
	lcd_data.Data_21L = data & 0x00FF; //���ڽ�����ʾ
	

	if(sys_data.Data_10H == 0)
		{
			//�ϸ�״̬ʱ����״̬ 
			if(Work_State == 2)
				{
					//˵�����ؽ��йػ������������ݽ��б���
					//Write_Second_Flash();
				}
			Work_State = sys_data.Data_10H ;
		}
	else
		Work_State =sys_data.Data_10H;

	
	//������1��ʱ����ڴ���״̬��ֱ���˳�?
	if(sys_flag.Work_1sec_Flag == FALSE || sys_data.Data_10H == 0)
		return 0;

	
	sys_flag.Work_1sec_Flag = FALSE; //������־λ
		
	 if(sys_data.Data_10H == 2 )
	 	{
	 		 
	 		if(sys_flag.flame_state)
	 			{
	 				Main_secs ++ ;
					if(Main_secs == 60)
						{
							Main_secs = 0;
							sys_time_inf.All_Minutes++;
						}
	 			}

			 
	 	}


	 return 0;
			

}


void copy_to_lcd(void)
{
	
	
	
}



void sys_control_config_function(void)
{

//���ÿ���ϵͳĬ�ϲ�������
	uint16  data_temp = 0;
	uint8 temp = 0;


	data_temp =  *(uint32 *)(CHECK_FLASH_ADDRESS);
	if(data_temp != FLASH_BKP_DATA) 
		{
			
			for(temp = 1; temp <= 4; temp ++)
				{
					SlaveG[temp].Key_Power = OK;
					SlaveG[temp].Fire_Power = 30;
					SlaveG[temp].Max_Power = 85;
					SlaveG[temp].Smoke_Protect = 85;
					SlaveG[temp].Inside_WenDu_ProtectValue = 270;//
					
				}
			LCD10D.DLCD.YunXu_Flag = SlaveG[1].Key_Power; 
			
			sys_flag.Wifi_Lock_System = 0; //Ĭ��wifiû����
			sys_flag.wifi_Lock_Year = 0;
			sys_flag.wifi_Lock_Month = 0;
			sys_flag.Wifi_Lock_Day = 0;

			
			Sys_Admin.ChaYa_WaterHigh_Enabled = 0;
			Sys_Admin.ChaYa_WaterLow_Set = 100; //200mm
			Sys_Admin.ChaYa_WaterMid_Set = 180; // 
			Sys_Admin.ChaYa_WaterHigh_Set = 230; // 
			
 			

			Sys_Admin.Device_Style  = 0;  //0 ���ǳ��浥��1��������1�������������?
		
			
			Sys_Admin.LianXu_PaiWu_DelayTime = 10; //Ĭ��15���Ӷ���һ�Σ�ÿ��3��
			Sys_Admin.LianXu_PaiWu_OpenSecs = 4; //���ȵ�1s,Ĭ�Ͽ���3��

			Sys_Admin.Water_BianPin_Enabled = 0;  //Ĭ�ϲ��򿪱�Ƶ��ˮ����
			Sys_Admin.Water_Max_Percent = 45; 
			
			
			Sys_Admin.YuRe_Enabled  = 1; //Ĭ�ϴ򿪸��±���
			Sys_Admin.Inside_WenDu_ProtectValue  = 270;// �����¶�Ĭ��Ϊ270��

			Sys_Admin.Steam_WenDu_Protect  = 173;//ȡ������Ҫ �����¶�Ĭ��Ϊ180��
		
			Sys_Admin.Special_Secs = 18;
			 
			sys_time_inf.UnPaiwuMinutes = 0;
		
			
			Sys_Admin.Balance_Big_Time = 90;
			Sys_Admin.Balance_Small_Time = 150;
		
			Sys_Admin.DeviceMaxPressureSet = 100; //Ĭ�϶ѹ����10����
			
		//��һ���� ����Ӧ�ṹ�帳ֵ
			Sys_Admin.First_Blow_Time = 30 * 1000;  //ǰ��ɨʱ��
	 	
	
			Sys_Admin.Last_Blow_Time = 30 *1000;//��ɨʱ��
			

			Sys_Admin.Dian_Huo_Power = 30;  //Ĭ�ϵ�����?0% 
		
			Sys_Admin.Max_Work_Power = 85;  //Ĭ��������?00
			Sys_Admin.Wen_Huo_Time =6 * 1000;  //�ȶ�����ʱ�� 10��

			Sys_Admin.Fan_Speed_Check = 1;  //Ĭ���Ǽ�����	
			
			 Sys_Admin.Fan_Speed_Value = 4800; //Ĭ�Ϸ�����ת��Ϊ6600��

			  Sys_Admin.Fan_Pulse_Rpm = 3;   //Ĭ��ÿת������3����Amtek 

			 Sys_Admin.Fan_Fire_Value = 3000 ; //Ĭ�Ϸ�������ת��Ϊ3500rpm

			 Sys_Admin.Danger_Smoke_Value =  850; //�����¶�Ĭ��ֵΪ80��
			 Sys_Admin.Supply_Max_Time =  320; //��ˮ��ʱĬ�ϱ���ֵΪ300��
			
			 Sys_Admin.ModBus_Address = 0; //Ĭ�ϵ�ַΪ20

			 sys_config_data.Sys_Lock_Set = 0;  //Ĭ�ϲ�������ͣ����
 
		  
		   	sys_config_data.Auto_stop_pressure = 60; //������4kg,ͣ¯Ĭ��Ϊ5kg

			sys_config_data.Auto_start_pressure = 40; //������4kg,����ѹ����1kg������  
	 		sys_config_data.zhuan_huan_temperture_value = 50; //����Ŀ��ѹ��ֵ0.4Mpa
	 		
			
			Sys_Admin.Admin_Work_Day = 0; //������ʱ�������Ĭ���?��Ĭ�ϲ�����
			Sys_Admin.Admin_Save_Day = 30;
			Sys_Admin.Admin_Save_Month = 12;
			Sys_Admin.Admin_Save_Year = 2025;
			
		//��һ���� д���ڲ�FLASH
			sys_flag.Lcd_First_Connect = OK;

			
	 	 	sys_time_inf.All_Minutes = 1; 
			 
			Write_Internal_Flash();
			Write_Admin_Flash();
			Write_Second_Flash();
			 
			
			
		}
	else  //˵���Ѿ�д�������������ڴ������������?�����ڲ�FLASH���ݣ���ֵ����Ӧ�ṹ��
		{
				
			Sys_Admin.Fan_Pulse_Rpm = *(uint32 *)(FAN_PULSE_RPM_ADDRESS);

			Sys_Admin.ChaYa_WaterHigh_Enabled =  *(uint32 *)(CHAYA_WATER_ENABLED);
			Sys_Admin.ChaYa_WaterLow_Set =  *(uint32 *)(CHAYA_WATERLOW_SET); //200mm
			Sys_Admin.ChaYa_WaterMid_Set = *(uint32 *)(CHAYA_WATERMID_SET);; // 
			Sys_Admin.ChaYa_WaterHigh_Set = *(uint32 *)(CHAYA_WATERHIGH_SET);; // 

		
			sys_flag.Wifi_Lock_System = *(uint32 *)(WIFI_LOCK_SET_ADDRESS);
			sys_flag.wifi_Lock_Year =  *(uint32 *)(WIFI_LOCK_YEAR_ADDRESS);
			sys_flag.wifi_Lock_Month =  *(uint32 *)(WIFI_LOCK_MONTH_ADDRESS);
			sys_flag.Wifi_Lock_Day =  *(uint32 *)(WIFI_LOCK_DAY_ADDRESS);

			 
			Sys_Admin.Device_Style =  *(uint32 *)(Device_Style_Choice_ADDRESS);


		
			Sys_Admin.LianXu_PaiWu_Enabled = *(uint32 *)(LianXu_PaiWu_Enabled_ADDRESS);
			Sys_Admin.LianXu_PaiWu_DelayTime = *(uint32 *)(LianXu_PaiWu_DelayTime_ADDRESS);
			Sys_Admin.LianXu_PaiWu_OpenSecs = *(uint32 *)(LianXu_PaiWu_OpenSecs_ADDRESS);
		
			Sys_Admin.Water_BianPin_Enabled = *(uint32 *)(WATER_BIANPIN_ADDRESS);
			Sys_Admin.Water_Max_Percent = *(uint32 *)(WATER_MAXPERCENT_ADDRESS);
			
		
			Sys_Admin.YuRe_Enabled  = *(uint32 *)(WENDU_PROTECT_ADDRESS);

			Sys_Admin.Inside_WenDu_ProtectValue  = *(uint32 *)(BENTI_WENDU_PROTECT_ADDRESS);//�����¶�
		//	Sys_Admin.Steam_WenDu_Protect  = *(uint32 *)(STEAM_WENDU_PROTECT_ADDRESS);//�����¶�
		
			Sys_Admin.Special_Secs = *(uint32 *)(SPECIAL_SECS_ADDRESS);
			
			
		
			Sys_Admin.Balance_Big_Power = *(uint32 *)(BALANCE_BIGPOWER_ADDRESS);
			Sys_Admin.Balance_Small_Power = *(uint32 *)(BALANCE_SMALLPOWER_ADDRESS);
			Sys_Admin.Balance_Big_Time = *(uint32 *)(BALANCE_BIGTIME_ADDRESS);
			Sys_Admin.Balance_Small_Time = *(uint32 *)(BALANCE_SMALLTIME_ADDRESS);

			
		
			Sys_Admin.Access_Data_Mode = *(uint32 *)(ACCESS_DATA_MODE_SET_ADDRESS);	
			
			Sys_Admin.DeviceMaxPressureSet = *(uint32 *)(DEVICE_MAX_PRESSURE_SET_ADDRESS);
			
			
			 sys_config_data.Sys_Lock_Set =  *(uint32 *)(SYS_LOCK_SET_ADDRESS); 

			  Sys_Admin.Supply_Max_Time =*(uint32 *)(SUPPLY_MAX_TIME_ADDRESS); 
			
			Sys_Admin.First_Blow_Time = *(uint32 *)(FIRST_BLOW_ADDRESS);  //Ԥ��ɨʱ��
			
		
			Sys_Admin.Last_Blow_Time =  *(uint32 *)(LAST_BLOW_ADDRESS);//��ɨʱ��
			
			
			Sys_Admin.Dian_Huo_Power =  *(uint32 *)(DIAN_HUO_POWER_ADDRESS);  //�����?
			


			Sys_Admin.Max_Work_Power = *(uint32 *)(MAX_WORK_POWER_ADDRESS);  //Ĭ��������?00
			
			Sys_Admin.Wen_Huo_Time = *(uint32 *)(WEN_HUO_ADDRESS);  //�ȶ�����ʱ��  

			Sys_Admin.Fan_Speed_Check = *(uint32 *)(FAN_SPEED_CHECK_ADDRESS);  //�Ƿ���з��ټ��
			
			Sys_Admin.Fan_Speed_Value = *(uint32 *)(FAN_SPEED_VALUE_ADDRESS);  //�Ƿ���з��ټ��
			Sys_Admin.Fan_Fire_Value = *(uint32 *)(FAN_FIRE_VALUE_ADDRESS);

			Sys_Admin.Danger_Smoke_Value = *(uint32 *)(DANGER_SMOKE_VALUE_ADDRESS);
			 
			
			 Sys_Admin.ModBus_Address = *(uint32 *)(MODBUS_ADDRESS_ADDRESS) ;  
			
			sys_config_data.wifi_record = *(uint32 *)(CHECK_WIFI_ADDRESS);  //ȡ��wifi��¼��ֵ

			sys_config_data.zhuan_huan_temperture_value = *(uint32 *)(ZHUAN_HUAN_TEMPERATURE); //����Ŀ��ѹ��ֵ0.4Mpa

			sys_config_data.Auto_stop_pressure = *(uint32 *)(AUTO_STOP_PRESSURE_ADDRESS); //ȡ���Զ�ͣ¯ѹ��

			sys_config_data.Auto_start_pressure = *(uint32 *)(AUTO_START_PRESSURE_ADDRESS);//ȡ���Զ���¯ѹ��

				SlaveG[1].Key_Power = *(uint32 *)(A1_KEY_POWER_ADDRESS) ;
				
				SlaveG[2].Key_Power = *(uint32 *)(A2_KEY_POWER_ADDRESS) ;
				SlaveG[2].Fire_Power = *(uint32 *)(A2_FIRE_POWER_ADDRESS) ;
				SlaveG[2].Max_Power = *(uint32 *)(A2_MAX_POWER_ADDRESS);
				SlaveG[2].Smoke_Protect = *(uint32 *)(A2_SMOKE_PROTECT_ADDRESS);
				SlaveG[2].Inside_WenDu_ProtectValue = *(uint32 *)(A2_INSIDESMOKE_PROTECT_ADDRESS);

				SlaveG[3].Key_Power = *(uint32 *)(A3_KEY_POWER_ADDRESS) ;
				SlaveG[3].Fire_Power = *(uint32 *)(A3_FIRE_POWER_ADDRESS) ;
				SlaveG[3].Max_Power = *(uint32 *)(A3_MAX_POWER_ADDRESS);
				SlaveG[3].Smoke_Protect = *(uint32 *)(A3_SMOKE_PROTECT_ADDRESS);
				SlaveG[3].Inside_WenDu_ProtectValue = *(uint32 *)(A3_INSIDESMOKE_PROTECT_ADDRESS);

			/**********************��ʷ������Ϣ��ȡ  *************************************/
			
			/**********************��ʷ������Ϣ��ȡ  ����*************************************/		
			
		}

		LCD10D.DLCD.YunXu_Flag = SlaveG[1].Key_Power; 

		
	     Sys_Admin.Admin_Work_Day = *(uint32 *)(ADMIN_WORK_DAY_ADDRESS); 
		 Sys_Admin.Admin_Save_Day = *(uint32 *)(ADMIN_SAVE_DAY_ADDRESS);
		 Sys_Admin.Admin_Save_Month = *(uint32 *)(ADMIN_SAVE_MONTH_ADDRESS);
		 Sys_Admin.Admin_Save_Year = *(uint32 *)(ADMIN_SAVE_YEAR_ADDRESS);


  	
	 	 sys_time_inf.All_Minutes = *(uint32 *)(SYS_WORK_TIME_ADDRESS); 

	 

  //���գ������ݷ���LCDչʾ
	
	
}



//��������Ϣ������ת��Ϊbit,��������ˢ��lcd������
uint8  byte_to_bit(void)
{
	 

	


		return 0;
}













//����LCD����MCU������
void Load_LCD_Data(void)
{
	
}





void clear_struct_memory(void)
{
	uint8 temp = 0;
		//�Խṹ�������ʼ��?
	memset(&sys_data,0,sizeof(sys_data));	//��״̬��Ϣ�ṹ������
  	memset(&lcd_data,0,sizeof(lcd_data));	//��״̬��Ϣ�ṹ������
	memset(&sys_time_inf,0,sizeof(sys_time_inf));	//��״̬��Ϣ�ṹ������
	
	memset(&sys_config_data,0,sizeof(sys_config_data));	//��״̬��Ϣ�ṹ������
	
	
	memset(&Switch_Inf,0,sizeof(Switch_Inf));//��ϵͳ��־����������
	memset(&Abnormal_Events,0,sizeof(Abnormal_Events));//���쳣�ṹ������
	memset(&sys_flag,0,sizeof(sys_flag));//��ϵͳ��־����
	
	memset(&Flash_Data,0,sizeof(Flash_Data));
	memset(&Temperature_Data,0,sizeof(Temperature_Data));
	 
	
	
}








void One_Sec_Check(void)
{
 	 
	 //�������Ч������֤��������������?
	if(sys_flag.Relays_3Secs_Flag)
		{
			sys_flag.Relays_3Secs_Flag = 0;

	//	u1_printf("\n* ͨ�ŵ�ַ���� = %d\n",Sys_Admin.ChaYa_WaterHigh_Set);
	//	u1_printf("\n* ��Һλ�߶� = %d\n",Sys_Admin.ChaYa_WaterMid_Set);
	
	//		u1_printf("\n* �ӻ�С����ʱ�� = %d\n",SlaveG[2].Small_time);
	//		u1_printf("\n* Сƽ���趨ʱ�� = %d\n",Sys_Admin.Balance_Small_Time);

		
		

			
		}
	 
	
	
		



				



	

	
//��ӡ������Ϣ
	if(sys_flag.two_sec_flag)
		{
			sys_flag.two_sec_flag = 0;
			
			//sys_flag.LianxuWorkTime
			//u1_printf("\n* ���õ�ʱ��= %d\n",Sys_Admin.LianXu_PaiWu_DelayTime);
			//u1_printf("\n* �Ѿ����е�ʱ��= %d\n",sys_flag.LianxuWorkTime);
			//u1_printf("\n* s���ÿ�����ʱ��= %d\n",Sys_Admin.LianXu_PaiWu_OpenSecs);
			
			//u1_printf("\n* ������ʱ��= %d\n",sys_flag.Lianxu_OpenTime);
		//	u1_printf("\n* ��ˮ�ı�־= %d\n",Switch_Inf.water_switch_flag);

			
		}

		
	
	
}



uint8  sys_start_cmd(void)
{
		

		if(sys_flag.Lock_System)
			{
				//��ת�����Ͻ��棬���޹��ϴ�����ʾ
				
				return 0 ;
			}
		
		 
		if(sys_flag.Error_Code)
			{
					 	Sys_Staus = 0;  // 
						sys_data.Data_10H = 0x00;  //ϵͳֹͣ״̬
						sys_data.Data_12H = 0x00; //�Է��������쳣��������

						
						
						delay_sys_sec(100);// 
					
						IDLE_INDEX = 1; 

						sys_flag.Lock_Error = 1;  //�Թ��Ͻ�������
						sys_flag.tx_hurry_flag = 1;//�����������ݸ�������
						Beep_Data.beep_start_flag = 1;	
						
			}
		else
			{
				if(sys_data.Data_10H == 0)
					{
						IDLE_INDEX = 0;  //��ֹ�ں�ɨʱ�����?
						Sys_Staus = 2;
						Sys_Launch_Index = 0;
						sys_flag.before_ignition_index = 0;
						Ignition_Index = 0;
						sys_time_up = 0;	

	   					 sys_data.Data_10H = 0x02;  //ϵͳ����״̬
					
						sys_flag.Paiwu_Flag = 0; //����д������ʲôԭ����
						
						
	    				sys_time_start = 0; //�������״̬�£����ܴ��ڵ���ʱ�ȴ�����ֹ�����ϵͳ
					/************�Դ���ѭ���ù���ʱ���������?****************8*/
						
						sys_flag.Already_Work_On_Flag = FALSE;
					
						sys_flag.get_60_percent_flag = 0;
						sys_flag.Pai_Wu_Idle_Index = 0;

						sys_flag.before_ignition_index = 0;	
						sys_flag.tx_hurry_flag = 1;//�����������ݸ�������											
	    				Dian_Huo_OFF();//���Ƶ��̵����ر�
	    				//LCD�л�����ҳ��
	    				
					}
				
				
			}
	    
		
	return 0;							
}


void sys_close_cmd(void)
{
	// #region agent log
	// 若因“火焰熄�?12)”进入停机流程，记录一次（用于确认“从机停机”是否由Error12触发�?
	if(sys_flag.Error_Code == Error12_FlameLose)
	{
		U5_Printf("{\"sessionId\":\"debug-session\",\"runId\":\"run1-pre\",\"hypothesisId\":\"H1\",\"location\":\"system_control.c:sys_close_cmd\",\"message\":\"sys_close_cmd with Error12\",\"data\":{\"boardAddr\":%d,\"deviceStyle\":%d,\"workState\":%d,\"errorCode\":%d,\"flameSignal\":%d,\"flameState\":%d},\"timestamp\":%lu}\r\n",
		          sys_flag.Address_Number,
		          (int)Sys_Admin.Device_Style,
		          (int)sys_data.Data_10H,
		          (int)sys_flag.Error_Code,
		          (int)IO_Status.Target.Flame_Signal,
		          (int)sys_flag.flame_state,
		          (unsigned long)sys_time_inf.sec * 1000UL);
	}
	// #endregion

			sys_data.Data_10H = 0x00;  //ϵͳֹͣ״̬
																		
			lcd_data.Data_16H = 0X00;
			lcd_data.Data_16L = 0x00;  //ϵͳˢ�¿���ͼ�꣬��ʾSTART
			//ϵͳֹͣ���Թؼ����ݽ��д洢
		 	WTS_Gas_One_Close();
		  	
		
			Abnormal_Events.target_complete_event = 0;
			Dian_Huo_OFF();//�رյ��̵���
			Send_Gas_Close();//�ر�ȼ������ 
			
			sys_flag.get_60_percent_flag = 0;
		
			 
			sys_flag.tx_hurry_flag = 1;//�����������ݸ�������
			 
			Write_Second_Flash(); //��������ʱ����?
		  //���ϴγ����п��ܴ��ڵ��쳣״̬������0
		memset(&Abnormal_Events,0,sizeof(Abnormal_Events));	//��״̬��Ϣ�ṹ������			
														
		//���к�ɨ��ʱ
		//�򿪷��������ɨ���?
		//�������״�?
		//��׼��ת����
		sys_data.Data_10H = SYS_IDLE; // 
		Sys_Staus = 0; // 
		Sys_Launch_Index = 0;
		sys_flag.before_ignition_index = 0;
		Ignition_Index = 0;
		IDLE_INDEX = 1;
		Last_Blow_Start_Fun();
	
}


//��ɨ��ʼִ�г���
void Last_Blow_Start_Fun(void)
{
	//ȷ�Ϸ���Ѿ���?
	Send_Air_Open();

	sys_flag.last_blow_flag = 1;//��ɨ״̬��ʼ��־
	 
	sys_flag.tx_hurry_flag = 1;//�����������ݸ�������

	
	
	Feed_First_Level();//90%�ķ������к�ɨ
	if(sys_flag.Already_Work_On_Flag)
		delay_sys_sec(Sys_Admin.Last_Blow_Time);//ִ�к�ɨ��ʱ	
	else
		delay_sys_sec(15000);//���û�ɹ����ʹ���?5��
}


/*�����ɨ������־��? ������������λ�����ʧ�ܹ��ϣ�ȼ������й¶���ϣ�ϵͳ�����л���Ϩ��?/

void Last_Blow_End_Fun(void)
{
	//ȷ�Ϸ���ر�?
	
			Send_Air_Close();

	sys_flag.tx_hurry_flag = 1;//�����������ݸ�������
	 
	 
	 
	sys_flag.last_blow_flag = 0;//��ɨ״̬������־
}

 




/*��ֹ�û��л����ֶ�����ҳ�棬��ʱ��û���˳��ֶ����ԣ�10���Ӻ��˳��ֶ�����*/



//���ü̵����źſ���������������������ˮλ�ź���,�����ˮλ�߼�����?
uint8  Water_Balance_Function(void)
{
	
	uint8 buffer = 0;
			
	
		
	lcd_data.Data_15H = 0;
	if (IO_Status.Target.water_protect== WATER_OK)
				buffer |= 0x01;
		else
				buffer &= 0x0E; 
	
		if (IO_Status.Target.water_mid== WATER_OK)
				buffer |= 0x02;
		else
				buffer &= 0x0D;
	
		
		if (IO_Status.Target.water_high== WATER_OK)
				buffer |= 0x04;
		else
				buffer &= 0x0B;
	
		if (IO_Status.Target.water_shigh== WATER_OK)
				buffer |= 0x08;
		else
				buffer &= 0x07;


//������й����У�����ˮλ��̽���ˮ������
		if(sys_data.Data_10H == 2)
			{
				//�����ˮλû���ź�?
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//��������У�����ˮλ��ʾ��׼������?
						if (IO_Status.Target.water_shigh== WATER_OK)
							{
								buffer &= 0x07;
							}
								
					}
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;

	//��ˮ��ʱ  �� ��ˮ��ʱ���ϴ���
	//��ˮ��ʱ�߼�

	
	if(sys_flag.Error_Code)//����ȱ����Ϻ�ˮλ�߼����󣬲����?
		{
			Feed_Main_Pump_OFF();	
			Second_Water_Valve_Close();
			 return 0;
		}

	 if(sys_data.Data_10H == SYS_MANUAL)   //�ֶ�ģʽ��ˮ����
	 		return 0;



	  if(sys_data.Data_10H == SYS_IDLE)
	 	{
	 		
	 		if(sys_flag.last_blow_flag)
	 			{
	 				/*2023��3��10��09:20:37 �ɳ����źţ��ĳ����źţ���ֹˮ����*/
	 				if( IO_Status.Target.water_mid == WATER_LOSE)
	 					sys_flag.Force_Supple_Water_Flag = OK;

					if( IO_Status.Target.water_mid == WATER_OK)
						sys_flag.Force_Supple_Water_Flag = FALSE;
					
	 			}
			else
				{ 
					//������ɨ������û������ˮλ��ˮ�û��ڹ���������
					
					sys_flag.Force_Supple_Water_Flag = FALSE;
					
					
				}
			if(sys_flag.Force_Supple_Water_Flag) //ǿ�Ʋ�ˮ��־����ǿ�ƴ򿪲�ˮ����
				{
					Feed_Main_Pump_ON();
					Second_Water_Valve_Open();
					 
				}
			 if(sys_flag.Force_Supple_Water_Flag == 0)
			 	{
			 		Feed_Main_Pump_OFF();
					Second_Water_Valve_Close();
			 	}

			return 0;
		
	 		
	 	}
			  

	 if(sys_flag.Force_Supple_Water_Flag) //ǿ�Ʋ�ˮ��־����ǿ�ƴ򿪲�ˮ����
		{
			Feed_Main_Pump_ON();
			Second_Water_Valve_Open();
			return 0;
		}
	
/**************************************************************/
	
	 
	//������ˮλ������ǲ��ܻ��ģ��������ɿ���?
	if(sys_flag.Error_Code == 0)
		{
	 		if(IO_Status.Target.water_mid == WATER_LOSE || IO_Status.Target.water_protect == WATER_LOSE)//��ˮλ�źŶ�ʧ�����벹ˮ
	 			{
						Feed_Main_Pump_ON();
						Second_Water_Valve_Open();
	 			}
	
			if(IO_Status.Target.water_high == WATER_OK && IO_Status.Target.water_mid == WATER_OK && IO_Status.Target.water_protect == WATER_OK )
				{
						Feed_Main_Pump_OFF();
						Second_Water_Valve_Close();
				}
				
		}
	else
		{
			Feed_Main_Pump_OFF();	
			Second_Water_Valve_Close();
		}
		

			
	return  0;	
}



//�����ֶ�ģʽһЩ���ܵĴ���
uint8 Manual_Realys_Function(void)
{
	
	
	
	
	//��ˮ��ʱ��ʾ����ֹˮ�������ٲ�
	
	
	return 0;
}

void Check_Config_Data_Function(void)
{
	float ResData = 0;
	
	//1�� ǰ��ɨ���?0--120s
	Sys_Admin.First_Blow_Time = *(uint32 *)(FIRST_BLOW_ADDRESS);  //Ԥ��ɨʱ��
	if(Sys_Admin.First_Blow_Time > 300000 ||Sys_Admin.First_Blow_Time < 30000) //��������趨��Χ����ֵ׷��?
		Sys_Admin.First_Blow_Time =30000 ;
	
	//2�� ��ɨ���?0--120s	
	Sys_Admin.Last_Blow_Time =  *(uint32 *)(LAST_BLOW_ADDRESS);//��ɨʱ��
	if(Sys_Admin.Last_Blow_Time > 300000 ||Sys_Admin.Last_Blow_Time < 30000) //��������趨��Χ����ֵ׷��?
		Sys_Admin.Last_Blow_Time =30000 ;
	
	//3�� �����?0--35%
	Sys_Admin.Dian_Huo_Power =  *(uint32 *)(DIAN_HUO_POWER_ADDRESS);  //�����?
	if(Sys_Admin.Dian_Huo_Power > Max_Dian_Huo_Power ||Sys_Admin.Dian_Huo_Power < Min_Dian_Huo_Power) //��������趨��Χ����ֵ׷��?
		Sys_Admin.Dian_Huo_Power =25 ;
	

	//4�� �������й��ʼ��?0--100%
	if(Sys_Admin.Max_Work_Power > 100 ||Sys_Admin.Max_Work_Power < 20)
		Sys_Admin.Max_Work_Power = 100;

	if(Sys_Admin.Max_Work_Power < Sys_Admin.Dian_Huo_Power)
		Sys_Admin.Max_Work_Power = Sys_Admin.Dian_Huo_Power;


	Sys_Admin.Fan_Speed_Check =  *(uint32 *)(FAN_SPEED_CHECK_ADDRESS);  //���ټ���Ƿ���?
	if(Sys_Admin.Fan_Speed_Check > 1)
		Sys_Admin.Fan_Speed_Check = 1; //Ĭ���Ǽ����ٵ�
	
		
	Sys_Admin.Danger_Smoke_Value =  *(uint32 *)(DANGER_SMOKE_VALUE_ADDRESS); //�������¶ȱ�������
	if(Sys_Admin.Danger_Smoke_Value > 2000 && Sys_Admin.Danger_Smoke_Value < 600)
		Sys_Admin.Danger_Smoke_Value = 800;
	
	

	

	sys_config_data.zhuan_huan_temperture_value = *(uint32 *)(ZHUAN_HUAN_TEMPERATURE);
	if(sys_config_data.zhuan_huan_temperture_value < 10|| sys_config_data.zhuan_huan_temperture_value >= Sys_Admin.DeviceMaxPressureSet)
		sys_config_data.zhuan_huan_temperture_value = 55; //������ޣ�Ĭ��?.5����

	if(sys_config_data.Auto_stop_pressure >= Sys_Admin.DeviceMaxPressureSet)
		sys_config_data.Auto_stop_pressure = Sys_Admin.DeviceMaxPressureSet - 5; //������ޣ�Ĭ����ȶѹ����0.05Mpa
	

	Sys_Admin.DeviceMaxPressureSet = *(uint32 *)(DEVICE_MAX_PRESSURE_SET_ADDRESS);
	if(Sys_Admin.DeviceMaxPressureSet > 250) //25�������Ҫ��������?
		Sys_Admin.DeviceMaxPressureSet = 80;

	


	switch (Sys_Admin.Device_Style)
		{
			case 0:
			case 1:
					if(sys_data.Data_10H == 0)
						LCD10D.DLCD.Start_Close_Flag  = 0 ;
					if(sys_data.Data_10H == 2)
						LCD10D.DLCD.Start_Close_Flag  = 1 ;

				   break;
			case 2:
			case 3:
					LCD10D.DLCD.Start_Close_Flag = UnionD.UnionStartFlag;
					if(UnionD.UnionStartFlag == 3)
						LCD10D.DLCD.Start_Close_Flag = 0;
					

					break;
		}

	LCD10D.DLCD.Zhu_WaterHigh = sys_flag.ChaYaWater_Value;

	 
	LCD10D.DLCD.Cong_WaterHigh = sys_flag.Cong_ChaYaWater_Value;

	LCD10D.DLCD.Input_Data = sys_flag.Inputs_State;

	LCD10D.DLCD.System_Lock = sys_config_data.Sys_Lock_Set ;
													
	LCD10D.DLCD.YunXu_Flag = SlaveG[1].Key_Power; 
	LCD10D.DLCD.Pump_State = Switch_Inf.Water_Valve_Flag ;

//	LCD10D.DLCD.Air_Power = 0;  //��PWM���ڹ����У��Զ��޸�

	LCD10D.DLCD.Paiwu_State = Switch_Inf.pai_wu_flag;

	LCD10D.DLCD.lianxuFa_State = Switch_Inf.LianXu_PaiWu_flag;


	LCD10D.DLCD.Flame_State = sys_flag.flame_state;

	LCD10D.DLCD.Air_Speed  = sys_flag.Fan_Rpm;  //���ת�����?
	LCD10D.DLCD.Air_Power = sys_data.Data_1FH ;

	
	LCD10D.DLCD.Target_Value = (float) sys_config_data.zhuan_huan_temperture_value / 100;
	LCD10D.DLCD.Stop_Value = (float) sys_config_data.Auto_stop_pressure / 100;
	LCD10D.DLCD.Start_Value = (float) sys_config_data.Auto_start_pressure / 100;
	LCD10D.DLCD.Max_Pressure = (float) Sys_Admin.DeviceMaxPressureSet / 100;
		
	LCD10D.DLCD.First_Blow_Time = 	Sys_Admin.First_Blow_Time / 1000;
	LCD10D.DLCD.Last_Blow_Time = 	Sys_Admin.Last_Blow_Time / 1000;
	LCD10D.DLCD.Dian_Huo_Power = 	Sys_Admin.Dian_Huo_Power ;
	LCD10D.DLCD.Max_Work_Power = 	Sys_Admin.Max_Work_Power ;
	LCD10D.DLCD.Danger_Smoke_Value = 	Sys_Admin.Danger_Smoke_Value / 10 ;

	LCD10D.DLCD.Fan_Speed_Check = Sys_Admin.Fan_Speed_Check ;
	LCD10D.DLCD.Fan_Fire_Value = Sys_Admin.Fan_Fire_Value ;
	LCD10D.DLCD.Fan_Pulse_Rpm = Sys_Admin.Fan_Pulse_Rpm ;

	LCD10D.DLCD.Error_Code = sys_flag.Error_Code ;
	LCD10D.DLCD.Paiwu_Flag = sys_flag.Paiwu_Flag ;  //���۱�־ͬ��

	LCD10D.DLCD.Air_State = Switch_Inf.air_on_flag ; 
	LCD10D.DLCD.lianxuFa_State = Switch_Inf.LianXu_PaiWu_flag;   //������������۷�״�?
	
	LCD10D.DLCD.Water_BianPin_Enabled  = Sys_Admin.Water_BianPin_Enabled ;
	LCD10D.DLCD.Water_Max_Percent  = Sys_Admin.Water_Max_Percent ;

	LCD10D.DLCD.YuRe_Enabled  = Sys_Admin.YuRe_Enabled ;
	LCD10D.DLCD.Inside_WenDu_ProtectValue  = Sys_Admin.Inside_WenDu_ProtectValue ;

	LCD10D.DLCD.LianXu_PaiWu_DelayTime  = Sys_Admin.LianXu_PaiWu_DelayTime ;
	LCD10D.DLCD.LianXu_PaiWu_OpenSecs  = Sys_Admin.LianXu_PaiWu_OpenSecs ;
	LCD10D.DLCD.ModBus_Address  = Sys_Admin.ModBus_Address ;

	
	LCD10D.DLCD.Balance_Big_Time  = Sys_Admin.Balance_Big_Time ;

	LCD10D.DLCD.Balance_Small_Time  = Sys_Admin.Balance_Small_Time ;
	

	 
	//LCD10D.DLCD.YunXu_Flag = 0;
	LCD10D.DLCD.System_Version  = Soft_Version ; //ϵͳ�汾��
	LCD10D.DLCD.Device_Style = Sys_Admin.Device_Style  ;  //�豸���͵�ѡ��
	
	ResData = Sys_Admin.DeviceMaxPressureSet;													
	LCD10D.DLCD.Max_Pressure = ResData / 100;  //�����ѹ������ʾ

	LCD10JZ[2].DLCD.YunXu_Flag = SlaveG[2].Key_Power;
	LCD10JZ[1].DLCD.YunXu_Flag = SlaveG[1].Key_Power;
	
}



void Fan_Speed_Check_Function(void)
{
	
	//Fan_Rpm = (1000/(2* fan_count)) / 3(ÿ������3ת) *60�� = 100000 / sys_flag.Fan_count


		 
		static uint8 Pulse = 3;    //���ַ��ÿ�?������
		 
		uint32 All_Fan_counts = 0;
			
		
			//G1G170   0.5T���?ÿת3�����壬  Ametek  0.5T���?ÿת2������
			//G3G250   1T����Ĳ���?ÿת3������
			//G3G315   2T����Ĳ���? ÿת 5������
			if(sys_flag.Rpm_1_Sec)
				{
					sys_flag.Rpm_1_Sec = FALSE;

			

					//����PB0�������жϣ�����
					if(Sys_Admin.Fan_Pulse_Rpm >=  10  || Sys_Admin.Fan_Pulse_Rpm == 0)
							Sys_Admin.Fan_Pulse_Rpm = 3; //�������������?

					if(sys_flag.Fan_count > 0 )
						{
							sys_flag.Fan_Rpm = sys_flag.Fan_count * 60 / Sys_Admin.Fan_Pulse_Rpm;
							sys_flag.Fan_count = 0;
							
						}
						  //��������/5��  *60	��60��ָ60�룬����5 ��3��ÿת5������
					else
						{
							sys_flag.Fan_count = 0;
							sys_flag.Fan_Rpm = 0;
						}
						
				
				}


	
}


/*���ھ����̹����������������е�ʱ��*/
uint8 Admin_Work_Time_Function(void)
{
	//�漰���ı�����Flash_Data.Admin_Work_Time��systmtime
	
	uint16 Now_Year = 0;
	uint16 Now_Month = 0;
	uint16 Now_Day = 0;

	uint16 Set_Year = 0;
	uint16 Set_Month = 0;
	uint16 Set_Day = 0;
	
	uint8 Set_Function = 0;  //�û����õ�����

	Set_Function = *(uint32 *)(ADMIN_WORK_DAY_ADDRESS); 

	//lcd_data.Data_40H = Set_Function>> 8;
	//lcd_data.Data_40L =Set_Function &0x00FF;//������ˢ�¸�LCD
	
	sys_flag.Lock_System = 0; //�����������?
	if(Set_Function == FALSE )
		return 0;

	Now_Year = systmtime.tm_year;
	Now_Month = systmtime.tm_mon;
	 Now_Day = systmtime.tm_mday;

	Set_Year= *(uint32 *)(ADMIN_SAVE_YEAR_ADDRESS); 
	Set_Month = *(uint32 *)(ADMIN_SAVE_MONTH_ADDRESS); 
	Set_Day =  *(uint32 *)(ADMIN_SAVE_DAY_ADDRESS); 

	if(Now_Year > Set_Year)
		{
			sys_flag.Lock_System = OK;
			return 0;
		}

	if(Now_Year == Set_Year)
		{
			if(Now_Month > Set_Month)
				{
					sys_flag.Lock_System = OK;
					return 0;
				}

			if(Now_Month == Set_Month)
				if(Now_Day >= Set_Day )
					sys_flag.Lock_System = OK;
			
		}
	
	
	return 0 ;
}









void HardWare_Protect_Relays_Function(void)
{
 	 
 }



uint8 Power_ON_Begin_Check_Function(void)
{

	
	
	

	return 0;
}

uint8 IDLE_Auto_Pai_Wu_Function(void)
{
	
	
	return 0;
}

uint8 Auto_Pai_Wu_Function(void)
{
	static uint8 OK_Pressure = 5;
	static uint8 PaiWu_Count = 0;
	uint8  Paiwu_Times = 3;  //4�ν�ѹ����
	//���������ѹ��С�ڰ빫��ʱ���Զ�����һ��?
    
	uint8  Time = 15;//����ѹ����ˮ30��

	uint8 	Ok_Value = 0;
	
	
		//1�� Ҫ��¯Ҫ���й���2���Զ����۹��ܣ�Ҫ����
		
		
				
				if(sys_flag.Paiwu_Flag)
					{
						switch (sys_flag.Pai_Wu_Idle_Index)
							{
								case 0:
										
									//	Pai_Wu_Door_Close();
										Pai_Wu_Door_Open();
									if(Temperature_Data.Pressure_Value > OK_Pressure)
										{
											delay_sys_sec(25000);
											
										}
									else
										{
											delay_sys_sec(35000); //��ѹ�������ʱ��?
										}
										sys_flag.Pai_Wu_Idle_Index = 2;
										

										break;
								
								case 2:  //��⼫��ˮλ�ж��Ƿ����
										if(sys_time_start == 0)
											{
												sys_time_up = 1;
											}
										else
											{
												
											}
										
										if ( IO_Status.Target.water_protect== WATER_LOSE ) 
											{
												sys_flag.Pai_Wu_Idle_Index = 3;
												delay_sys_sec(60000);//������ˮλ��ˮ��ʱ��
												 Pai_Wu_Door_Close();
											}

										
										if(sys_time_up)
											{
												sys_time_up = 0;
												sys_flag.Force_Supple_Water_Flag = FALSE;
												 Pai_Wu_Door_Close();
												 delay_sys_sec(60000); //������ˮλ��ˮ��ʱ��
												sys_flag.Pai_Wu_Idle_Index = 3;
											}
										else
											{
												
											}

										break;
								case 3:
										Pai_Wu_Door_Close();
										if(sys_time_start == 0)
											{
												sys_time_up = 1;
											}
										else
											{
												
											}
										if ( IO_Status.Target.water_mid== WATER_OK ) 
											{
												sys_flag.Pai_Wu_Idle_Index = 4;
											}

										if(sys_time_up)
											{
												sys_time_up = 0;
												sys_flag.Force_Supple_Water_Flag = FALSE;
												 Pai_Wu_Door_Close();
												sys_flag.Pai_Wu_Idle_Index = 4;
											}
										else
											{
												
											}

										break;
								
								case 4:
										Pai_Wu_Door_Close();
										sys_flag.Force_Supple_Water_Flag  = 0;
										sys_flag.Paiwu_Flag = FALSE;
										sys_flag.Pai_Wu_Idle_Index = 0;
										Ok_Value = OK;  //�����Զ����۳���
										break;
								
								default:
									sys_flag.Paiwu_Flag = FALSE;
									sys_flag.Pai_Wu_Idle_Index =0;
									Ok_Value = OK; 
										break;
							}
					}
				else
					{
						sys_flag.Pai_Wu_Idle_Index = 0;
						sys_flag.Force_Supple_Water_Flag = FALSE;
						Ok_Value = OK; 
						Pai_Wu_Door_Close();

					}
			
		
		
	return Ok_Value;
}



uint8 YunXingZhong_TimeAdjustable_PaiWu_Function(void)
{
	//�豸���й�����ʹ�øù���
	uint8  set_flag = 0;
	
		


	return set_flag;
}


uint8 PaiWu_Warnning_Function(void)
{
	//���ۼ�ʱ����2E       2F ,30��
	static uint16 Max_Time = 480 ;  //���ʱ���?Сʱ
	static uint16 Max_Value = 1439; //�����ʾ��ʱ���?3:59
	static uint8 Low_Flag = 0;

	if(sys_data.Data_10H == SYS_WORK)
		{
			if(sys_flag.Paiwu_Secs >= 60)
				{
					sys_flag.Paiwu_Secs = 0;
					sys_time_inf.UnPaiwuMinutes ++;
					 
				}
				
		}
	else
		{
			sys_flag.Paiwu_Secs = 0;
		}
		
	if(sys_time_inf.UnPaiwuMinutes > Max_Value)
		sys_time_inf.UnPaiwuMinutes = Max_Value;

	if(sys_time_inf.UnPaiwuMinutes > Max_Time)
		{
			lcd_data.Data_2EH = 0;
			lcd_data.Data_2EL = OK; //������ʾͼ����?
			//sys_flag.Paiwu_Alarm_Flag  = OK;
		}
	else
		{
			lcd_data.Data_2EH = 0;
			lcd_data.Data_2EL = 0; //������ʾͼ����?
			//sys_flag.Paiwu_Alarm_Flag  = FALSE;
		}

	
	if(Low_Flag == 0)
		sys_flag.Low_Count = 0;
	if(sys_time_inf.UnPaiwuMinutes > 1) //δ����ʱ�䳬��10���ӣ�ˮλ��ʧ�󣬻������ʱ������?
		{
			if (IO_Status.Target.water_protect == WATER_LOSE)
				{
					Low_Flag = OK;
					if(sys_flag.Low_Count >= 3)//�����ˮλ����?0����������
						{
							Low_Flag = 0;
							sys_time_inf.UnPaiwuMinutes = 0;
							Write_Second_Flash();
						}
				}
					
		}

	lcd_data.Data_2FH = 0;
	lcd_data.Data_2FL = sys_time_inf.UnPaiwuMinutes / 60; //δ����ʱ�䣺 Сʱ��ʾ
	lcd_data.Data_30H = 0;
	lcd_data.Data_30L = sys_time_inf.UnPaiwuMinutes % 60; //δ����ʱ�䣺 ������ʾ

	
	return 0;
}


uint8 Special_Water_Supply_Function(void)
{
	static uint8 High_Flag = 0;
	//���½�ˮ��ŷ�?���漰�����»�ˮ��ŷ�?
	 

	if(Sys_Admin.Special_Secs > 50)
 //���ʱ��������?
		Sys_Admin.Special_Secs = 20;
	
	if(sys_flag.Error_Code)
		Special_Water_OFF();

	if(sys_data.Data_10H == 0 || sys_data.Data_12H) //������ѹͣ¯ģʽ
		Special_Water_OFF();


	if (IO_Status.Target.water_high== WATER_OK) //�ﵽ��ˮλ����رո��»�ˮ��?
		{
			Special_Water_OFF();
			sys_flag.High_Lose_Flag = 0;
			sys_flag.High_Lose_Count = 0;
		}

	
	if (IO_Status.Target.water_high== WATER_LOSE) 
		{
			if(sys_flag.High_Lose_Flag == 0)
				{
					sys_flag.High_Lose_Flag = OK;
					sys_flag.High_Lose_Count = 0;
				}

			if(sys_flag.High_Lose_Count >= Sys_Admin.Special_Secs) //18��
				{
					sys_flag.High_Lose_Count = Sys_Admin.Special_Secs; //��ס
					Special_Water_Open(); //�򿪸��»�ˮ��ŷ�?
				}
		}



	return 0 ;
}



//��ʱ�����ó���Ͷ��ʹ��
uint8 WaterLevel_Unchange_Check(void)
{
	static uint8 LastState = 0;
	uint8  Water_Buffer = 0;

	//ֻ������״̬���м��?
	if(Sys_Admin.WaterUnchangeMaxTime >= 250) //Ĭ�������رռ��?
		return 0; 

	//�����ܣ������й����У� ���ˮλ����ʱ�䲻�仯���򱨾�?
	Water_Buffer = lcd_data.Data_15L & 0x07; //ȡ�������ˮλ״̬�ļ���?


	if(LastState != Water_Buffer)
		{
			LastState = Water_Buffer;
			//ȡ��ʱ��
			//��Ҫ���״ν������г���ǰ����
			sys_flag.WaterUnsupply_Count = 0; //���¼�ʱ
		}


	if(sys_flag.WaterUnsupply_Count >= Sys_Admin.WaterUnchangeMaxTime)
		{
			//��ʱ��δ���в�ˮ���� ��Ĭ���� 150��
			//sys_flag.Error_Code  = Error19_NotSupplyWater;
		}

	return 0;
}


uint8  Water_BianPin_Function(void)
{
	
	uint8 buffer = 0;
	static uint8 Old_State = 0;
	static uint8 New_Percent = 18;
	uint8 Max_Percent = 0; 
	uint8 Min_Percent = 18;
	uint8 Jump_Index = 0;

	if(Sys_Admin.Water_Max_Percent > 99)
		Sys_Admin.Water_Max_Percent = 99; //��󿪶�ֵ���ܳ���?00

	if(Sys_Admin.Water_Max_Percent < 25)
		Sys_Admin.Water_Max_Percent = 25; //������С����ֵ���ܵ���25
		
	Max_Percent = Sys_Admin.Water_Max_Percent;
	
		
	lcd_data.Data_15H = 0;
	if (IO_Status.Target.water_protect== WATER_OK)
				buffer |= 0x01;
		else
				buffer &= 0x0E; 
	
		if (IO_Status.Target.water_mid== WATER_OK)
				buffer |= 0x02;
		else
				buffer &= 0x0D;
	
		
		if (IO_Status.Target.water_high== WATER_OK)
				buffer |= 0x04;
		else
				buffer &= 0x0B;
	
		if (IO_Status.Target.water_shigh== WATER_OK)
				buffer |= 0x08;
		else
				buffer &= 0x07;


//������й����У�����ˮλ��̽���ˮ������
		if(sys_data.Data_10H == 2)
			{
				//�����ˮλû���ź�?
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//��������У�����ˮλ��ʾ��׼������?
						if (IO_Status.Target.water_shigh== WATER_OK)
								buffer &= 0x07;
					}
			}
		else
			{
			
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;

	//��ˮ��ʱ  �� ��ˮ��ʱ���ϴ���
	//��ˮ��ʱ�߼�
	if(sys_flag.Water_Percent > 0)
		{
			Feed_Main_Pump_ON();
			Second_Water_Valve_Open();
		}
		
		
	
	if(sys_flag.Water_Percent == 0)
		{
			Feed_Main_Pump_OFF();
			Second_Water_Valve_Close();
		}


	//���� �� �ֶ�ģʽ�£������в�ˮ����
	 if(sys_data.Data_10H == SYS_MANUAL )  
	 	  return 0;
	
	
	

	

	 if(sys_flag.Error_Code)//����ȱ����Ϻ�ˮλ�߼����󣬲����?
		{
			sys_flag.Water_Percent = 0;
			 return 0;
		}

	
	 	

	
//ȡ��ǿ�Ʋ�ˮ�Ĵ�ʩ
	
	 if(sys_flag.Force_Supple_Water_Flag) //ǿ�Ʋ�ˮ��־����ǿ�ƴ򿪲�ˮ����
		{
			
			sys_flag.Water_Percent = Max_Percent;
			
			return 0;
		}


		if(sys_data.Data_10H == 0 )
			{
				sys_flag.Water_Percent = 0;

				return 0;
			}

	 
		
	
/**************************************************************/
	
	if(IO_Status.Target.water_protect == WATER_OK && IO_Status.Target.water_mid == WATER_LOSE)
		Jump_Index = 1;
	if( IO_Status.Target.water_mid == WATER_OK && IO_Status.Target.water_high == WATER_LOSE)
		Jump_Index = 2;
	if( IO_Status.Target.water_mid == WATER_OK && IO_Status.Target.water_high == WATER_OK)
		Jump_Index = 3;
	if(IO_Status.Target.water_protect == WATER_LOSE)
		Jump_Index = 0;


	

	switch (Jump_Index)
		{
		case 0://û��ˮʱ
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 0;

				break;
		case 1://��ˮλʱ
				if(Old_State == 2)
					New_Percent++;
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 1;

				break;

		case 2://��ˮλʱ
				if(New_Percent < Min_Percent)
					New_Percent = Min_Percent;

				if(New_Percent > Max_Percent)
					New_Percent = Max_Percent;
					
				sys_flag.Water_Percent = New_Percent;

				Old_State = 2;

				break;

		case 3://��ˮλʱ
				if(Old_State == 2)
					New_Percent--;
				if(New_Percent < Min_Percent)
					New_Percent = Min_Percent;
				Old_State = 3;
				sys_flag.Water_Percent = 0;

				break;

		default:
				Jump_Index = 0;
				sys_flag.Water_Percent = 0;

				break;
		}
	
	 
	

			
	return  0;	
}


uint8 LianXu_Paiwu_Control_Function(void)
{
	uint32 Dealy_Time = 0;
	uint16 Open_Time = 0; //�������۷���ʵ�ʿ���ʱ���趨����ȷ��0.1s

	uint16 Cong_Work_Time = 0;
	static uint8 Time_Ok = 0;  //����ʱ�䵽�ı�־����̬����
	
	//�������ۿ�����־����������ʱ�������������ۿ���ʱ����

	//������4����ѹ���£�����2�룬��ˮ����1L

	//Sys_Admin.LianXu_PaiWu_Enabled 
	//Sys_Admin.LianXu_PaiWu_DelayTime //���ȵ�0.1Сʱ
	//Sys_Admin.LianXu_PaiWu_OpenSecs //���ȵ�1s

	//ADouble5[1].True.LianXuTime_H���ӻ���ǰ�Ѿ�������ʱ��
	//************��Ҫ�������ӻ�ͬʱ���ۣ���ô���������������ӣ��ӻ�����ԭ��ʱ���趨�������ӳ������ӣ�Ҫ��Ҫ������ۣ�?
	//��Ҫ�Ѵӻ�����������ʱ�䣬ͬ����������

	//������Ҫ��ˮ�ò�ˮ�������ܴ�

	//sys_flag.LianXu_1sFlag
	Dealy_Time = Sys_Admin.LianXu_PaiWu_DelayTime * 1 * 60; //0.1h * min  * 60sec/min
	

	Open_Time = Sys_Admin.LianXu_PaiWu_OpenSecs * 10; //�����?00ms��λ�����㾫׼����ʱ��

	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
		{
			//�����飬�ü̵����������й�?
			return 0 ;
		}
	
	if(sys_data.Data_10H == 3)
		return 0;
	

	//����״̬���л���ı�־���ŶԹ�����ʱ�����ͳ��
	if(sys_data.Data_10H == 2)
		{
			if(sys_flag.flame_state)
				if(sys_flag.LianXu_1sFlag)
					{
						sys_flag.LianxuWorkTime ++;//���?
						sys_flag.LianXu_1sFlag = 0;
					}
		}


	 

	//��鹤���ĵ�ʱ�䣬��û�дﵽ�趨���?
	if(sys_flag.LianxuWorkTime >= Dealy_Time)
		{
			sys_flag.LianxuWorkTime = 0; //��������
			sys_flag.Lianxu_OpenTime  = 0;
		
			Time_Ok = OK;//�����������۱�־
		}

	//������ʱ�䵽���Ҵ��ڲ�ˮ״̬������������۷�����鷧�ſ�����ʱ��
	if(Time_Ok)
		{
			
			if(sys_flag.Lianxu_OpenTime < Open_Time)
				{
					 if( Switch_Inf.water_switch_flag)//  ����Ƶ��ˮ�������Ǹ�ˮ���������������ź�����
					 	{
					 		LianXu_Paiwu_Open();
							if(sys_flag.LianXu_100msFlag)
								{
									sys_flag.LianXu_100msFlag = 0;
									sys_flag.Lianxu_OpenTime++;
								}
							
					 	}
					 else
					 	LianXu_Paiwu_Close();
				}
			else
				{
					Time_Ok = FALSE; //ʱ�䵽�ı�־����
					
				}
			
		}
	else
		{
			sys_flag.Lianxu_OpenTime  = 0; //����ϴ�ʹ�õı������?
			LianXu_Paiwu_Close();
		}
	
	

	return 0;
}



uint8 Auto_StartOrClose_Process_Function(void)
{
	
	

	return 0;
}


void JTAG_Diable(void)
{
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO ,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
}




uint8 Speed_Pressure_Function(void)
{
	static uint16 Old_Pressure = 0; //���ڱ����ϸ��׶ε�����ֵ
	uint16 New_Pressure =0;
	static uint16 TimeCount = 0;
	uint8 Chazhi = 0;

	//
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) 
		{
			//������ʹ���ڲ�ѹ����Ϊ׷��Ŀ��
			New_Pressure = Temperature_Data.Inside_High_Pressure;  //����һ�β��ѹ����ΪĿ��?
		}
	else
		{
			New_Pressure = Temperature_Data.Pressure_Value;   //���β�ѹ����Ϊ׷��Ŀ��
		}

	
	
	if(sys_flag.Pressure_1sFlag)
		{
			sys_flag.Pressure_1sFlag = 0;
			
			if(sys_flag.flame_state)
				{
					TimeCount ++;
					if(New_Pressure > Old_Pressure)
						{
							Chazhi = New_Pressure - Old_Pressure;
							Old_Pressure = New_Pressure;
							sys_flag.Pressure_ChangeTime = TimeCount;
							sys_flag.Pressure_ChangeTime = sys_flag.Pressure_ChangeTime / Chazhi;
							TimeCount = 0;
						}


					if(New_Pressure < Old_Pressure)
						{
							Chazhi = Old_Pressure - New_Pressure;
							
							Old_Pressure = New_Pressure;
							sys_flag.Pressure_ChangeTime = TimeCount;
							sys_flag.Pressure_ChangeTime = sys_flag.Pressure_ChangeTime / Chazhi;
							
							TimeCount = 0;
						}
				}
			else   //û�л���ʱ��״̬����
				{
					Old_Pressure = New_Pressure;
					TimeCount = 0;
					sys_flag.Pressure_ChangeTime = 0;
				}
		}

	

		return 0;
}

uint8 Wifi_Lock_Time_Function(void)
{
	//�漰���ı�����Flash_Data.Admin_Work_Time��systmtime
	Date Now;
	Date Set;

	

	Now.iYear = LCD10D.DLCD.Year;
	Now.iMonth = LCD10D.DLCD.Month;
	Now.iDay = LCD10D.DLCD.Day;     //�������Ļ��ʱ��?

	Set.iYear= *(uint32 *)(WIFI_LOCK_YEAR_ADDRESS); 
	Set.iMonth = *(uint32 *)(WIFI_LOCK_MONTH_ADDRESS); 
	Set.iDay =  *(uint32 *)(WIFI_LOCK_DAY_ADDRESS); 

	if(Set.iYear == 0 || Set.iMonth == 0 || Set.iDay == 0)
		{
			return 0;
		}

	if(Now.iYear > Set.iYear)
		{
			
			return OK;
		}

	if(Now.iYear == Set.iYear)
		{
			if(Now.iMonth > Set.iMonth)
				{
					
					return OK;
				}

			if(Now.iMonth == Set.iMonth)
				if(Now.iDay >=Set.iDay )
					{
						return OK;
					}
					
		}
	
	
	
	
	return 0 ;
}

uint8 XiangBian_Steam_AddFunction(void)
{

	uint16 Protect_Pressure = 150;  //1.5Mpa
	
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)  //��������������
		{
			if(sys_data.Data_10H == 2)
				{
					if(Temperature_Data.Inside_High_Pressure >=Protect_Pressure) //����15���ֱ�ӱ���
						{
							if(sys_flag.Error_Code == 0 )
								sys_flag.Error_Code  = Error20_XB_HighPressureYabian_Bad;
						}

					
				}


			switch (sys_data.Data_10H)
				{
					case 0:  //����״̬,�����ּ���ˮλ������������
							if(IO_Status.Target.XB_WaterLow == FALSE)
								{
									if(sys_flag.XB_WaterLow_Flag == 0)
										{
											sys_flag.XB_WaterLow_Flag = OK;
											sys_flag.XB_WaterLow_Count = 0;
										}

									if(sys_flag.XB_WaterLow_Count > 15)
										{
											//sys_flag.Error_Code = Error22_XB_HighPressureWater_Low; 
										}
								}
							else
								{
									sys_flag.XB_WaterLow_Flag = 0;
									sys_flag.XB_WaterLow_Count = 0;
								}
							
							break;
					case 2://����״̬
							if(sys_flag.flame_state == OK)
								{
									//���ּ���ˮλ�����ұ����¶ȳ���230�ȣ���ͣ��ת��ɨ������4�κ��򱨾�
									if(IO_Status.Target.XB_WaterLow == FALSE && sys_flag.Protect_WenDu >= 200)
										{
											if(sys_flag.XB_WaterLow_Flag == 0)
												{
													sys_flag.XB_WaterLow_Flag = OK;
													sys_flag.XB_WaterLow_Count = 0;
												}

											if(sys_flag.XB_WaterLow_Count > 10)  //��ˮλ����15��
												{
													if(sys_data.Data_12H == 0)
														{
															sys_flag.XB_WaterLowAB_Count ++;
														}
													
													if(sys_flag.XB_WaterLowAB_Count >= 4)
														{
															sys_flag.Error_Code = Error22_XB_HighPressureWater_Low; 
														}
													else
														{
															//ת���쳣״̬
															sys_data.Data_12H = 5; //  
															Abnormal_Events.target_complete_event = 1;//�쳣�¼���¼
														}
												}
											
										}
									else
										{
											//���¶Ȼ�ˮλ�κ�һ����㣬����0
												sys_flag.XB_WaterLow_Flag = 0;
												sys_flag.XB_WaterLow_Count = 0;

												if(sys_flag.XB_WaterLowAB_Count)
													{
														//�������ȼ�հ�Сʱ���Զ���Ϩ���¼����
														if(sys_flag.XB_WaterLowAB_RecoverTime >= 1800)//30min�������У�����Ϊ��������
															sys_flag.XB_WaterLowAB_Count = 0;
													}
										}
								}
							else
								{
									//�豸������״̬����ֹ�մ������쳣��ˮλ��û�ȶ����������������?
									//�豸��ǰ��ɨ�����У���⵽ȱˮ��Ҳ��ֱ�ӱ���?
								
									if(sys_data.Data_12H == 0)
										{
											//���쳣״̬����ֱ�ӱ���
											if(IO_Status.Target.XB_WaterLow == FALSE)
												{
													if(sys_flag.XB_WaterLow_Flag == 0)
														{
															sys_flag.XB_WaterLow_Flag = OK;
															sys_flag.XB_WaterLow_Count = 0;
														}

													if(sys_flag.XB_WaterLow_Count > 10) //�ȴ�10��
														{
															sys_flag.Error_Code = Error22_XB_HighPressureWater_Low; 
														}
													
												}
											else
												{
													sys_flag.XB_WaterLow_Flag = 0;
													sys_flag.XB_WaterLow_Count = 0;
												}

											
										}
									else
										{
											//��ѹͣ¯״̬��ֱ�Ӳ����ˮλ������־����?
											sys_flag.XB_WaterLow_Flag = 0;
											sys_flag.XB_WaterLow_Count = 0;	
										}
								}
							break;
					default:

							break;
				}

			

			

			if(IO_Status.Target.XB_Hpress_Ykong == PRESSURE_ERROR)
				{
					 if(sys_flag.Error_Code == 0 )
						sys_flag.Error_Code = Error21_XB_HighPressureYAKONG_Bad; //����ѹ��������ȫ��Χ����	
				}

			if(Temperature_Data.Inside_High_Pressure >= 2)//0.02Mpa
	 			{
	 				LianXu_Paiwu_Close();
	 			}
		}
		 


	return 0;	
}


uint8 GetOut_Mannual_Function(void)
{
	Feed_Main_Pump_OFF();
	sys_flag.WaterClose_Time = 2; 
	Second_Water_Valve_Close();
	Pai_Wu_Door_Close();
	Send_Air_Close();
	LianXu_Paiwu_Close();
	WTS_Gas_One_Close();



		return 0;
}



uint8  ShuangPin_Water_Balance_Function(void)
{
	
	uint8 buffer = 0;

	lcd_data.Data_15H = 0;
	if (IO_Status.Target.water_protect== WATER_OK)
				buffer |= 0x01;
		else
				buffer &= 0x0E; 
	
		if (IO_Status.Target.water_mid== WATER_OK)
				buffer |= 0x02;
		else
				buffer &= 0x0D;
	
		
		if (IO_Status.Target.water_high== WATER_OK)
				buffer |= 0x04;
		else
				buffer &= 0x0B;
	
		if (IO_Status.Target.water_shigh== WATER_OK)
				buffer |= 0x08;
		else
				buffer &= 0x07;


//������й����У�����ˮλ��̽���ˮ������
		//������й����У�����ˮλ��̽���ˮ������
		if(sys_data.Data_10H == 2)
			{
				//�����ˮλû���ź�?
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//��������У�����ˮλ��ʾ��׼������?
						if (IO_Status.Target.water_shigh== WATER_OK)
								buffer &= 0x07;
					}
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;
	
	

	//���� �� �ֶ�ģʽ�£������в�ˮ����
	 if(sys_data.Data_10H == SYS_MANUAL)  
	 		return 0;


	//������״̬�¼���ˮ��ŷ��Ŀ�����ر�
	
	if(sys_flag.Address_Number == 0)
		{
			//���������Լ�����ˮ��ŷ����ӻ����ڴ������յ�����?
			if(Water_State.ZCommand)
				Second_Water_Valve_Open();
			else
				Second_Water_Valve_Close();
		}
	
	 

	if(sys_flag.Error_Code )//����ȱ����Ϻ�ˮλ�߼����󣬲����?
		{
			Water_State.ZSignal = FALSE;
			 return 0;
		}

	 //���������в�ˮ������**************************

	
	 if(sys_data.Data_10H == SYS_IDLE)
	 	{
	 		
	 		if(sys_flag.last_blow_flag)
	 			{
	 				/*2023��3��10��09:20:37 �ɳ����źţ��ĳ����źţ���ֹˮ����*/
	 				if( IO_Status.Target.water_mid == WATER_LOSE)
	 					sys_flag.Force_Supple_Water_Flag = OK;

					if( IO_Status.Target.water_mid == WATER_OK)
						sys_flag.Force_Supple_Water_Flag = FALSE;
					
	 			}
			else
				{
					Water_State.ZSignal = FALSE;
					sys_flag.Force_Supple_Water_Flag = FALSE;
					return 0;
				}
		
	 		
	 	}
	 	
				
	
//ȡ��ǿ�Ʋ�ˮ�Ĵ�ʩ
	
	 if(sys_flag.Force_Supple_Water_Flag) //ǿ�Ʋ�ˮ��־����ǿ�ƴ򿪲�ˮ����
		{
			
			Water_State.ZSignal = OK;
			
			return 0;
		}
		
	
/**************************************************************/


	if(sys_flag.Error_Code == 0)
		{
	 		if(IO_Status.Target.water_mid == WATER_LOSE && IO_Status.Target.water_high == WATER_LOSE)//��ˮλ�źź͸�ˮλ�źŶ�ʧ�����벹ˮ
	 			{
						Water_State.ZSignal = OK;
	 			}
	
			if(IO_Status.Target.water_high == WATER_OK)
				{
						Water_State.ZSignal = FALSE;
				}
				
		}
	else
		{
			Water_State.ZSignal = FALSE;
		}
		
	
			
	return  0;	
}

uint8 Double_WaterPump_LogicFunction(void)
{
	uint8 State_Index = 0;
	static uint16 Time_Value = 900 ; //����700ms����


	 if(sys_data.Data_10H == SYS_MANUAL)
	 	{
			if(Water_State.Zstate_Flag || Water_State.Cstate_Flag)
				Logic_Pump_On();
			else
				Logic_Pump_OFF();

			return 0;
	 	}

	
	//�Կ���ص��ź�״̬���м���?
	if(Water_State.ZSignal == FALSE && Water_State.CSignal == FALSE)
		State_Index = 0;
	if(Water_State.ZSignal == OK && Water_State.CSignal == FALSE)
		State_Index = 1;
	if(Water_State.ZSignal == FALSE && Water_State.CSignal == OK)
		State_Index = 2;
	if(Water_State.ZSignal == OK && Water_State.CSignal == OK)
		State_Index = 3;


	switch (State_Index)
		{
			case 0: //���Ӷ��ǹ������ź�
					//���ȵü��ˮ���Ƿ�Ϊ��״�?
					if(Switch_Inf.water_switch_flag)
						{
							//����ˮ�ùرյĶ����ź�
							Water_State.Pump_Signal = FALSE;
							Water_State.PUMP_Close_Time = 0;
						}
					Water_State.Pump_Signal = FALSE;

					//ˮ�ùر�ʱ�䵽���������ӿ��ص�״̬�ر����ӵĽ�ˮ��ŷ�?
					if(Water_State.PUMP_Close_Time >= Time_Value)  //���պ������?
						{
							Water_State.ZCommand = FALSE; //�������ӵ�ŷ��ر�ָ��?
							Water_State.CCommand = FALSE;
						}

					break;
			case 1: //���������źţ��ӹر������ź�
					//���޴����ر������źţ�Ȼ���ٴ������ź�
					if(Water_State.Cstate_Flag == OK)
						{
							//���йص������źţ��Ҵӵĵ�ŷ����ڿ���״̬�?�ȹر�ˮ��
							if(Switch_Inf.water_switch_flag)
								Water_State.PUMP_Close_Time = 0;//�����ʱ��?
							Water_State.Pump_Signal = FALSE;
							if(Water_State.PUMP_Close_Time >= Time_Value)
								{
									Water_State.CCommand = FALSE;
									Water_State.ZC_Open_Time = 0; //�Ե�ŷ�������ʱ�����¼��㣬��ֹ˲��˲��?
								}
								
						}
					else
						{
							//�ӻ��Ѿ��رգ������������ź�
							if(Water_State.Zstate_Flag == OK)
								{
									//����ŷ��Ѿ�����?
									if(Water_State.ZC_Open_Time >= Time_Value)
										Water_State.Pump_Signal = OK;//����ˮ���ź�
								}
							else
								{
									//��ǰ�����ĵ�ŷ���û�д򿪣����
									Water_State.ZCommand = OK;
									Water_State.ZC_Open_Time = 0;
								}
						}

					break;
			case 2: //�ӻ��п����źţ������ǹر��ź�
					if(Water_State.Zstate_Flag == OK)
						{
							//���йص������źţ������ĵ�ŷ����ڿ���״̬�?�ȹر�ˮ��
							if(Switch_Inf.water_switch_flag)
								Water_State.PUMP_Close_Time = 0;//�����ʱ��?
							Water_State.Pump_Signal = FALSE;
							if(Water_State.PUMP_Close_Time >= Time_Value)
								{
									Water_State.ZCommand = FALSE;
									Water_State.ZC_Open_Time = 0; //�Ե�ŷ�������ʱ�����¼��㣬��ֹ˲��˲��?
								}
								
						}
					else
						{
							//�����Ѿ��رգ��ӻ��������ź�
							if(Water_State.Cstate_Flag == OK)
								{
									//����ŷ��Ѿ�����?
									if(Water_State.ZC_Open_Time >= Time_Value)
										Water_State.Pump_Signal = OK;//����ˮ���ź�
								}
							else
								{
									//��ǰ�����ĵ�ŷ���û�д򿪣����
									Water_State.CCommand = OK;
									Water_State.ZC_Open_Time = 0;
								}
						}

					break;
			case 3: //�����ʹӻ��������󿪵��ź�
					//��Ҫȷ��ˮ���Ƿ��Ѿ���
					Water_State.ZCommand = OK; //�������ӵ�ŷ�����ָ��?
					Water_State.CCommand = OK;

					if(Water_State.ZC_Open_Time >= Time_Value)
							Water_State.Pump_Signal = OK;//����ˮ���ź�
				
					break;

			default:

					break;
		}


	//�������?
	if(sys_flag.Address_Number == 0)
		{
			
			if(Water_State.Pump_Signal)
				{
					Logic_Pump_On();
				}
			else
				{
					Logic_Pump_OFF();
				}

		}

	

		if(Water_State.CCommand)
			{
				//���ӻ�������򿪲�ˮ��ŷ�
				if(Water_State.Cstate_Flag == 0)
					{
						Water_State.Cstate_Flag = OK;
						SlaveG[2].PumpOpen_Flag = 3;
						SlaveG[2].PumpOpen_Data = OK;
					}
			}
		else
			{
				if(Water_State.Cstate_Flag == OK)
					{
						SlaveG[2].PumpOpen_Flag = 3;
						SlaveG[2].PumpOpen_Data = FALSE;
						Water_State.Cstate_Flag = FALSE;
					}
			}



	return 0;
}


uint8  Double_Water_BianPin_Function(void)
{
	
	uint8 buffer = 0;
	static uint8 Old_State = 0;
	static uint8 New_Percent = 18;
	uint8 Max_Percent = 40;  //2023��3��5��10:37:32  ��32����Ϊ 40
	uint8 Min_Percent = 18;
	uint8 Jump_Index = 0;

	if(Sys_Admin.Water_Max_Percent > 99)
		Sys_Admin.Water_Max_Percent = 99; //��󿪶�ֵ���ܳ���?00

	if(Sys_Admin.Water_Max_Percent < 25)
		Sys_Admin.Water_Max_Percent = 25; //������С����ֵ���ܵ���25
		
	Max_Percent = Sys_Admin.Water_Max_Percent;
	
		
	lcd_data.Data_15H = 0;
	if (IO_Status.Target.water_protect== WATER_OK)
				buffer |= 0x01;
		else
				buffer &= 0x0E; 
	
		if (IO_Status.Target.water_mid== WATER_OK)
				buffer |= 0x02;
		else
				buffer &= 0x0D;
	
		
		if (IO_Status.Target.water_high== WATER_OK)
				buffer |= 0x04;
		else
				buffer &= 0x0B;
	
		if (IO_Status.Target.water_shigh== WATER_OK)
				buffer |= 0x08;
		else
				buffer &= 0x07;


//������й����У�����ˮλ��̽���ˮ������
		if(sys_data.Data_10H == 2)
			{
				//�����ˮλû���ź�?
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//��������У�����ˮλ��ʾ��׼������?
						if (IO_Status.Target.water_shigh== WATER_OK)
								buffer &= 0x07;
					}
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;

	//��ˮ��ʱ  �� ��ˮ��ʱ���ϴ���
	//��ˮ��ʱ�߼�
	if(sys_flag.Water_Percent > 0)
		{
			Water_State.ZSignal = OK;
			sys_flag.WaterOut_Time = 3;
			Feed_Main_Pump_ON();
			Second_Water_Valve_Open();
		}
		
	
	if(sys_flag.Water_Percent == 0)
		{
			Water_State.ZSignal = FALSE;
			Feed_Main_Pump_OFF();
			Second_Water_Valve_Close();
		}


	//���� �� �ֶ�ģʽ�£������в�ˮ����
	 if(sys_data.Data_10H == SYS_MANUAL )  
	 		return 0;
	
	//������״̬�¼���ˮ��ŷ��Ŀ�����ر�
	if(sys_flag.Address_Number == 0)
		{
			//if(Water_State.ZCommand)
			//	Second_Water_Valve_Open();
			//else
			//	Second_Water_Valve_Close();
		}
	
	

	

	 if(sys_flag.Error_Code)//����ȱ����Ϻ�ˮλ�߼����󣬲����?
		{
			Water_State.ZSignal = FALSE;
			sys_flag.Water_Percent = 0;
			 return 0;
		}

	 if(sys_data.Data_10H == SYS_IDLE)
	 	{
	 		//�ں�ɨ�����л���Ҫ��ˮ
	 		if(sys_flag.last_blow_flag)
	 			{
	 				if( IO_Status.Target.water_mid == WATER_LOSE)
	 					sys_flag.Water_Percent = 30;

					if( IO_Status.Target.water_mid == WATER_OK)
						sys_flag.Water_Percent = 0;
					
	 			}
			else
				{
					Water_State.ZSignal = FALSE;
					sys_flag.Water_Percent = 0;
				}
				
				
	 		return 0;
	 	}
	 	

	
//ȡ��ǿ�Ʋ�ˮ�Ĵ�ʩ
	
	 if(sys_flag.Force_Supple_Water_Flag) //ǿ�Ʋ�ˮ��־����ǿ�ƴ򿪲�ˮ����
		{
			Water_State.ZSignal = OK;
			sys_flag.Water_Percent = Max_Percent;
			
			return 0;
		}
		
	
/**************************************************************/
	
	if(IO_Status.Target.water_protect == WATER_OK && IO_Status.Target.water_mid == WATER_LOSE)
		Jump_Index = 1;
	if( IO_Status.Target.water_mid == WATER_OK && IO_Status.Target.water_high == WATER_LOSE)
		Jump_Index = 2;
	if( IO_Status.Target.water_mid == WATER_OK && IO_Status.Target.water_high == WATER_OK)
		Jump_Index = 3;
	if(IO_Status.Target.water_protect == WATER_LOSE)
		Jump_Index = 0;


	

	switch (Jump_Index)
		{
		case 0://û��ˮʱ
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 0;

				break;
		case 1://��ˮλʱ
				if(Old_State == 2)
					New_Percent++;
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 1;

				break;

		case 2://��ˮλʱ
				if(New_Percent < Min_Percent)
					New_Percent = Min_Percent;

				if(New_Percent > Max_Percent)
					New_Percent = Max_Percent;
					
				sys_flag.Water_Percent = New_Percent;

				Old_State = 2;

				break;

		case 3://��ˮλʱ
				if(Old_State == 2)
					New_Percent--;
				if(New_Percent < Min_Percent)
					New_Percent = Min_Percent;
				Old_State = 3;
				sys_flag.Water_Percent = 0;

				break;

		default:
				Jump_Index = 0;
				sys_flag.Water_Percent = 0;

				break;
		}
	
	 
	

			
	return  0;	
}



