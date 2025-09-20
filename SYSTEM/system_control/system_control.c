
#include "main.h"



uint32	sys_control_time = 0;  //定时闹铃时间
 uint8	   sys_time_up	   = false ;   //闹铃标志
 uint8	   sys_time_start = false;	//启动控制时间标志 0 = not ,1 = yes

 
uint8 target_percent = 0; //用于设定目标风机风速
uint8 now_percent = 0; //用于设定现在的实时风速

uint8 adc_sample_flag = 0; //adc 采样时间标志

uint8 T_PERCENT = 0;
uint32_t BJ_TimeVar;//定义时间戳变量

/*时间结构体，默认时间2000-01-01 00:00:00*/
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



Lcd_Read_Data read_lcd_data;//用于记录人工设置的系统参数
SYS_WORK_TIME sys_time_inf;//锅炉系统累计运行时间变量
SYS_WORK_TIME Start_End_Time; //用于记录本次启停的运行时间间隔，必须得烧起来

SYS_WORK_TIME big_time_inf;//锅炉小风量运行时间
SYS_WORK_TIME small_time_inf;//锅炉大风量运行时间

sys_flags sys_flag; //系统需要使用的标志量集合


SYS_CONFIG sys_config_data;//用于系统设置大小火延时等参数

SYS_ADMIN  Sys_Admin; //用于设置管理员参数

AB_EVENTS  Abnormal_Events;//用于系统运行时的异常记录
BYTE_WORD4 Secret_uint; //用于对4个字节转换为32位整型
BYTE_WORD1 Data_wordtobyte;//用于1WORD    2个字节转换

FLP_INT  Float_Int;//用于单精度浮点型数据的转化
BYTE_INT32 Byte_To_Duint32;  //用于4个字节到uint32的数据转换


LCD_QuXian lcd_quxian_data;//用于刷新数据统计的曲线
ERR_LCD  Err_Lcd_Code;//用于刷新lcd报警代码
LCD_FLASH_STRUCT  Lcd_FlashD;


LCD_E_M  Err_Lcd_Memory[8];//用于记录8个故障信息（时间和故障原因）
ERROR_DATE_STRUCT SPI_Error_Data;



IO_DATA IO_Status;
 Login_TT Login_D; //定义登录信息管理结构体

 Logic_Water Water_State;





 JUMP_TYPE  Jump_Step = First;



 










uint8  Air_Door_Index = 0;//用于磁性风门异常，跳转状态使用
uint8  ab_index =0 ;
 



//ADC_VrefintCmd()
 
 





uint8 Self_Index = 0;
uint8 Sys_Staus = 0;
uint8	Sys_Launch_Index = 0;
uint8 Ignition_Index = 0;
uint8	Pressure_Index = 0;
uint8 IDLE_INDEX = 0;





uint8 cmd_string111[7] = {0x5A,0xA5,0x04,0x80,0x03,0x00,0x4B}; //换页指令，切换到第76页


void Get_IO_Inf(void)
{
	uint8  Error16_Time = 8;
	
	uint8  Error_Buffer = 0;
		//固定一直检查信号： 燃气压力，机械式压力传感器信号
	
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
				sys_flag.Force_Flag = FALSE; // 22.07.12可能没有及时清楚强制补水变量
				sys_flag.Error16_Flag = 0;
				sys_flag.Error16_Count = 0;
			}
		//强制补水12秒，然后清除强制补水的标志
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
				//极低水位有
				if(IO_Status.Target.water_protect)
					Error16_Time = 8; //原12秒，现改成15秒2022年5月6日10:19:38
				else
					Error16_Time =5;//2022年7月12日14:18:33  由8改成12，增加点时间    2024年8月24日08:51:17 修改成5秒，上面8 秒
			}
		else
			Error16_Time = 10;
		
		if(sys_flag.Error16_Count >= Error16_Time)  //8秒
			{
				//sys_data.Data_l5H = SET_BIT_N(sys_data.Data_l5H,8);//报错，水位逻辑故障
				if(sys_flag.Error_Code == 0)
					sys_flag.Error_Code = Error8_WaterLogic;
				
				sys_flag.Error16_Flag = FALSE;
				sys_flag.Error16_Count = 0;
			}

	//固定一直检查信号： 燃气压力，机械式压力传感器信号
		 
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

		
		//机械式压力检测信号	
		if(IO_Status.Target.hpressure_signal == PRESSURE_ERROR)
			{
				if(sys_flag.Error1_Flag == 0)
					sys_flag.Error1_Count = 0;
				
				sys_flag.Error1_Flag = OK;		
				//若蒸汽压力超出安全范围，故障，报警
				if(sys_flag.Error1_Count > 1)
					{
						 if(sys_flag.Error_Code == 0 )
							sys_flag.Error_Code = Error1_YakongProtect; //蒸汽压力超出安全范围报警	
					}
			}
		else
			{
				sys_flag.Error1_Flag = 0;
				sys_flag.Error1_Count = 0;
			}
			

	
}



/**
  * @brief  处理点火前的相关准备工作
  * @param  sys_flag.before_ignition_index
  * @retval 准备好返回1，否则返回0
  */

uint8 Before_Ignition_Prepare(void)
{
		//1、水位信号必须有                2、流量信号必须有
		//sys_flag.before_ignition_index
		uint8 func_state = 0;

		func_state = 0;
		switch (sys_flag.before_ignition_index)
			{
				case 0 :
						//开主电磁阀，循环泵，sys_flag.Pai_Wu_Already检查水位信号决定是否调节流量控制阀
							 Send_Air_Open();  //打开风机前吹扫	
							 
							 PWM_Adjust(0); //等待5秒后开启
							 Pai_Wu_Door_Close();
							 delay_sys_sec(12000);
							 if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
							 	{
							 		if(Temperature_Data.Inside_High_Pressure <= 1)
							 			{
							 				LianXu_Paiwu_Open();
							 			}
							 	}
							 
							sys_flag.before_ignition_index = 1;//跳转到下个状态
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
								sys_flag.before_ignition_index = 2;//跳转到下个状态
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
				 			//相变机组，再次检查该阀有没有关闭
				 			LianXu_Paiwu_Close();
				 		}
					if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet-1))
						{
							sys_flag.Error_Code =  Error2_YaBianProtect; //蒸汽压力超出安全范围报警
						}

					sys_flag.before_ignition_index = 0;
					func_state = SUCCESS;	

					break;

			   default:
			   	sys_flag.before_ignition_index = 0;//恢复默认状态
			   			sys_close_cmd();
			   			break;
			}

		

		return func_state ;//点火前准备，准备好了，返回1
}



/**
  * @brief  检查并转串的IO，水位信息和热保护开关状态
* @param  
  * @retval 无
  */
 void Self_Check_Function()
{
	
	
	Get_IO_Inf(); //获取IO信息

	
		if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
		{
			
			sys_flag.Error_Code = Error16_SmokeValueHigh;
		}
		
						 
}

/**
  * @brief  系统点火程序
* @param   点火完成返回1，否则返回0
  * @retval 无
  */
uint8  Sys_Ignition_Fun(void)
{
		
		sys_data.Data_12H = 0x00; //点火过程中，没有对异常进行检测
		Abnormal_Events.target_complete_event = 0;
		switch(Ignition_Index)
		{
			case 0 : //  
						sys_flag.Ignition_Count = 0;
						sys_flag.FlameRecover_Time = 0; //对复位时间进行清零
						sys_flag.LianxuWorkTime = 0;  //对本阶段工作时间清零
						WTS_Gas_One_Close();
					
						/*******************PWM控制*一级风量吹扫***********************************/
						Send_Air_Open();  //风机前吹扫			
						//Feed_First_Level();

						delay_sys_sec(10000);
						
						Ignition_Index = 10; //切换流程，
							
						
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
							Ignition_Index = 1; //切换流程
						}
					else
						{
							
						}

					break;

		case 1:
					Feed_First_Level();
					if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) //相变款，不需要补到超高
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
					
					//时间到，也会继续执行程序
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
						
							delay_sys_sec(Sys_Admin.First_Blow_Time);  //正式吹扫时间 

							Ignition_Index = 20; //切换流程，,大风切点火风速
						}
					else
						{
							
						}

					
					break;

		case 20:
					Send_Air_Open();
					/*2024年11月28日09:26:13 解决在前吹扫过程中，未判定超高水位，会强制补水的问题*/
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

							
							
							Ignition_Index = 2; //切换流程，,大风切点火风速
						}
					else
						{
							
						}

					

					break;

		
		 
		case 2://进行风速切换,不可以随意变换，注意检查超压停炉程序
					Send_Air_Open();
					Send_Gas_Close();//燃气阀组关闭，关闭，关闭
					Feed_First_Level();//功率百分之60
					
					if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
						{
							if(IO_Status.Target.water_mid == WATER_OK)
								{
									if(sys_flag.Force_Supple_Water_Flag)
										{

											sys_flag.Force_Supple_Water_Flag = FALSE;
											sys_time_up = 1;  //直接进入下个流程
										}
									
								}
						}
					else
						{
							if(IO_Status.Target.water_shigh == WATER_OK)
								{
									sys_flag.Force_Supple_Water_Flag = FALSE;
									sys_time_up = 1;  //直接进入下个流程
								}
						}

					
					if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet-2))
						{
							sys_flag.Error_Code =  Error2_YaBianProtect; //蒸汽压力超出安全范围报警
						}
			
					if(sys_time_start == 0)
						sys_time_up = 1;
					if(sys_time_up)
						{
							sys_time_up = 0;
								
							if(IO_Status.Target.Air_Door == AIR_CLOSE)//风门关闭则报警，高电平报警
								sys_flag.Error_Code = Error9_AirPressureBad; //磁性风门故障
							else
								{
									//NOP
								}
								
							//远程一键锁停后，直接故障显示
							if(sys_config_data.Sys_Lock_Set)
								sys_flag.Error_Code = Error9_AirPressureBad; //磁性风门故障
							else
								{
								//NOP
								}

							
								
							PWM_Adjust(30);//检测功率
							if(Sys_Admin.Fan_Speed_Check)
								delay_sys_sec(20000);  //等待风速变化时间，超时则报警
							else
								delay_sys_sec(3000);
							Ignition_Index = 3;
							//Mcu_mail_Wifi();
						}

					break;
						
	case 3://正式开始点火，点火阀组先开1.5s
					Send_Air_Open();  //风门必须打开
					Send_Gas_Close();//燃气阀组关闭，关闭，关闭
					PWM_Adjust(30);//检测功率
					Dian_Huo_OFF();  //关闭点火继电器
					sys_flag.Force_Supple_Water_Flag = FALSE;
					//点火前确认，
					if (IO_Status.Target.water_protect== WATER_LOSE)
 						{
							sys_flag.Error_Code  = Error5_LowWater;
 						}
					if(Sys_Admin.Fan_Speed_Check)
						{
							if(sys_flag.Fan_Rpm > 200 && sys_flag.Fan_Rpm < Sys_Admin.Fan_Fire_Value)//点火风速在1000转和2500转之间
								{
									delay_sys_sec(8000);//增加风速稳定的时间
									Dian_Huo_Air_Level();//控制点火风速程序
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
									//风速控制失灵报故障，结束
									sys_flag.Error_Code = Error13_AirControlFail; //磁性风门故障//系统报警标志
								}
							else
								{
								//NOP
								}
						}
					else  //若不进行风速检测
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
									Dian_Huo_Air_Level();//控制点火风速程序
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
					Send_Air_Open();  //风门必须打开
					Send_Gas_Close();//燃气阀组关闭，关闭，关闭
					Dian_Huo_Air_Level();//控制点火风速程序
					Dian_Huo_OFF();  //关闭点火继电器
		
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
							
							
							Dian_Huo_Start();//点火启动
							delay_sys_sec(1500);// 
							Ignition_Index = 5;
						}
					else
						{
							
						}
					
					break;
	case 5://开燃气2.5s
					Send_Air_Open();  //风门必须打开
					Dian_Huo_Air_Level();//控制点火风速程序
				
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
							WTS_Gas_One_Open();//燃气阀1
							delay_sys_sec(3500);
							
							Ignition_Index = 6;
						}
					else
						{
							
						}

				break;
					 
	case 6: //点火阀组关闭，等待3秒后检测有无火焰，因硬件延迟
					Send_Air_Open();  //风门必须打开
					Dian_Huo_Air_Level();//控制点火风速程序
					 
					 
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

							//Dian_Huo_OFF(); //2023年10月17日12:21:58 注释掉该行代码
							Send_Gas_Open();
							delay_sys_sec(4800);  //修改 由1秒变为1.5秒
							Ignition_Index = 7;
						}
					else
						{
							
						}
		
					break;
	case 7://3秒时间到，有火焰则温火一段时间，无火焰则报警
					 
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
							Dian_Huo_OFF();  //关闭点火继电器
							WTS_Gas_One_Close();
							 
							if(sys_flag.flame_state == FLAME_OK )  //有火焰
							{
								 //点火成功，风机保持原状态，等待火焰稳定
								  delay_sys_sec(Sys_Admin.Wen_Huo_Time);  //设定稳定火焰时间10sec，
								 Ignition_Index = 8;//切换流程，点火成功，进入系统正常运行状态	
 
							}
							else  //无火焰
							{
								
								sys_flag.Ignition_Count ++;
								Send_Gas_Close();//关闭燃气阀组
								WTS_Gas_One_Close();
								
								Dian_Huo_OFF();  //关闭点火继电器，并将点火图标转为红色
								if(sys_flag.Ignition_Count < Max_Ignition_Times)
									{
										//执行第二次点火
										Ignition_Index = 9;
										Feed_First_Level();//
										delay_sys_sec(Sys_Admin.First_Blow_Time);  //设定下次点火时间间隔为20sec + 10秒点后风速，
				  					}
								else
									{
										sys_flag.Error_Code = Error11_DianHuo_Bad;//系统报警标志
										Ignition_Index = 0;
									}
									
							}
						}
					else
						{
							//NOP
						}
					
						
				break;
			case 8: //等待温火延时

					
					Dian_Huo_OFF();
					sys_flag.Force_UnSupply_Water_Flag = FALSE ;  //可以补水
					 //防止没到高水位，再开一次
					if(sys_flag.flame_state == FLAME_OUT)//稳火过程火焰熄灭
						{ 
							sys_flag.Ignition_Count ++;
								Send_Gas_Close();//关闭燃气阀组
								WTS_Gas_One_Close();
								Dian_Huo_OFF();  //关闭点火继电器，并将点火图标转为红色
								if(sys_flag.Ignition_Count < Max_Ignition_Times)
									{
										//执行第二次点火
										Ignition_Index = 9;
										Feed_First_Level();//
										delay_sys_sec(Sys_Admin.First_Blow_Time);  //设定下次点火时间间隔为20sec + 10秒点后风速，
									}
								else
									{
										sys_flag.Error_Code = Error11_DianHuo_Bad;//系统报警标志
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
							sys_time_up = 0;//火焰稳定时间到
							
	/**************************************跳转到第二阶段参数设置***START********************************************/
							 sys_flag.Ignition_Count = 0;
				
							return 1;
	/**************************************跳转到第二阶段参数设置***END********************************************/
						}
						else
						{
							
						}
				
					break;

			case 9://点火失败，切换风速流程
					Send_Gas_Close();//关闭燃气阀组
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
							Dian_Huo_Air_Level();//控制点火风速程序
							delay_sys_sec(6000);  //设定下次点火时间为15sec，
							Ignition_Index =4;//切换流程，准备再次点火,风机不停
						}  

					break;
			

			default:
					sys_close_cmd();
					break;
		}

		return 0;
		
}




/**
* @brief  检查运行时各器件状态，对于未启动有火焰，磁性风门，燃烧机热保护。炉体超温等皆是（异常），按异常处理，不报警
* @param   将故障和异常进行分离，燃气压力在系统运行和点火中检测
  * @retval 无
  */
void Auto_Check_Fun(void)
{

	uint8 Error_Buffer = 0;
		//***********读取并转串口的数据*************//
		Get_IO_Inf(); //获取IO信息
	
		//待机时风门应该闭合，若打开则异常，说明风门没落下来
		 if(IO_Status.Target.Air_Door == AIR_CLOSE) 
		 	if(sys_flag.Error_Code  == 0 )
				sys_flag.Error_Code  = Error9_AirPressureBad;
		
		if(IO_Status.Target.gas_low_pressure == GAS_OUT)
			{
				//若燃气压力低，故障，报警
				
				if(sys_flag.Error_Code  == 0 )
					sys_flag.Error_Code  = Error3_LowGas; //燃气压力低故障报警
			}
		
//检测极低水位保护
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
			
	
		if(sys_flag.Error5_Count >= 5)  //原设定 7秒， 现改成10秒 2022年5月6日10:11:58
			{
				 //运行中，极低水位缺水故障	
				 sys_data.Data_12H = 6; // 温度高于用户设定值0.01kg
				Abnormal_Events.target_complete_event = 1;//异常事件记录

				//sys_flag.Error_Code  = Error5_LowWater;  //转到超压停炉后，若再次出现水位的问题，直接停炉
				sys_flag.Error5_Flag = 0;
				sys_flag.Error5_Count = 0;
			}
								
//火焰探测器检测

		if(sys_flag.flame_state == FLAME_OUT) //等于0时，则无火焰信号
			{
					Send_Gas_Close();//燃气阀组关闭

					sys_flag.FlameOut_Count++;
					if(sys_flag.FlameOut_Count >= 3)
						{
							sys_flag.Error_Code  = Error12_FlameLose;
						}
					else
						{
							sys_data.Data_12H |= Set_Bit_5; // 温度高于用户设定值0.01kg
							Abnormal_Events.target_complete_event = 1;//异常事件记录
						}
			}
		
		if(sys_flag.FlameOut_Count)
			{
				//如果正常燃烧半小时候，自动对熄灭记录清零
				if(sys_flag.FlameRecover_Time >= 1800)
					sys_flag.FlameOut_Count = 0;
			}
		
		

	

	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
		{
			 
				sys_flag.Error_Code  = Error16_SmokeValueHigh;//排烟温度超高
		}
	
		
	if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 1))
		{
			 
				sys_flag.Error_Code  = Error2_YaBianProtect;
		}

		 
}

	
/**
* @brief  检查点火时各器件状态，，燃烧机热保护，燃气压力状态检测等皆是故障，必须报警提示。
* @param  不检测火焰和炉体超温,磁性风门
  * @retval 无
  */
void Ignition_Check_Fun(void)
{
		
		Get_IO_Inf(); //获取IO信息

		if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
		{
			
				sys_flag.Error_Code  = Error16_SmokeValueHigh;//排烟温度超高
		}

	 	//燃气压力状态检测
		if(IO_Status.Target.gas_low_pressure == GAS_OUT)
		{
				
				//若燃气压力低，故障，报警
				
				sys_flag.Error_Code = Error3_LowGas;
				
		}
		
	


	

}
	
	

		

/**
  * @brief  检查待机时各器件状态，对于未启动有火焰，燃烧机热保护。炉体超温等皆是故障，必须报警提示
* @param  不检测磁性风门和流量开关
  * @retval 无
  */
uint8 Idel_Check_Fun(void)
{
	//***********水位检查是一直要查的*************//
		
	 if(sys_flag.Error_Code )
	 		return 0;//如果有故障，直接退出，不再进行检测

	
	

	 
	  Get_IO_Inf(); //获取IO信息

	
	
	if (IDLE_INDEX == 0)
		{
		 if(sys_flag.flame_state == FLAME_OK)
			{
				if(sys_flag.Error_Code == 0 )
					sys_flag.Error_Code = Error7_FlameZiJian;
					 //待机时，肯定没有火焰，火焰探测器故障报警
			}
		
		}
		


	 
	 

		return 0 ;
		
}







uint8 System_Pressure_Balance_Function(void)
 	{
		

		static	uint16  Man_Set_Pressure = 0;  //1kg = 0.1Mpa  用于系统全局变量，用户可调节,当表示温度时，300 = 30.0度
		static  uint8 	air_min = 0;//最小风速
		static  uint8   air_max = 0;//最大风速
		static	uint16  	stop_wait_pressure = 0; //用于达到目标设定值时，风机功率记录 
		uint8  Tp_value = 0; //用于风机功率中间值

/*************************相变机组增加**************************************************/
		uint16 Real_Pressure = 0;
		static uint8   Yacha_Value = 65;  //固定压差0.45Mpa，原来65，现在调整到
		uint16 Max_Pressure = 150;  //15公斤  1.50Mpa
/******************************************************************************************/
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) 
		{
			//相变机组的启动压力跟还是根据用户的设定压力来启动
			
			Yacha_Value = 65;
			
			Real_Pressure = Temperature_Data.Inside_High_Pressure;
		}
	else
		{
			//常规蒸汽
			Yacha_Value = 0;
			Real_Pressure = Temperature_Data.Pressure_Value;
		}

	
		air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);//取点火功率为最小运行功率

		air_max = Sys_Admin.Max_Work_Power;  //对最大可运行功率进行边界保护
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
					
						if(sys_flag.Pressure_ChangeTime > 6) //8秒以上变化0.01，则0.1Mpa 需要100秒左右时间太长， 如果变化时间太短，比如小于2秒变化0.01，
							{
								sys_flag.get_60_percent_flag = OK; //需要更快
							}
		
						if(sys_flag.Pressure_ChangeTime <= 5)
							{
								sys_flag.get_60_percent_flag = 0;  //这个变化速率放慢
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
		
				if(now_percent > 80)//前提是必须大于40
					{
						Tp_value = 80;
					}
			/*以上这样写，有缺陷，尤其是针对小火位*/	

				
				sys_flag.get_60_percent_flag = 1;//燃烧预加热完成
			}

		/********************在相变机型，内压和外部压力都需要比较设定压力**********************************************/
		if(Real_Pressure > Man_Set_Pressure  || Temperature_Data.Pressure_Value > sys_config_data.zhuan_huan_temperture_value)
			{
				//衰减速度为每秒减1

				if(Temperature_Data.Pressure_Value > (sys_config_data.zhuan_huan_temperture_value ))
					{
						if(Real_Pressure > Man_Set_Pressure)
							{
								//两个都高，要降功率
								if(now_percent > 80)//前提是必须大于40
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
								//内侧压力小于相对值，外侧压力高的有限，则适当增大功率
								if(Real_Pressure <= (Man_Set_Pressure - 5) )
									{
										if(Temperature_Data.Pressure_Value < (sys_config_data.zhuan_huan_temperture_value + 1 ))
											{
												//则适当增大功率，保证内压的稳定
												if(sys_flag.Power_1_Sec)
													{
														sys_flag.Power_1_Sec = 0;
														Tp_value = Tp_value + 1;
													}
											}
										else
											{
												//外侧压力已经高出不少了，只能降功率
												if(sys_flag.Power_1_Sec)
													{
														sys_flag.Power_1_Sec = 0;
														Tp_value = Tp_value - 1;
													}
											}
										
									}
								else
									{
										//内压在相对范围内，当压力大于等于设定压力也要降低功率 
										if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value + 1 ))
											{
												//则适当降低功率，保证内压的稳定
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
						//没有到达用户设定压力，但是内侧压力已经超过设定值，则也需要降功率
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
			sys_flag.get_60_percent_flag = 1;//燃烧预加热完成

		
		PWM_Adjust(now_percent);

		


		//如果蒸汽压力大于设定压力0.05Mpa以上，则停炉
		if(Real_Pressure >= stop_wait_pressure  || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
			{
				sys_data.Data_12H |= Set_Bit_4; // 温度高于用户设定值0.01kg
				Abnormal_Events.target_complete_event = 1;//异常事件记录
			 
			}
		
 		return  OK;
 	}


uint8 XB_System_Pressure_Balance_Function(void)
 	{
		

		static	uint16  Man_Set_Pressure = 0;  //1kg = 0.1Mpa  用于系统全局变量，用户可调节,当表示温度时，300 = 30.0度
		static  uint8 	air_min = 0;//最小风速
		static  uint8   air_max = 0;//最大风速
		static	uint16  	stop_wait_pressure = 0; //用于达到目标设定值时，风机功率记录 
		uint8  Tp_value = 0; //用于风机功率中间值

/*************************相变机组增加**************************************************/
		uint16 Real_Pressure = 0;
		static uint8   Yacha_Value = 65;  //固定压差0.45Mpa，原来65，现在调整到
		uint16 Max_Pressure = 150;  //15公斤  1.50Mpa
/******************************************************************************************/
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) 
		{
			//相变机组的启动压力跟还是根据用户的设定压力来启动
			
			Yacha_Value = 65;
			
			Real_Pressure = Temperature_Data.Inside_High_Pressure;
		}
	else
		{
			//常规蒸汽
			Yacha_Value = 0;
			Real_Pressure = Temperature_Data.Pressure_Value;
		}

	
		air_min = *(uint32 *)(DIAN_HUO_POWER_ADDRESS);//取点火功率为最小运行功率

		air_max = Sys_Admin.Max_Work_Power;  //对最大可运行功率进行边界保护
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
					
						if(sys_flag.Pressure_ChangeTime > 6) //8秒以上变化0.01，则0.1Mpa 需要100秒左右时间太长， 如果变化时间太短，比如小于2秒变化0.01，
							{
								sys_flag.get_60_percent_flag = OK; //需要更快
							}
		
						if(sys_flag.Pressure_ChangeTime <= 5)
							{
								sys_flag.get_60_percent_flag = 0;  //这个变化速率放慢
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
		
				if(now_percent > 80)//前提是必须大于40
					{
						Tp_value = 80;
					}
			/*以上这样写，有缺陷，尤其是针对小火位*/	

				
				sys_flag.get_60_percent_flag = 1;//燃烧预加热完成
			}

		/********************在相变机型，内压和外部压力都需要比较设定压力**********************************************/
		if(Real_Pressure > Man_Set_Pressure  || Temperature_Data.Pressure_Value >= sys_config_data.zhuan_huan_temperture_value)
			{
				//衰减速度为每秒减1

				if(Temperature_Data.Pressure_Value >= (sys_config_data.zhuan_huan_temperture_value ))
					{
						//两个条件同时满足，则必须减功率
						if(Real_Pressure > Man_Set_Pressure)
							{
								//两个都高，要降功率
								if(now_percent > 80)//前提是必须大于40
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
								//内侧压力小于相对值，外侧压力高的有限，则适当增大功率
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
						//没有到达用户设定压力，但是内侧压力已经超过设定值，则也需要降功率
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
			sys_flag.get_60_percent_flag = 1;//燃烧预加热完成

		
		PWM_Adjust(now_percent);

		//如果蒸汽压力大于设定压力0.05Mpa以上，则停炉
		if(Real_Pressure >= stop_wait_pressure  || Temperature_Data.Pressure_Value >= sys_config_data.Auto_stop_pressure)
			{
				sys_data.Data_12H |= Set_Bit_4; // 温度高于用户设定值0.01kg
				Abnormal_Events.target_complete_event = 1;//异常事件记录
			 
			}
		
 		return  OK;
 	}




/**
	 * @brief  系统运行过程中，异常的应变处理，异常处理次数的累加，根据应变结果，进化为系统故障
	 * @param    运行时火焰熄灭异常
							 炉内超温异常
							 磁力开关闭合异常
							 燃烧器热保护开关异常
  * @retval 无
  */
void  Abnormal_Events_Response(void)
{
		
	
//当出现异常时，第一步：执行关闭燃气阀组，风机延时吹扫，吹扫时间，用户可调
	 
		
		if (sys_data.Data_12H)
			{
			switch (ab_index)
				{
					case 0:
						   Dian_Huo_OFF();
						   Send_Gas_Close();//关闭燃气阀组	
						   Feed_First_Level();

						   if(sys_flag.LianxuWorkTime < 600) //小于10分钟，就慢功率
						   	{
						   		sys_flag.get_60_percent_flag = 0;  //点着火后，小功率运行
						   	}
						   
						   delay_sys_sec(1000);//执行后吹扫延时5秒
						   	ab_index = 1; //跳转程序
							break;
					case 1:
							//改强制熄灭LCD图标
						
							Send_Gas_Close();//关闭燃气阀组
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
									ab_index = 2; //跳转程序
									delay_sys_sec(Sys_Admin.Last_Blow_Time);//
								}
							else
								{
								//NOP
								}

							
							break;
					case 2:
							//改强制熄灭LCD图标
						
							Send_Gas_Close();//关闭燃气阀组
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
									ab_index = 3; //跳转程序
									delay_sys_sec(1000);//
									sys_flag.Force_Supple_Water_Flag = 0;
									/*2023年11月27日11:59:34*/
							
									/*检查极低水位的状况*/
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
						    Send_Gas_Close();//关闭燃气阀组	
							Feed_First_Level();
						
							if(sys_flag.flame_state == FLAME_OK)
								{
									 //这时，肯定没有火焰，火焰探测器故障报警
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
									 //跳转程序
									//Send_Air_Close();//关闭风机	
									 if(sys_data.Data_12H == 3)
									 		ab_index = 10;//跳转自动排污程序
									 else
									 	{
									 		if( Temperature_Data.Pressure_Value <= sys_config_data.Auto_start_pressure   || sys_data.Data_12H == 5 || sys_data.Data_12H == 6)
												{
													sys_data.Data_12H = 0 ;// 温度低于停炉值
													Abnormal_Events.target_complete_event = 0;
													memset(&Abnormal_Events,0,sizeof(Abnormal_Events));//对异常结构体清零
											 
													ab_index = 0;  //对index初始化，允许下次进入
													//需添加相关指示页面，从而提醒用户，系统正在冷却
													//恢复相关参数，进行重启
													sys_data.Data_10H = SYS_WORK;// 进入工作状态
													Sys_Staus = 2; //系统跳到第2阶段，启动运行
													Sys_Launch_Index = 1; //进行点火前检查
													
													Ignition_Index = 0;  //******************************跳过前吹扫，直接进行风速检查过程
													Send_Air_Open();  //风机前吹扫
													
												//	Feed_First_Level();//大风量吹扫										
													delay_sys_sec(1000);//延迟12s
													
													
												}
											else
												{
													ab_index = 4; //跳转程序
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
										//双泵需要打开，其它关闭
											Dian_Huo_OFF();
											Send_Gas_Close();//关闭燃气阀组
											
											if( Temperature_Data.Pressure_Value <= sys_config_data.Auto_start_pressure ||  sys_data.Data_12H == 5 || sys_data.Data_12H == 6)
												{
													sys_data.Data_12H = 0 ;// 温度低于停炉值
													Abnormal_Events.target_complete_event = 0;
													memset(&Abnormal_Events,0,sizeof(Abnormal_Events));//对异常结构体清零
											 
													ab_index = 0;  //对index初始化，允许下次进入
													//需添加相关指示页面，从而提醒用户，系统正在冷却
													//恢复相关参数，进行重启
													sys_data.Data_10H = SYS_WORK;// 进入工作状态
													Sys_Staus = 2; //系统跳到第2阶段，启动运行
													Sys_Launch_Index = 1; //进行点火前检查
													
													Ignition_Index = 0;  //最终跳转地址，点火前一阶段
													Send_Air_Open();  //风机前吹扫 
													//Feed_First_Level();//大风量吹扫										
													delay_sys_sec(1000);//延迟12s
													
												}
											else
												{
													Send_Air_Close();//关闭风机
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
				ab_index = 0;  //对index初始化，允许下次进入
			}
			


		
	
	
	
}
/**
  * @brief  系统运行程序
* @param   Sys_Launch_Index变量，切换系统运行步骤
  * @retval 无
  */
void Sys_Launch_Function(void)
{
		switch(Sys_Launch_Index)
		{
			case  0: //系统自检
						Self_Check_Function();//检查燃气压力和机械式压力传感器
						
						if(Before_Ignition_Prepare())
						{
								Ignition_Index = 0;
								Sys_Launch_Index = 1;//跳转到下个流程：点火阶段
								
						}
						
					break;
			
			case  1: //系统初启动
						
						Ignition_Check_Fun();
						if(Sys_Ignition_Fun())
							{
								Sys_Launch_Index = 2;//切换系统流程到正常运转状态
							
								Ignition_Index = 0; //复位本程序跳转变量

								delay_sys_sec(2000);//很重要，没有，怎下一阶段的程序，执行不起来 

								sys_data.Data_12H = 0; //对异常检测记录复位
								Abnormal_Events.airdoor_event = 0;
								Abnormal_Events.burner_heat_protect_count = 0;
								Abnormal_Events.flameout_event = 0;
								Abnormal_Events.overheat_event = 0;

								sys_flag.WaterUnsupply_Count = 0; //长时间未补水标志超时清零
							}
						Self_Index = 0;
						ab_index =0;
						Air_Door_Index = 0;

				break;
			
			case  2: //系统运行
			
						sys_flag.Force_Supple_Water_Flag = FALSE; //运行状态关闭强制补水
						sys_flag.Already_Work_On_Flag = OK ;
								
					    if(sys_data.Data_12H == 0)
					    	{
					    		Auto_Check_Fun();  //当没有异常时，执行IO和各参数检测
				   				System_Pressure_Balance_Function();
								
								
								if(sys_flag.Paiwu_Flag)
									sys_data.Data_12H = 3 ;//需要进行排污的标志
					    	}
						else//异常状态对一些状态量的检测
							{
								Abnormal_Check_Fun();
							}
	
						Abnormal_Events_Response(); //异常检测
						
					break;
			
			default:
					sys_close_cmd();
					Sys_Launch_Index = 0;
					break;
		}	
}





void Abnormal_Check_Fun(void)
{
	//检测燃气压力是否正常，流量开关是否正常，炉内热水是否超温
		Get_IO_Inf();
	
		
		if(IO_Status.Target.gas_low_pressure == GAS_OUT)
		{
				
				//若燃气压力低，故障，报警
				if(sys_flag.Error_Code  == 0 )
					sys_flag.Error_Code  = Error3_LowGas; //燃气压力低故障报警
				
		}
		


	


	if(Temperature_Data.Smoke_Tem > Sys_Admin.Danger_Smoke_Value)
		{
		
			sys_flag.Error_Code  = Error16_SmokeValueHigh;//排烟温度超高
		}
	
		
	if( Temperature_Data.Pressure_Value >= (Sys_Admin.DeviceMaxPressureSet - 1))
		{
			

			sys_flag.Error_Code  = Error2_YaBianProtect;
		}


	
		
}

//刷新LCD故障信息记录表格
void Lcd_Err_Refresh(void)
{
	
	
}

void Lcd_Err_Read(void)
{
	
	
}

 

void  Err_Response(void)
{
	static uint8 Old_Error = 0;
	//如果有故障报警，停炉，14H，15H为报警变量
	  if(sys_flag.Error_Code == 0)
	  	{
	  		if(sys_flag.Lock_Error)
				sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器
				
	  			sys_flag.Error_Code = 0;
	  			sys_flag.Lock_Error = 0;//对故障解锁
				Beep_Data.beep_start_flag = 0;	//清除报警声音
					
	  	}

	/******************新增重复故障记录的时间************************/
	  if(sys_flag.Old_Error_Count >=1800)
	  	{
	  		Old_Error = 0; //再次记录
	  		sys_flag.Old_Error_Count = 0;
	  	}
	  else
	  	{
	  	
	  	}

	  
	 //报警的前提是必须先激活
	 if(sys_flag.Lock_Error == 0)
	 	{
	 		if(sys_flag.Error_Code )
				{
			 		sys_close_cmd();
			 		sys_flag.Lock_Error = 1;  //对故障进行锁定
			 		sys_flag.Alarm_Out = OK;
			 		Beep_Data.beep_start_flag = 1;//控制蜂鸣器声音	
					
					
					if(sys_flag.Error_Code != Old_Error)
							{
								Old_Error = sys_flag.Error_Code;
								
							}
						else
							{
							
							}

						sys_flag.Old_Error_Count = 0; //故障记录时间清零

					
				
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
			 				Beep_Data.beep_start_flag = 1;//控制蜂鸣器声音	
	 					}
	 			}
	 		
	 		
	 	}

	 	 
				
	  
}


void  IDLE_Err_Response(void)
{
	static uint8 Old_Error = 0;
	//如果有故障报警，停炉，
	  if(sys_flag.Error_Code == 0)
	  	{
	  		if(sys_flag.Lock_Error)
				sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器

			sys_flag.Error_Code = 0;
	  			sys_flag.Lock_Error = 0;  //对故障解锁
					Beep_Data.beep_start_flag = 0;	//清除报警声音
					
	  	}

	  
	/******************新增重复故障记录的时间************************/
	  if(sys_flag.Old_Error_Count >=1800)
	  	{
	  		Old_Error = 0; //再次记录
	  		sys_flag.Old_Error_Count = 0;
	  	}
	  else
	  	{
	  	
	  	}


		//如果有故障报警，停炉，
		 if (sys_flag.Lock_Error == 0)
 		 	{	
		  		
  				if(sys_flag.Error_Code && sys_flag.Error_Code != 0xFF)
  					{
  						
						Sys_Staus = 0;  //系统进入报警程序
						
						
						
						if(sys_data.Data_10H == 2)
							{
								sys_close_cmd();
							}
						else
							{
								
							}
						
						
						Beep_Data.beep_start_flag = 1;	
						sys_flag.Lock_Error = 1;  //对故障进行锁定
						sys_flag.Alarm_Out = OK;
						sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器
						//刷新LCD故障信息记录表格
						if(sys_flag.Error_Code != Old_Error)
							{
								Old_Error = sys_flag.Error_Code;
							
							}
						else
							{
							
							}

						sys_flag.Old_Error_Count = 0; //故障记录时间清零
						
					
						
						
  					}
		  				
		  			
				
			}
	
	
	  
}



/**
* @brief  系统待机，关闭所有电机，等待启动命令，向服务器发送待机指令
* @param   
  * @retval 无
  */
void System_Idel_Function(void)
{
	//1、	该关的全部关掉 
		
		Send_Air_Close();
 		Dian_Huo_OFF();//控制点火继电器关闭
		Send_Gas_Close();//燃气阀组关闭
		WTS_Gas_One_Close();
		
		Auto_Pai_Wu_Function();
	
		 
}

/**
* @brief  系统总控程序
* @param   
  * @retval 无
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
								
								Water_BianPin_Function();//变频补水模式
							}
						else
							{
								Water_Balance_Function();//常规补水模式
							}
						
						break;
				case 2:
				case 3:
						//非变频补水
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
	//补水功能

		if(sys_flag.Work_1S_Flag)
			{
				//取个风机功率运行的时间，然后计数，用于点火过程中的吹扫时间控制
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

					case 0 :	//系统待机

						 switch(IDLE_INDEX)
						 	{
						 		case  0 : //正常待机状态  ,, 注意待机状态循环水泵的开启，根据回水温度
						 				
						 				sys_flag.Ignition_Count = 0;//待机时对点火次数清零
										sys_flag.last_blow_flag = 0;//后吹扫状态结束标志
									
										 System_Idel_Function( );//待机功能处理
										//检查基本输入量，实时显示到屏上，只提醒不执行
										 Idel_Check_Fun();
										 IDLE_Err_Response();
										 Sys_Launch_Index = 0;
										break;

								case  1: //等待后吹扫延时
									 
										Send_Gas_Close();//燃气阀组关闭
									 	Dian_Huo_OFF();//控制点火继电器关闭
										sys_data.Data_14H &= Rst_Bit_0;//这个状态特殊，风门报警一旦解除，不再报警
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
											IDLE_INDEX = 2;//进入正常待机状态
											//关闭风机，吹扫结束，进入待机
											Send_Air_Close();
										}
										break;
								case 2: //等待风门自由落下，防止误检测，大概10秒左右
									  Send_Air_Close();//风机电源关闭
									  Send_Gas_Close();//燃气阀组关闭
									  Dian_Huo_OFF();//控制点火继电器关闭
									 
									  Get_IO_Inf();
									  IDLE_Err_Response();
									
	 									sys_time_up = 0;
	 									IDLE_INDEX = 0;//进入正常待机状态
	 									Last_Blow_End_Fun();//后吹扫必须执行到位
	 									sys_flag.Force_Supple_Water_Flag = 0;
										sys_flag.Force_UnSupply_Water_Flag = FALSE ;
	
										break;

								default :
										Sys_Staus = 0;
										IDLE_INDEX = 0;
										break;
						 	}
					
							
						break;
					
					case 2:		//系统启动
						
						Sys_Launch_Function();
						 //用于控制速度
						Err_Response();//进行错误状态响应
						break;
			
					case 3://手动测试状态
							//手动模式：1、 中水位，丢失，自动补水，高水位则停
							
							//将转速的值送往LCD显示
							
							if(IO_Status.Target.hot_protect == THERMAL_BAD)
								{
									if(sys_flag.Error_Code == 0 )
										sys_flag.Error_Code = Error15_RebaoBad;
								}

		
							//机械式压力检测信号	
							if(IO_Status.Target.hpressure_signal == PRESSURE_ERROR)
								{
											
									//若蒸汽压力超出安全范围，故障，报警
									 if(sys_flag.Error_Code == 0 )
										sys_flag.Error_Code = Error1_YakongProtect; //蒸汽压力超出安全范围报警			
								}
							Send_Gas_Close();//燃气阀组关闭
							
							IDLE_Err_Response();
			
							break;


					case 4://故障报警模式
							
							if(sys_flag.Error_Code == 0)
									{
										if(sys_flag.Lock_Error)
											sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器
							
										sys_flag.Error_Code = 0;
										sys_flag.Lock_Error = 0;  //对故障解锁
										Beep_Data.beep_start_flag = 0;	//清除报警声音	

										//要进行状态跳转
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
//系统累计运行时间,锅炉开启时间
	 uint16  data = 0;
	static uint8 Work_State = 0;
	static uint8 Main_secs = 0;
	
		
			 
	data = sys_time_inf.All_Minutes / 60;
	 
	 

	
	lcd_data.Data_21H = data >> 8;
	lcd_data.Data_21L = data & 0x00FF; //用于界面显示
	

	if(sys_data.Data_10H == 0)
		{
			//上个状态时运行状态 
			if(Work_State == 2)
				{
					//说明主控进行关机，则对相关数据进行保存
					//Write_Second_Flash();
				}
			Work_State = sys_data.Data_10H ;
		}
	else
		Work_State =sys_data.Data_10H;

	
	//不满足1秒时间或处于待机状态，直接退出
	if(sys_flag.Work_1sec_Flag == FALSE || sys_data.Data_10H == 0)
		return 0;

	
	sys_flag.Work_1sec_Flag = FALSE; //消除标志位
		
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

//设置开机系统默认参数配置
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
			
			sys_flag.Wifi_Lock_System = 0; //默认wifi没锁机
			sys_flag.wifi_Lock_Year = 0;
			sys_flag.wifi_Lock_Month = 0;
			sys_flag.Wifi_Lock_Day = 0;

			
			Sys_Admin.ChaYa_WaterHigh_Enabled = 0;
			Sys_Admin.ChaYa_WaterLow_Set = 100; //200mm
			Sys_Admin.ChaYa_WaterMid_Set = 180; // 
			Sys_Admin.ChaYa_WaterHigh_Set = 230; // 
			
 			

			Sys_Admin.Device_Style  = 0;  //0 则是常规单体1吨蒸汽，1则相变蒸汽运行
		
			
			Sys_Admin.LianXu_PaiWu_DelayTime = 10; //默认15分钟动作一次，每次3秒
			Sys_Admin.LianXu_PaiWu_OpenSecs = 4; //精度到1s,默认开启3秒

			Sys_Admin.Water_BianPin_Enabled = 0;  //默认不打开变频补水功能
			Sys_Admin.Water_Max_Percent = 45; 
			
			
			Sys_Admin.YuRe_Enabled  = 1; //默认打开副温保护
			Sys_Admin.Inside_WenDu_ProtectValue  = 270;// 本体温度默认为270度

			Sys_Admin.Steam_WenDu_Protect  = 173;//取消不需要 蒸汽温度默认为180度
		
			Sys_Admin.Special_Secs = 18;
			 
			sys_time_inf.UnPaiwuMinutes = 0;
		
			
			Sys_Admin.Balance_Big_Time = 90;
			Sys_Admin.Balance_Small_Time = 150;
		
			Sys_Admin.DeviceMaxPressureSet = 100; //默认额定压力是10公斤
			
		//第一步： 对相应结构体赋值
			Sys_Admin.First_Blow_Time = 30 * 1000;  //前吹扫时间
	 	
	
			Sys_Admin.Last_Blow_Time = 30 *1000;//后吹扫时间
			

			Sys_Admin.Dian_Huo_Power = 30;  //默认点火功率为30% 
		
			Sys_Admin.Max_Work_Power = 85;  //默认最大功率为100
			Sys_Admin.Wen_Huo_Time =6 * 1000;  //稳定火焰时间 10秒

			Sys_Admin.Fan_Speed_Check = 1;  //默认是检测风速	
			
			 Sys_Admin.Fan_Speed_Value = 4800; //默认风机最大转速为6600；

			  Sys_Admin.Fan_Pulse_Rpm = 3;   //默认每转脉冲数3个，Amtek 

			 Sys_Admin.Fan_Fire_Value = 3000 ; //默认风机点火检测转速为3500rpm

			 Sys_Admin.Danger_Smoke_Value =  850; //排烟温度默认值为80度
			 Sys_Admin.Supply_Max_Time =  320; //补水超时默认报警值为300秒
			
			 Sys_Admin.ModBus_Address = 0; //默认地址为20

			 sys_config_data.Sys_Lock_Set = 0;  //默认不进行锁停控制
 
		  
		   	sys_config_data.Auto_stop_pressure = 60; //若设置4kg,停炉默认为5kg

			sys_config_data.Auto_start_pressure = 40; //若设置4kg,启动压力就1kg启动锅  
	 		sys_config_data.zhuan_huan_temperture_value = 50; //设置目标压力值0.4Mpa
	 		
			
			Sys_Admin.Admin_Work_Day = 0; //可运行时间管理，默认为0，默认不开启
			Sys_Admin.Admin_Save_Day = 30;
			Sys_Admin.Admin_Save_Month = 12;
			Sys_Admin.Admin_Save_Year = 2025;
			
		//第一步： 写入内部FLASH
			sys_flag.Lcd_First_Connect = OK;

			
	 	 	sys_time_inf.All_Minutes = 1; 
			 
			Write_Internal_Flash();
			Write_Admin_Flash();
			Write_Second_Flash();
			 
			
			
		}
	else  //说明已经写入过，不再向该内存填入出厂数据,读出内部FLASH内容，赋值给相应结构体
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

			Sys_Admin.Inside_WenDu_ProtectValue  = *(uint32 *)(BENTI_WENDU_PROTECT_ADDRESS);//本体温度
		//	Sys_Admin.Steam_WenDu_Protect  = *(uint32 *)(STEAM_WENDU_PROTECT_ADDRESS);//蒸汽温度
		
			Sys_Admin.Special_Secs = *(uint32 *)(SPECIAL_SECS_ADDRESS);
			
			
		
			Sys_Admin.Balance_Big_Power = *(uint32 *)(BALANCE_BIGPOWER_ADDRESS);
			Sys_Admin.Balance_Small_Power = *(uint32 *)(BALANCE_SMALLPOWER_ADDRESS);
			Sys_Admin.Balance_Big_Time = *(uint32 *)(BALANCE_BIGTIME_ADDRESS);
			Sys_Admin.Balance_Small_Time = *(uint32 *)(BALANCE_SMALLTIME_ADDRESS);

			
		
			Sys_Admin.Access_Data_Mode = *(uint32 *)(ACCESS_DATA_MODE_SET_ADDRESS);	
			
			Sys_Admin.DeviceMaxPressureSet = *(uint32 *)(DEVICE_MAX_PRESSURE_SET_ADDRESS);
			
			
			 sys_config_data.Sys_Lock_Set =  *(uint32 *)(SYS_LOCK_SET_ADDRESS); 

			  Sys_Admin.Supply_Max_Time =*(uint32 *)(SUPPLY_MAX_TIME_ADDRESS); 
			
			Sys_Admin.First_Blow_Time = *(uint32 *)(FIRST_BLOW_ADDRESS);  //预吹扫时间
			
		
			Sys_Admin.Last_Blow_Time =  *(uint32 *)(LAST_BLOW_ADDRESS);//后吹扫时间
			
			
			Sys_Admin.Dian_Huo_Power =  *(uint32 *)(DIAN_HUO_POWER_ADDRESS);  //点火功率
			


			Sys_Admin.Max_Work_Power = *(uint32 *)(MAX_WORK_POWER_ADDRESS);  //默认最大功率为100
			
			Sys_Admin.Wen_Huo_Time = *(uint32 *)(WEN_HUO_ADDRESS);  //稳定火焰时间  

			Sys_Admin.Fan_Speed_Check = *(uint32 *)(FAN_SPEED_CHECK_ADDRESS);  //是否进行风速检测
			
			Sys_Admin.Fan_Speed_Value = *(uint32 *)(FAN_SPEED_VALUE_ADDRESS);  //是否进行风速检测
			Sys_Admin.Fan_Fire_Value = *(uint32 *)(FAN_FIRE_VALUE_ADDRESS);

			Sys_Admin.Danger_Smoke_Value = *(uint32 *)(DANGER_SMOKE_VALUE_ADDRESS);
			 
			
			 Sys_Admin.ModBus_Address = *(uint32 *)(MODBUS_ADDRESS_ADDRESS) ;  
			
			sys_config_data.wifi_record = *(uint32 *)(CHECK_WIFI_ADDRESS);  //取出wifi记录的值

			sys_config_data.zhuan_huan_temperture_value = *(uint32 *)(ZHUAN_HUAN_TEMPERATURE); //设置目标压力值0.4Mpa

			sys_config_data.Auto_stop_pressure = *(uint32 *)(AUTO_STOP_PRESSURE_ADDRESS); //取出自动停炉压力

			sys_config_data.Auto_start_pressure = *(uint32 *)(AUTO_START_PRESSURE_ADDRESS);//取出自动启炉压力

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

			/**********************历史故障信息提取  *************************************/
			
			/**********************历史故障信息提取  结束*************************************/		
			
		}

		LCD10D.DLCD.YunXu_Flag = SlaveG[1].Key_Power; 

		
	     Sys_Admin.Admin_Work_Day = *(uint32 *)(ADMIN_WORK_DAY_ADDRESS); 
		 Sys_Admin.Admin_Save_Day = *(uint32 *)(ADMIN_SAVE_DAY_ADDRESS);
		 Sys_Admin.Admin_Save_Month = *(uint32 *)(ADMIN_SAVE_MONTH_ADDRESS);
		 Sys_Admin.Admin_Save_Year = *(uint32 *)(ADMIN_SAVE_YEAR_ADDRESS);


  	
	 	 sys_time_inf.All_Minutes = *(uint32 *)(SYS_WORK_TIME_ADDRESS); 

	 

  //最终，将数据发给LCD展示
	
	
}



//将错误信息由整形转换为bit,根据数据刷新lcd故障码
uint8  byte_to_bit(void)
{
	 

	


		return 0;
}













//加载LCD发给MCU的数据
void Load_LCD_Data(void)
{
	
}





void clear_struct_memory(void)
{
	uint8 temp = 0;
		//对结构体变量初始化	
	memset(&sys_data,0,sizeof(sys_data));	//对状态信息结构体清零
  	memset(&lcd_data,0,sizeof(lcd_data));	//对状态信息结构体清零
	memset(&sys_time_inf,0,sizeof(sys_time_inf));	//对状态信息结构体清零
	
	memset(&sys_config_data,0,sizeof(sys_config_data));	//对状态信息结构体清零
	
	
	memset(&Switch_Inf,0,sizeof(Switch_Inf));//对系统标志量进行清零
	memset(&Abnormal_Events,0,sizeof(Abnormal_Events));//对异常结构体清零
	memset(&sys_flag,0,sizeof(sys_flag));//对系统标志清零
	
	memset(&Flash_Data,0,sizeof(Flash_Data));
	memset(&Temperature_Data,0,sizeof(Temperature_Data));
	 
	
	
}








void One_Sec_Check(void)
{
 	 
	 //三秒呼吸效果，验证控制在正常运行
	if(sys_flag.Relays_3Secs_Flag)
		{
			sys_flag.Relays_3Secs_Flag = 0;

	//	u1_printf("\n* 通信地址测试 = %d\n",Sys_Admin.ChaYa_WaterHigh_Set);
	//	u1_printf("\n* 中液位高度 = %d\n",Sys_Admin.ChaYa_WaterMid_Set);
	
	//		u1_printf("\n* 从机小功率时间 = %d\n",SlaveG[2].Small_time);
	//		u1_printf("\n* 小平衡设定时间 = %d\n",Sys_Admin.Balance_Small_Time);

		
		

			
		}
	 
	
	
		



				



	

	
//打印测试信息
	if(sys_flag.two_sec_flag)
		{
			sys_flag.two_sec_flag = 0;
			
			//sys_flag.LianxuWorkTime
			//u1_printf("\n* 设置的时间= %d\n",Sys_Admin.LianXu_PaiWu_DelayTime);
			//u1_printf("\n* 已经运行的时间= %d\n",sys_flag.LianxuWorkTime);
			//u1_printf("\n* s设置开启的时间= %d\n",Sys_Admin.LianXu_PaiWu_OpenSecs);
			
			//u1_printf("\n* 开启的时间= %d\n",sys_flag.Lianxu_OpenTime);
		//	u1_printf("\n* 补水的标志= %d\n",Switch_Inf.water_switch_flag);

			
		}

		
	
	
}



uint8  sys_start_cmd(void)
{
		

		if(sys_flag.Lock_System)
			{
				//跳转到故障界面，但无故障代码显示
				
				return 0 ;
			}
		
		 
		if(sys_flag.Error_Code)
			{
					 	Sys_Staus = 0;  // 
						sys_data.Data_10H = 0x00;  //系统停止状态
						sys_data.Data_12H = 0x00; //对防冻保护异常进行清零

						
						
						delay_sys_sec(100);// 
					
						IDLE_INDEX = 1; 

						sys_flag.Lock_Error = 1;  //对故障进行锁定
						sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器
						Beep_Data.beep_start_flag = 1;	
						
			}
		else
			{
				if(sys_data.Data_10H == 0)
					{
						IDLE_INDEX = 0;  //防止在后吹扫时误操作
						Sys_Staus = 2;
						Sys_Launch_Index = 0;
						sys_flag.before_ignition_index = 0;
						Ignition_Index = 0;
						sys_time_up = 0;	

	   					 sys_data.Data_10H = 0x02;  //系统运行状态
					
						sys_flag.Paiwu_Flag = 0; //这样写会引起什么原因呢
						
						
	    				sys_time_start = 0; //清除待机状态下，可能存在的延时等待，防止误干扰系统
					/************对待机循环泵工作时间变量清零*****************8*/
						
						sys_flag.Already_Work_On_Flag = FALSE;
					
						sys_flag.get_60_percent_flag = 0;
						sys_flag.Pai_Wu_Idle_Index = 0;

						sys_flag.before_ignition_index = 0;	
						sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器											
	    				Dian_Huo_OFF();//控制点火继电器关闭
	    				//LCD切换到主页面
	    				
					}
				
				
			}
	    
		
	return 0;							
}


void sys_close_cmd(void)
{
			sys_data.Data_10H = 0x00;  //系统停止状态
																		
			lcd_data.Data_16H = 0X00;
			lcd_data.Data_16L = 0x00;  //系统刷新控制图标，显示START
			//系统停止，对关键数据进行存储
		 	WTS_Gas_One_Close();
		  	
		
			Abnormal_Events.target_complete_event = 0;
			Dian_Huo_OFF();//关闭点火继电器
			Send_Gas_Close();//关闭燃气阀组 
			
			sys_flag.get_60_percent_flag = 0;
		
			 
			sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器
			 
			Write_Second_Flash(); //保存运行时间的值
		  //对上次程序中可能存在的异常状态进行清0
		memset(&Abnormal_Events,0,sizeof(Abnormal_Events));	//对状态信息结构体清零			
														
		//进行后吹扫延时
		//打开风机二挡后吹扫延时
		//进入待机状态1
		//标准跳转步骤
		sys_data.Data_10H = SYS_IDLE; // 
		Sys_Staus = 0; // 
		Sys_Launch_Index = 0;
		sys_flag.before_ignition_index = 0;
		Ignition_Index = 0;
		IDLE_INDEX = 1;
		Last_Blow_Start_Fun();
	
}


//后吹扫开始执行程序
void Last_Blow_Start_Fun(void)
{
	//确认风机已经打开
	Send_Air_Open();

	sys_flag.last_blow_flag = 1;//后吹扫状态开始标志
	 
	sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器

	
	
	Feed_First_Level();//90%的风量进行后吹扫
	if(sys_flag.Already_Work_On_Flag)
		delay_sys_sec(Sys_Admin.Last_Blow_Time);//执行后吹扫延时	
	else
		delay_sys_sec(15000);//点火没成功，就吹个15秒
}


/*清除后吹扫结束标志，  软故障主动复位。点火失败故障，燃气阀组泄露故障，系统运行中火焰熄灭*/

void Last_Blow_End_Fun(void)
{
	//确认风机关闭
	
			Send_Air_Close();

	sys_flag.tx_hurry_flag = 1;//立即发送数据给服务器
	 
	 
	 
	sys_flag.last_blow_flag = 0;//后吹扫状态结束标志
}

 




/*防止用户切换到手动测试页面，长时间没有退出手动测试，10分钟后退出手动测试*/



//采用继电器信号控制流量开启，运用两根水位信号针,并检查水位逻辑错误
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


//针对运行过程中，超高水位的探针挂水的问题
		if(sys_data.Data_10H == 2)
			{
				//如果高水位没有信号
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//解决运行中，超高水位显示不准的问题
						if (IO_Status.Target.water_shigh== WATER_OK)
							{
								buffer &= 0x07;
							}
								
					}
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;

	//进水超时  和 保水超时故障处理
	//保水超时逻辑

	
	if(sys_flag.Error_Code)//针对热保故障和水位逻辑错误，不补水
		{
			Feed_Main_Pump_OFF();	
			Second_Water_Valve_Close();
			 return 0;
		}

	 if(sys_data.Data_10H == SYS_MANUAL)   //手动模式补水自理
	 		return 0;



	  if(sys_data.Data_10H == SYS_IDLE)
	 	{
	 		
	 		if(sys_flag.last_blow_flag)
	 			{
	 				/*2023年3月10日09:20:37 由超高信号，改成中信号，防止水过多*/
	 				if( IO_Status.Target.water_mid == WATER_LOSE)
	 					sys_flag.Force_Supple_Water_Flag = OK;

					if( IO_Status.Target.water_mid == WATER_OK)
						sys_flag.Force_Supple_Water_Flag = FALSE;
					
	 			}
			else
				{ 
					//修正后吹扫结束后，没补到中水位，水泵还在工作的事项
					
					sys_flag.Force_Supple_Water_Flag = FALSE;
					
					
				}
			if(sys_flag.Force_Supple_Water_Flag) //强制补水标志，则强制打开补水阀，
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
			  

	 if(sys_flag.Force_Supple_Water_Flag) //强制补水标志，则强制打开补水阀，
		{
			Feed_Main_Pump_ON();
			Second_Water_Valve_Open();
			return 0;
		}
	
/**************************************************************/
	
	 
	//这两根水位检测针是不能坏的，否则会造成空烧
	if(sys_flag.Error_Code == 0)
		{
	 		if(IO_Status.Target.water_mid == WATER_LOSE || IO_Status.Target.water_protect == WATER_LOSE)//中水位信号丢失，必须补水
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



//用于手动模式一些功能的处理
uint8 Manual_Realys_Function(void)
{
	
	
	
	
	//补水超时提示，防止水无限制再补
	
	
	return 0;
}

void Check_Config_Data_Function(void)
{
	float ResData = 0;
	
	//1、 前吹扫检查30--120s
	Sys_Admin.First_Blow_Time = *(uint32 *)(FIRST_BLOW_ADDRESS);  //预吹扫时间
	if(Sys_Admin.First_Blow_Time > 300000 ||Sys_Admin.First_Blow_Time < 30000) //如果超出设定范围，将值追回
		Sys_Admin.First_Blow_Time =30000 ;
	
	//2、 后吹扫检查30--120s	
	Sys_Admin.Last_Blow_Time =  *(uint32 *)(LAST_BLOW_ADDRESS);//后吹扫时间
	if(Sys_Admin.Last_Blow_Time > 300000 ||Sys_Admin.Last_Blow_Time < 30000) //如果超出设定范围，将值追回
		Sys_Admin.Last_Blow_Time =30000 ;
	
	//3、 点火功率20--35%
	Sys_Admin.Dian_Huo_Power =  *(uint32 *)(DIAN_HUO_POWER_ADDRESS);  //点火功率
	if(Sys_Admin.Dian_Huo_Power > Max_Dian_Huo_Power ||Sys_Admin.Dian_Huo_Power < Min_Dian_Huo_Power) //如果超出设定范围，将值追回
		Sys_Admin.Dian_Huo_Power =25 ;
	

	//4、 最大可运行功率检查30--100%
	if(Sys_Admin.Max_Work_Power > 100 ||Sys_Admin.Max_Work_Power < 20)
		Sys_Admin.Max_Work_Power = 100;

	if(Sys_Admin.Max_Work_Power < Sys_Admin.Dian_Huo_Power)
		Sys_Admin.Max_Work_Power = Sys_Admin.Dian_Huo_Power;


	Sys_Admin.Fan_Speed_Check =  *(uint32 *)(FAN_SPEED_CHECK_ADDRESS);  //风速检测是否开启
	if(Sys_Admin.Fan_Speed_Check > 1)
		Sys_Admin.Fan_Speed_Check = 1; //默认是检测风速的
	
		
	Sys_Admin.Danger_Smoke_Value =  *(uint32 *)(DANGER_SMOKE_VALUE_ADDRESS); //对排烟温度报警保护
	if(Sys_Admin.Danger_Smoke_Value > 2000 && Sys_Admin.Danger_Smoke_Value < 600)
		Sys_Admin.Danger_Smoke_Value = 800;
	
	

	

	sys_config_data.zhuan_huan_temperture_value = *(uint32 *)(ZHUAN_HUAN_TEMPERATURE);
	if(sys_config_data.zhuan_huan_temperture_value < 10|| sys_config_data.zhuan_huan_temperture_value >= Sys_Admin.DeviceMaxPressureSet)
		sys_config_data.zhuan_huan_temperture_value = 55; //如果超限，默认5.5公斤

	if(sys_config_data.Auto_stop_pressure >= Sys_Admin.DeviceMaxPressureSet)
		sys_config_data.Auto_stop_pressure = Sys_Admin.DeviceMaxPressureSet - 5; //如果超限，默认则比额定压力少0.05Mpa
	

	Sys_Admin.DeviceMaxPressureSet = *(uint32 *)(DEVICE_MAX_PRESSURE_SET_ADDRESS);
	if(Sys_Admin.DeviceMaxPressureSet > 250) //25公斤的需要另外制作
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

//	LCD10D.DLCD.Air_Power = 0;  //在PWM调节过程中，自动修改

	LCD10D.DLCD.Paiwu_State = Switch_Inf.pai_wu_flag;

	LCD10D.DLCD.lianxuFa_State = Switch_Inf.LianXu_PaiWu_flag;


	LCD10D.DLCD.Flame_State = sys_flag.flame_state;

	LCD10D.DLCD.Air_Speed  = sys_flag.Fan_Rpm;  //风机转速显示
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
	LCD10D.DLCD.Paiwu_Flag = sys_flag.Paiwu_Flag ;  //排污标志同步

	LCD10D.DLCD.Air_State = Switch_Inf.air_on_flag ; 
	LCD10D.DLCD.lianxuFa_State = Switch_Inf.LianXu_PaiWu_flag;   //风机和连续排污阀状态
	
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
	LCD10D.DLCD.System_Version  = Soft_Version ; //系统版本号
	LCD10D.DLCD.Device_Style = Sys_Admin.Device_Style  ;  //设备类型的选择
	
	ResData = Sys_Admin.DeviceMaxPressureSet;													
	LCD10D.DLCD.Max_Pressure = ResData / 100;  //额定蒸汽压力的显示

	LCD10JZ[2].DLCD.YunXu_Flag = SlaveG[2].Key_Power;
	LCD10JZ[1].DLCD.YunXu_Flag = SlaveG[1].Key_Power;
	
}



void Fan_Speed_Check_Function(void)
{
	
	//Fan_Rpm = (1000/(2* fan_count)) / 3(每个周期3转) *60秒 = 100000 / sys_flag.Fan_count


		 
		static uint8 Pulse = 3;    //两吨风机每转5个脉冲
		 
		uint32 All_Fan_counts = 0;
			
		
			//G1G170   0.5T风机	每转3个脉冲，  Ametek  0.5T风机 每转2个脉冲
			//G3G250   1T风机的参数 每转3个脉冲
			//G3G315   2T风机的参数  每转 5个脉冲
			if(sys_flag.Rpm_1_Sec)
				{
					sys_flag.Rpm_1_Sec = FALSE;

			

					//走了PB0上升沿中断，计数
					if(Sys_Admin.Fan_Pulse_Rpm >=  10  || Sys_Admin.Fan_Pulse_Rpm == 0)
							Sys_Admin.Fan_Pulse_Rpm = 3; //做脉冲个数保护

					if(sys_flag.Fan_count > 0 )
						{
							sys_flag.Fan_Rpm = sys_flag.Fan_count * 60 / Sys_Admin.Fan_Pulse_Rpm;
							sys_flag.Fan_count = 0;
							
						}
						  //（周期数/5）  *60	，60是指60秒，其中5 是3吨每转5个脉冲
					else
						{
							sys_flag.Fan_count = 0;
							sys_flag.Fan_Rpm = 0;
						}
						
				
				}


	
}


/*用于经销商管理控制器可以运行的时间*/
uint8 Admin_Work_Time_Function(void)
{
	//涉及到的变量：Flash_Data.Admin_Work_Time，systmtime
	
	uint16 Now_Year = 0;
	uint16 Now_Month = 0;
	uint16 Now_Day = 0;

	uint16 Set_Year = 0;
	uint16 Set_Month = 0;
	uint16 Set_Day = 0;
	
	uint8 Set_Function = 0;  //用户设置的天数

	Set_Function = *(uint32 *)(ADMIN_WORK_DAY_ADDRESS); 

	//lcd_data.Data_40H = Set_Function>> 8;
	//lcd_data.Data_40L =Set_Function &0x00FF;//将天数刷新给LCD
	
	sys_flag.Lock_System = 0; //清除锁机命令
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
	uint8  Paiwu_Times = 3;  //4次降压排污
	//待机，检测压力小于半公斤时，自动排污一次
    
	uint8  Time = 15;//超过压力补水30秒

	uint8 	Ok_Value = 0;
	
	
		//1、 要锅炉要运行过，2、自动排污功能，要开启
		
		
				
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
											delay_sys_sec(35000); //低压排污最大时间
										}
										sys_flag.Pai_Wu_Idle_Index = 2;
										

										break;
								
								case 2:  //检测极低水位判定是否结束
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
												delay_sys_sec(60000);//给极低水位上水的时间
												 Pai_Wu_Door_Close();
											}

										
										if(sys_time_up)
											{
												sys_time_up = 0;
												sys_flag.Force_Supple_Water_Flag = FALSE;
												 Pai_Wu_Door_Close();
												 delay_sys_sec(60000); //给极低水位上水的时间
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
										Ok_Value = OK;  //结束自动排污程序
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
	//设备运行过程中使用该功能
	uint8  set_flag = 0;
	
		


	return set_flag;
}


uint8 PaiWu_Warnning_Function(void)
{
	//排污计时提醒2E       2F ,30，
	static uint16 Max_Time = 480 ;  //最大时间时8小时
	static uint16 Max_Value = 1439; //最大显示的时间为23:59
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
			lcd_data.Data_2EL = OK; //排污显示图标变色
			//sys_flag.Paiwu_Alarm_Flag  = OK;
		}
	else
		{
			lcd_data.Data_2EH = 0;
			lcd_data.Data_2EL = 0; //排污显示图标变色
			//sys_flag.Paiwu_Alarm_Flag  = FALSE;
		}

	
	if(Low_Flag == 0)
		sys_flag.Low_Count = 0;
	if(sys_time_inf.UnPaiwuMinutes > 1) //未排污时间超过10分钟，水位丢失后，会对排污时间清零
		{
			if (IO_Status.Target.water_protect == WATER_LOSE)
				{
					Low_Flag = OK;
					if(sys_flag.Low_Count >= 3)//如果低水位持续20秒后，排污完成
						{
							Low_Flag = 0;
							sys_time_inf.UnPaiwuMinutes = 0;
							Write_Second_Flash();
						}
				}
					
		}

	lcd_data.Data_2FH = 0;
	lcd_data.Data_2FL = sys_time_inf.UnPaiwuMinutes / 60; //未排污时间： 小时显示
	lcd_data.Data_30H = 0;
	lcd_data.Data_30L = sys_time_inf.UnPaiwuMinutes % 60; //未排污时间： 分钟显示

	
	return 0;
}


uint8 Special_Water_Supply_Function(void)
{
	static uint8 High_Flag = 0;
	//高温进水电磁阀 ，涉及到高温回水电磁阀
	 

	if(Sys_Admin.Special_Secs > 50)
 //最大时间间隔保护
		Sys_Admin.Special_Secs = 20;
	
	if(sys_flag.Error_Code)
		Special_Water_OFF();

	if(sys_data.Data_10H == 0 || sys_data.Data_12H) //待机或超压停炉模式
		Special_Water_OFF();


	if (IO_Status.Target.water_high== WATER_OK) //达到高水位，则关闭高温回水阀
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

			if(sys_flag.High_Lose_Count >= Sys_Admin.Special_Secs) //18秒
				{
					sys_flag.High_Lose_Count = Sys_Admin.Special_Secs; //锁住
					Special_Water_Open(); //打开高温回水电磁阀
				}
		}



	return 0 ;
}



//暂时不将该程序投入使用
uint8 WaterLevel_Unchange_Check(void)
{
	static uint8 LastState = 0;
	uint8  Water_Buffer = 0;

	//只在运行状态进行检测
	if(Sys_Admin.WaterUnchangeMaxTime >= 250) //默认最大则关闭检测
		return 0; 

	//程序功能，在运行过程中， 监测水位若长时间不变化，则报警
	Water_Buffer = lcd_data.Data_15L & 0x07; //取消对最高水位状态的监事


	if(LastState != Water_Buffer)
		{
			LastState = Water_Buffer;
			//取消时间
			//需要在首次进入运行程序前清零
			sys_flag.WaterUnsupply_Count = 0; //重新计时
		}


	if(sys_flag.WaterUnsupply_Count >= Sys_Admin.WaterUnchangeMaxTime)
		{
			//长时间未进行补水报警 ，默认是 150秒
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
		Sys_Admin.Water_Max_Percent = 99; //最大开度值不能超过100

	if(Sys_Admin.Water_Max_Percent < 25)
		Sys_Admin.Water_Max_Percent = 25; //限制最小开度值不能低于25
		
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


//针对运行过程中，超高水位的探针挂水的问题
		if(sys_data.Data_10H == 2)
			{
				//如果高水位没有信号
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//解决运行中，超高水位显示不准的问题
						if (IO_Status.Target.water_shigh== WATER_OK)
								buffer &= 0x07;
					}
			}
		else
			{
			
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;

	//进水超时  和 保水超时故障处理
	//保水超时逻辑
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


	//待机 和 手动模式下，不进行补水动作
	 if(sys_data.Data_10H == SYS_MANUAL )  
	 	  return 0;
	
	
	

	

	 if(sys_flag.Error_Code)//针对热保故障和水位逻辑错误，不补水
		{
			sys_flag.Water_Percent = 0;
			 return 0;
		}

	
	 	

	
//取消强制补水的措施
	
	 if(sys_flag.Force_Supple_Water_Flag) //强制补水标志，则强制打开补水阀，
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
		case 0://没有水时
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 0;

				break;
		case 1://低水位时
				if(Old_State == 2)
					New_Percent++;
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 1;

				break;

		case 2://中水位时
				if(New_Percent < Min_Percent)
					New_Percent = Min_Percent;

				if(New_Percent > Max_Percent)
					New_Percent = Max_Percent;
					
				sys_flag.Water_Percent = New_Percent;

				Old_State = 2;

				break;

		case 3://高水位时
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
	uint16 Open_Time = 0; //连续排污阀的实际开启时间设定，精确到0.1s

	uint16 Cong_Work_Time = 0;
	static uint8 Time_Ok = 0;  //工作时间到的标志，静态变量
	
	//连续排污开启标志，连续排污时间间隔，连续排污开启时间秒

	//测试在4公斤压力下，排污2秒，排水量在1L

	//Sys_Admin.LianXu_PaiWu_Enabled 
	//Sys_Admin.LianXu_PaiWu_DelayTime //精度到0.1小时
	//Sys_Admin.LianXu_PaiWu_OpenSecs //精度到1s

	//ADouble5[1].True.LianXuTime_H，从机当前已经工作的时间
	//************需要考虑主从机同时排污，怎么处理，错峰三分钟，从机按照原来时间设定，主机延迟三分钟，要不要间隔排污，
	//需要把从机的连续工作时间，同步到主机来

	//排污需要跟水泵补水联动才能打开

	//sys_flag.LianXu_1sFlag
	Dealy_Time = Sys_Admin.LianXu_PaiWu_DelayTime * 1 * 60; //0.1h * min  * 60sec/min
	

	Open_Time = Sys_Admin.LianXu_PaiWu_OpenSecs * 10; //换算成100ms单位，方便精准控制时间

	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)
		{
			//相变机组，该继电器用于真空泄压
			return 0 ;
		}
	
	if(sys_data.Data_10H == 3)
		return 0;
	

	//运行状态下有火焰的标志，才对工作的时间进行统计
	if(sys_data.Data_10H == 2)
		{
			if(sys_flag.flame_state)
				if(sys_flag.LianXu_1sFlag)
					{
						sys_flag.LianxuWorkTime ++;//秒计
						sys_flag.LianXu_1sFlag = 0;
					}
		}


	 

	//检查工作的的时间，有没有达到设定的值
	if(sys_flag.LianxuWorkTime >= Dealy_Time)
		{
			sys_flag.LianxuWorkTime = 0; //变量清零
			sys_flag.Lianxu_OpenTime  = 0;
		
			Time_Ok = OK;//设置连续排污标志
		}

	//工作的时间到，且处于补水状态，则打开连续排污阀，检查阀门开启的时间
	if(Time_Ok)
		{
			
			if(sys_flag.Lianxu_OpenTime < Open_Time)
				{
					 if( Switch_Inf.water_switch_flag)//  跟变频补水联动还是跟水泵联动，跟启动信号联动
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
					Time_Ok = FALSE; //时间到的标志清零
					
				}
			
		}
	else
		{
			sys_flag.Lianxu_OpenTime  = 0; //清除上次使用的变量标志
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
	static uint16 Old_Pressure = 0; //用于保存上个阶段的蒸汽值
	uint16 New_Pressure =0;
	static uint16 TimeCount = 0;
	uint8 Chazhi = 0;

	//
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3) 
		{
			//相变机组使用内侧压力作为追踪目标
			New_Pressure = Temperature_Data.Inside_High_Pressure;  //采用一次侧的压力作为目标
		}
	else
		{
			New_Pressure = Temperature_Data.Pressure_Value;   //二次侧压力作为追踪目标
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
			else   //没有火焰时，状态归零
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
	//涉及到的变量：Flash_Data.Admin_Work_Time，systmtime
	Date Now;
	Date Set;

	

	Now.iYear = LCD10D.DLCD.Year;
	Now.iMonth = LCD10D.DLCD.Month;
	Now.iDay = LCD10D.DLCD.Day;     //换算成屏幕的时间

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
	
	if(Sys_Admin.Device_Style == 1 || Sys_Admin.Device_Style == 3)  //针对相变机组的类型
		{
			if(sys_data.Data_10H == 2)
				{
					if(Temperature_Data.Inside_High_Pressure >=Protect_Pressure) //大于15斤，则直接报警
						{
							if(sys_flag.Error_Code == 0 )
								sys_flag.Error_Code  = Error20_XB_HighPressureYabian_Bad;
						}

					
				}


			switch (sys_data.Data_10H)
				{
					case 0:  //待机状态,若出现极低水位，则立即报警
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
					case 2://运行状态
							if(sys_flag.flame_state == OK)
								{
									//出现极低水位，并且本体温度超过230度，则停机转后吹扫，连续4次后，则报警
									if(IO_Status.Target.XB_WaterLow == FALSE && sys_flag.Protect_WenDu >= 200)
										{
											if(sys_flag.XB_WaterLow_Flag == 0)
												{
													sys_flag.XB_WaterLow_Flag = OK;
													sys_flag.XB_WaterLow_Count = 0;
												}

											if(sys_flag.XB_WaterLow_Count > 10)  //低水位持续15秒
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
															//转入异常状态
															sys_data.Data_12H = 5; //  
															Abnormal_Events.target_complete_event = 1;//异常事件记录
														}
												}
											
										}
									else
										{
											//当温度或水位任何一项不满足，则清0
												sys_flag.XB_WaterLow_Flag = 0;
												sys_flag.XB_WaterLow_Count = 0;

												if(sys_flag.XB_WaterLowAB_Count)
													{
														//如果正常燃烧半小时候，自动对熄灭记录清零
														if(sys_flag.XB_WaterLowAB_RecoverTime >= 1800)//30min正常运行，则认为是正常的
															sys_flag.XB_WaterLowAB_Count = 0;
													}
										}
								}
							else
								{
									//设备在运行状态，防止刚处理完异常，水位还没稳定，得区分两种情况
									//设备在前吹扫过程中，检测到缺水，也是直接报警
								
									if(sys_data.Data_12H == 0)
										{
											//非异常状态，则直接报警
											if(IO_Status.Target.XB_WaterLow == FALSE)
												{
													if(sys_flag.XB_WaterLow_Flag == 0)
														{
															sys_flag.XB_WaterLow_Flag = OK;
															sys_flag.XB_WaterLow_Count = 0;
														}

													if(sys_flag.XB_WaterLow_Count > 10) //等待10秒
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
											//超压停炉状态，直接不检测水位，将标志清零
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
						sys_flag.Error_Code = Error21_XB_HighPressureYAKONG_Bad; //蒸汽压力超出安全范围报警	
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


//针对运行过程中，超高水位的探针挂水的问题
		//针对运行过程中，超高水位的探针挂水的问题
		if(sys_data.Data_10H == 2)
			{
				//如果高水位没有信号
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//解决运行中，超高水位显示不准的问题
						if (IO_Status.Target.water_shigh== WATER_OK)
								buffer &= 0x07;
					}
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;
	
	

	//待机 和 手动模式下，不进行补水动作
	 if(sys_data.Data_10H == SYS_MANUAL)  
	 		return 0;


	//在正常状态下检查进水电磁阀的开启或关闭
	
	if(sys_flag.Address_Number == 0)
		{
			//主机可以自己开进水电磁阀，从机得在串口中收到命令开
			if(Water_State.ZCommand)
				Second_Water_Valve_Open();
			else
				Second_Water_Valve_Close();
		}
	
	 

	if(sys_flag.Error_Code )//针对热保故障和水位逻辑错误，不补水
		{
			Water_State.ZSignal = FALSE;
			 return 0;
		}

	 //待机过程中补水的问题**************************

	
	 if(sys_data.Data_10H == SYS_IDLE)
	 	{
	 		
	 		if(sys_flag.last_blow_flag)
	 			{
	 				/*2023年3月10日09:20:37 由超高信号，改成中信号，防止水过多*/
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
	 	
				
	
//取消强制补水的措施
	
	 if(sys_flag.Force_Supple_Water_Flag) //强制补水标志，则强制打开补水阀，
		{
			
			Water_State.ZSignal = OK;
			
			return 0;
		}
		
	
/**************************************************************/


	if(sys_flag.Error_Code == 0)
		{
	 		if(IO_Status.Target.water_mid == WATER_LOSE && IO_Status.Target.water_high == WATER_LOSE)//中水位信号和高水位信号丢失，必须补水
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
	static uint16 Time_Value = 900 ; //按照700ms计算


	 if(sys_data.Data_10H == SYS_MANUAL)
	 	{
			if(Water_State.Zstate_Flag || Water_State.Cstate_Flag)
				Logic_Pump_On();
			else
				Logic_Pump_OFF();

			return 0;
	 	}

	
	//以开或关的信号状态进行检索
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
			case 0: //主从都是关请求信号
					//首先得检查水泵是否为开状态
					if(Switch_Inf.water_switch_flag)
						{
							//给出水泵关闭的动作信号
							Water_State.Pump_Signal = FALSE;
							Water_State.PUMP_Close_Time = 0;
						}
					Water_State.Pump_Signal = FALSE;

					//水泵关闭时间到，根据主从开关的状态关闭主从的进水电磁阀
					if(Water_State.PUMP_Close_Time >= Time_Value)  //按照毫秒计算
						{
							Water_State.ZCommand = FALSE; //给出主从电磁阀关闭指令
							Water_State.CCommand = FALSE;
						}

					break;
			case 1: //主开请求信号，从关闭请求信号
					//有限处理关闭请求信号，然后再处理开信号
					if(Water_State.Cstate_Flag == OK)
						{
							//从有关的请求信号，且从的电磁阀属于开的状态时,先关闭水泵
							if(Switch_Inf.water_switch_flag)
								Water_State.PUMP_Close_Time = 0;//先清楚时间
							Water_State.Pump_Signal = FALSE;
							if(Water_State.PUMP_Close_Time >= Time_Value)
								{
									Water_State.CCommand = FALSE;
									Water_State.ZC_Open_Time = 0; //对电磁阀开启的时间重新计算，防止瞬开瞬关
								}
								
						}
					else
						{
							//从机已经关闭，主机请求开启信号
							if(Water_State.Zstate_Flag == OK)
								{
									//主电磁阀已经开启
									if(Water_State.ZC_Open_Time >= Time_Value)
										Water_State.Pump_Signal = OK;//开启水泵信号
								}
							else
								{
									//当前主机的电磁阀还没有打开，则打开
									Water_State.ZCommand = OK;
									Water_State.ZC_Open_Time = 0;
								}
						}

					break;
			case 2: //从机有开启信号，主机是关闭信号
					if(Water_State.Zstate_Flag == OK)
						{
							//主有关的请求信号，且主的电磁阀属于开的状态时,先关闭水泵
							if(Switch_Inf.water_switch_flag)
								Water_State.PUMP_Close_Time = 0;//先清楚时间
							Water_State.Pump_Signal = FALSE;
							if(Water_State.PUMP_Close_Time >= Time_Value)
								{
									Water_State.ZCommand = FALSE;
									Water_State.ZC_Open_Time = 0; //对电磁阀开启的时间重新计算，防止瞬开瞬关
								}
								
						}
					else
						{
							//主机已经关闭，从机请求开启信号
							if(Water_State.Cstate_Flag == OK)
								{
									//主电磁阀已经开启
									if(Water_State.ZC_Open_Time >= Time_Value)
										Water_State.Pump_Signal = OK;//开启水泵信号
								}
							else
								{
									//当前主机的电磁阀还没有打开，则打开
									Water_State.CCommand = OK;
									Water_State.ZC_Open_Time = 0;
								}
						}

					break;
			case 3: //主机和从机都在请求开的信号
					//需要确认水泵是否已经打开
					Water_State.ZCommand = OK; //给出主从电磁阀开启指令
					Water_State.CCommand = OK;

					if(Water_State.ZC_Open_Time >= Time_Value)
							Water_State.Pump_Signal = OK;//开启水泵信号
				
					break;

			default:

					break;
		}


	//主机情况
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
				//给从机发命令打开补水电磁阀
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
	uint8 Max_Percent = 40;  //2023年3月5日10:37:32  由32调整为 40
	uint8 Min_Percent = 18;
	uint8 Jump_Index = 0;

	if(Sys_Admin.Water_Max_Percent > 99)
		Sys_Admin.Water_Max_Percent = 99; //最大开度值不能超过100

	if(Sys_Admin.Water_Max_Percent < 25)
		Sys_Admin.Water_Max_Percent = 25; //限制最小开度值不能低于25
		
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


//针对运行过程中，超高水位的探针挂水的问题
		if(sys_data.Data_10H == 2)
			{
				//如果高水位没有信号
				if (IO_Status.Target.water_high== WATER_LOSE)
					{
						//解决运行中，超高水位显示不准的问题
						if (IO_Status.Target.water_shigh== WATER_OK)
								buffer &= 0x07;
					}
			}


		lcd_data.Data_15L = buffer;
		LCD10D.DLCD.Water_State = buffer;

	//进水超时  和 保水超时故障处理
	//保水超时逻辑
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


	//待机 和 手动模式下，不进行补水动作
	 if(sys_data.Data_10H == SYS_MANUAL )  
	 		return 0;
	
	//在正常状态下检查进水电磁阀的开启或关闭
	if(sys_flag.Address_Number == 0)
		{
			//if(Water_State.ZCommand)
			//	Second_Water_Valve_Open();
			//else
			//	Second_Water_Valve_Close();
		}
	
	

	

	 if(sys_flag.Error_Code)//针对热保故障和水位逻辑错误，不补水
		{
			Water_State.ZSignal = FALSE;
			sys_flag.Water_Percent = 0;
			 return 0;
		}

	 if(sys_data.Data_10H == SYS_IDLE)
	 	{
	 		//在后吹扫过程中还需要补水
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
	 	

	
//取消强制补水的措施
	
	 if(sys_flag.Force_Supple_Water_Flag) //强制补水标志，则强制打开补水阀，
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
		case 0://没有水时
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 0;

				break;
		case 1://低水位时
				if(Old_State == 2)
					New_Percent++;
				sys_flag.Water_Percent = Max_Percent;
				Old_State = 1;

				break;

		case 2://中水位时
				if(New_Percent < Min_Percent)
					New_Percent = Min_Percent;

				if(New_Percent > Max_Percent)
					New_Percent = Max_Percent;
					
				sys_flag.Water_Percent = New_Percent;

				Old_State = 2;

				break;

		case 3://高水位时
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


