/**
  ******************************************************************************
  * @file    usr_c210.c
  * @author  heic
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   usr_c210 WIFI模组 接口定义及应用函数
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "main.h"

extern uint16 USART2_RX_STA;




 WWW_DATA Talk;






 ///////////////////////////////////////服务器命令解析////////////////////////////////// 	 
 //功能：对服务器发来的数据进行分析
 //输入参数
 //输出参数
 //无
 //////////////////////////////////////////////////////////////////////////////////////////////    
 void Server_Cmd_Analyse(void)
 {
 	uint8 Cmd = 0;
	uint8 index = 0;
	uint16 Address = 0;
	uint16 Data = 0;
	//uint16 check_sum = 0;
	
	
	Cmd = U1_Inf.RX_Data[1];
	Address = U1_Inf.RX_Data[2] *256+ U1_Inf.RX_Data[3];
	
	switch (Cmd)
		{
			case 03:  //00 03 01 F3 00 0F F5 D0		 
					if(Address == 0x01F3)
						{
							sys_flag.Access_Data_Flag = OK;
							Mcu_mail_Wifi();
						}
					break;
		
			case 06://单个寄存器的写入
					
					switch (Address)
						{
							case 0x0063:////地址40100 为控制字 控制启停寄存器
										Data = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];
										if(Data == 0x0022)//待机命令
										{
											if(sys_data.Data_10H  == 2)//运行状态 执行关闭命令
												sys_close_cmd();

										}

										if(Data == 0x0033)//开机命令
										{
											if(sys_data.Data_10H  == 0)//待机状态 执行启动命令，不能有故障哦
												sys_start_cmd();
										}
										//返回确认码
											for(index = 0; index < U1_Inf.RX_Length; index++ )
												U1_Inf.TX_Data[index] = U1_Inf.RX_Data[index];
											
											//Usart_SendStr_length(USART1,U1_Inf.TX_Data,U1_Inf.RX_Length);

											Mcu_mail_Wifi();
										break;

							case 0x0064://地址40101 故障复位寄存器
										
										
										sys_flag.Error_Code = 0;
										Beep_Data.beep_start_flag = 0;//消音标志
										sys_flag.Lock_Error = 0; //解除故障锁定

										for(index = 0; index < U1_Inf.RX_Length; index++ )
												U1_Inf.TX_Data[index] = U1_Inf.RX_Data[index];
											
										//Usart_SendStr_length(USART1,U1_Inf.TX_Data,U1_Inf.RX_Length);
										Mcu_mail_Wifi();
										break;

							case 0x00C7:
										Data = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];
										if(Data != Sys_Admin.Access_Data_Mode)
											{
												if(Data <= 1)
													{
														Sys_Admin.Access_Data_Mode = Data;
														Write_Admin_Flash();
														Mcu_mail_Wifi();
													}
												
											}
											
										break;

							case 0x012C:
										
										
										break;

							default:
									break;
						}
					
					break;

			case 16: //40110--40115多个寄存器的写入
					if(Address == 0x006D)
					if(U1_Inf.RX_Data[5] == 0x05 &&U1_Inf.RX_Data[6] == 10)
						{
							//Data = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];//40110 未使用
							//Data = U1_Inf.RX_Data[10] *256+ U1_Inf.RX_Data[11];//40114 未使用

							sys_config_data.zhuan_huan_temperture_value = U1_Inf.RX_Data[9] * 256 +U1_Inf.RX_Data[10]  ; 
							sys_config_data.Auto_stop_pressure = U1_Inf.RX_Data[11] * 256 + U1_Inf.RX_Data[12]  ; 
							sys_config_data.Auto_start_pressure = U1_Inf.RX_Data[13] * 256 + U1_Inf.RX_Data[14]; 

							if(sys_config_data.zhuan_huan_temperture_value < 10)
								sys_config_data.zhuan_huan_temperture_value = 10;

							
							if(sys_config_data.zhuan_huan_temperture_value > Max_Pressure_Set)
									sys_config_data.zhuan_huan_temperture_value = Max_Pressure_Set;

							//2、 对停炉压力进行检测，要大于目标压力至少0.5kg,但不能超过8kg
								if(sys_config_data.Auto_stop_pressure <= sys_config_data.zhuan_huan_temperture_value)
									sys_config_data.Auto_stop_pressure = sys_config_data.zhuan_huan_temperture_value + 10;

								if(sys_config_data.Auto_stop_pressure > Stop_Pressure_Set)
									sys_config_data.Auto_stop_pressure = Stop_Pressure_Set;

							//3、对起炉压力进行保护
								if(sys_config_data.Auto_start_pressure >= sys_config_data.zhuan_huan_temperture_value)
									sys_config_data.Auto_start_pressure = 10;

								if(sys_config_data.Auto_start_pressure < 10)
									sys_config_data.Auto_start_pressure = 10;

								
								Write_Internal_Flash();//对相关参数进行保存
								
								copy_to_lcd();

							   	Mcu_mail_Wifi();
						}
					break;


			default:
					//无效指令
					break;
		}
 }

 
 //MCU定时往WIFI发送数据


 uint8  New_Server_Cmd_Analyse(void)
 {
 	uint8 Cmd = 0;
	uint8 index = 0;
	uint8 Length = 0;
	uint16 Value = 0;
	uint16 Address = 0;
	uint16 Data = 0; 
	uint8  Device_ID = 1; //身份地址，不能错 ************88
	float  value_buffer = 0;
	uint16 check_sum = 0;
	uint16 Data1 = 0;
	uint16 Data2 = 0;
	uint16 Data3 = 0;
	uint32 Data32_Buffer = 0;
	uint32 Int_CharH = 0;
	uint32 Int_CHarL = 0;
	uint8 Code[6] = {0};
	


	 //Float_Int.value  用于单精度的转换

	
	Cmd = U1_Inf.RX_Data[1];
	Address = U1_Inf.RX_Data[2] *256+ U1_Inf.RX_Data[3];
	Length = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];
	Value = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];   //06格式下，value = length

	if(U1_Inf.RX_Data[0] > 1)
		{
			//当查询地址大于1时，自动切换为对方询问的地址
			Device_ID = Sys_Admin.ModBus_Address;

			if(U1_Inf.RX_Data[0] != Sys_Admin.ModBus_Address)
				{
					return 0;   //非本机地址直接退出
				}
		}
	
	
	switch (Cmd)
		{
			case 03:  //00 03 01 F3 00 0F F5 D0		

					switch (Address)
						{
							case 100:
									if(Address == 100 && Length == 11)  //这边的数据长度也根据总变化，不是字节长度的变化
										{
											U1_Inf.TX_Data[0] = Device_ID;
											U1_Inf.TX_Data[1] = 0x03;// 
											U1_Inf.TX_Data[2] = 22; //数据长度为6， 根据情况改变***
									
											U1_Inf.TX_Data[3] = 0x00;
											U1_Inf.TX_Data[4] = sys_data.Data_10H;// 1当前的运行状态

											U1_Inf.TX_Data[5] = Temperature_Data.Pressure_Value >> 8 ;
											U1_Inf.TX_Data[6] = Temperature_Data.Pressure_Value & 0x00FF;//2当前蒸汽的压力值当前的水位状态
											
											U1_Inf.TX_Data[7] = 0x00;
											U1_Inf.TX_Data[8] = Temperature_Data.steam_value;//3当前蒸汽的温度

											U1_Inf.TX_Data[9] = 0x00;
											U1_Inf.TX_Data[10] = LCD10D.DLCD.Water_State;//4当前水位的状态

											U1_Inf.TX_Data[11] = 0x00;
											U1_Inf.TX_Data[12] = Temperature_Data.Smoke_Tem /10;//5烟气的温度

											U1_Inf.TX_Data[13] = sys_flag.Protect_WenDu >> 8;
											U1_Inf.TX_Data[14] = sys_flag.Protect_WenDu & 0x00FF;//6内部烟气的温度

											U1_Inf.TX_Data[15] = 0;
											U1_Inf.TX_Data[16] = sys_flag.Error_Code;//7故障代码

											U1_Inf.TX_Data[17] = 0;
											U1_Inf.TX_Data[18] = Switch_Inf.air_on_flag;//8风机运行状态

											U1_Inf.TX_Data[19] = 0;
											U1_Inf.TX_Data[20] = LCD10D.DLCD.Air_Power;//9风机功率

											U1_Inf.TX_Data[21] = 0;
											U1_Inf.TX_Data[22] = Switch_Inf.water_switch_flag;//10水泵状态

											U1_Inf.TX_Data[23] = 0;
											U1_Inf.TX_Data[24] = sys_flag.flame_state;//11火焰状态
											

											
										 
											check_sum  = ModBusCRC16(U1_Inf.TX_Data,27);   //这个根据总字节数改变
											U1_Inf.TX_Data[25]  = check_sum >> 8 ;
											U1_Inf.TX_Data[26]  = check_sum & 0x00FF;
											
											Usart_SendStr_length(USART1,U1_Inf.TX_Data,27);

											
											
										}

									break;
							case 120:  //读取蒸汽压力浮点数
									value_buffer  = Temperature_Data.Pressure_Value;
									Float_Int.value = value_buffer / 100;
									if(Address == 120 && Length == 2)
										{
											U1_Inf.TX_Data[0] = Device_ID;
											U1_Inf.TX_Data[1] = 0x03;// 
											U1_Inf.TX_Data[2] = 4; //数据长度为 ， 根据情况改变***

											U1_Inf.TX_Data[3] = Float_Int.byte4.data_HH;
											U1_Inf.TX_Data[4] = Float_Int.byte4.data_HL;// 1当前的运行状态

											U1_Inf.TX_Data[5] = Float_Int.byte4.data_LH;
											U1_Inf.TX_Data[6] = Float_Int.byte4.data_LL;

											check_sum  = ModBusCRC16(U1_Inf.TX_Data,9);   //这个根据总字节数改变
											U1_Inf.TX_Data[7]  = check_sum >> 8 ;
											U1_Inf.TX_Data[8]  = check_sum & 0x00FF;
											
											Usart_SendStr_length(USART1,U1_Inf.TX_Data,9);
											
										}

									break;
							case 150:  //蒸汽压力参数设置
									
										if( Length == 3)  
											{
												U1_Inf.TX_Data[0] = Device_ID;
												U1_Inf.TX_Data[1] = 0x03;// 
												U1_Inf.TX_Data[2] = 6; //数据长度为 ， 根据情况改变***

												U1_Inf.TX_Data[3] = sys_config_data.zhuan_huan_temperture_value >> 8;
												U1_Inf.TX_Data[4] = sys_config_data.zhuan_huan_temperture_value & 0x00FF;// 1目标蒸汽压力

												U1_Inf.TX_Data[5] = sys_config_data.Auto_stop_pressure >> 8;
												U1_Inf.TX_Data[6] = sys_config_data.Auto_stop_pressure & 0x00FF;

												U1_Inf.TX_Data[7] = sys_config_data.Auto_start_pressure >> 8;
												U1_Inf.TX_Data[8] = sys_config_data.Auto_start_pressure & 0x00FF;

												check_sum  = ModBusCRC16(U1_Inf.TX_Data,11);   //这个根据总字节数改变
												U1_Inf.TX_Data[9]  = check_sum >> 8 ;
												U1_Inf.TX_Data[10]  = check_sum & 0x00FF;
												
												Usart_SendStr_length(USART1,U1_Inf.TX_Data,11);
											}
									break;

							case 160:
									if( Length == 8)  
											{
												U1_Inf.TX_Data[0] = Device_ID;
												U1_Inf.TX_Data[1] = 0x03;// 
												U1_Inf.TX_Data[2] = 16;
												
												U1_Inf.TX_Data[3] = 0;
												U1_Inf.TX_Data[4] = sys_flag.Wifi_Lock_System;

												U1_Inf.TX_Data[5] = sys_flag.wifi_Lock_Year >> 8;
												U1_Inf.TX_Data[6] = sys_flag.wifi_Lock_Year & 0x00FF;

												U1_Inf.TX_Data[7] = 0;
												U1_Inf.TX_Data[8] = sys_flag.wifi_Lock_Month;

												U1_Inf.TX_Data[9] = 0;
											//	sys_flag.Wifi_Lock_Day = 0;
												U1_Inf.TX_Data[10] = sys_flag.Wifi_Lock_Day;

												Data32_Buffer = Char_to_Int_6(Login_D.Login_KeyH[6],Login_D.Login_KeyL[6]);
												Data1 = Data32_Buffer >> 16;
												U1_Inf.TX_Data[11] = Data1 >> 8;
												U1_Inf.TX_Data[12] = Data1 & 0x00FF;
												Data1 =Data32_Buffer & 0x0000FFFF;
												U1_Inf.TX_Data[13] = Data1 >> 8;
												U1_Inf.TX_Data[14] = Data1 & 0x00FF;

												Data32_Buffer = Char_to_Int_6(Login_D.Login_KeyH[5],Login_D.Login_KeyL[5]);
												Data1 = Data32_Buffer >> 16;
												U1_Inf.TX_Data[15] = Data1 >> 8;
												U1_Inf.TX_Data[16] = Data1 & 0x00FF;
												Data1 =Data32_Buffer & 0x0000FFFF; 
												U1_Inf.TX_Data[17] = Data1 >> 8;
												U1_Inf.TX_Data[18] = Data1 & 0x00FF;

												check_sum  = ModBusCRC16(U1_Inf.TX_Data,21);   //这个根据总字节数改变
												U1_Inf.TX_Data[19]  = check_sum >> 8 ;
												U1_Inf.TX_Data[20]  = check_sum & 0x00FF;
												
												Usart_SendStr_length(USART1,U1_Inf.TX_Data,21);
											}


									break;
								

							default :
									break;
						}
					

					//针对压力做浮点数转换
					
					break;
			case 06://单个寄存器的写入
					
					switch (Address)
						{
							

							case 0x0064://设备启动或关闭，根据值来区分
										 
										if(sys_data.Data_10H  == 0  && Value == 2)//待机状态 执行启动命令，不能有故障哦
											{
												sys_start_cmd();
												ModuBusRTU_WriteResponse(Address,Value);
												
												
											}


										if(sys_data.Data_10H == 2  && Value == 0)//待机状态 执行启动命令，不能有故障哦
											{
												sys_close_cmd();
												
												ModuBusRTU_WriteResponse(Address,Value);
												
											}
												
										
										break;

							case 0x006A:  //故障复位处理
										
										sys_flag.Error_Code = 0;
										Beep_Data.beep_start_flag = 0;//消音标志
										sys_flag.Lock_Error = 0; //解除故障锁定

										ModuBusRTU_WriteResponse(Address,Value);

										
											
										break;

							case 150:  //目标蒸汽压力设置
										
										if(Value < sys_config_data.Auto_stop_pressure)
											{
												if(Value >= 20)
													{
														sys_config_data.zhuan_huan_temperture_value = Value;
														Write_Internal_Flash();//对相关参数进行保存
														ModuBusRTU_WriteResponse(Address,Value);
														
														
													}
											}
										
										break;
							case 151:  //停机蒸汽压力设置
										
												if(Value < Sys_Admin.DeviceMaxPressureSet)
													{
														if(Value > sys_config_data.zhuan_huan_temperture_value)
															{
																sys_config_data.Auto_stop_pressure = Value;
																Write_Internal_Flash();//对相关参数进行保存
																ModuBusRTU_WriteResponse(Address,Value);
																
															}
													}

											
										
										break;
							case 152: //启动蒸汽压力
										
												if(Value < sys_config_data.Auto_stop_pressure)
													{
														sys_config_data.Auto_start_pressure = Value;
														Write_Internal_Flash();//对相关参数进行保存
														ModuBusRTU_WriteResponse(Address,Value);
														
															
													}

										break;
							case 160:
										if( Value <= 1)//执行锁机或解锁功能
											{
												sys_flag.Wifi_Lock_System = Value; 

												Write_Internal_Flash();

												ModuBusRTU_WriteResponse(Address,Value);
											}

										break;

								case 161:
										//执行锁机的年份
										if( Value >= 0)
											{
												sys_flag.wifi_Lock_Year = Value; 

												Write_Internal_Flash();

												ModuBusRTU_WriteResponse(Address,Value);

												//得保存下数据
												
											}

										break;
								case 162:
										//执行锁机的月份
										if( Value >= 0 && Value <= 12)
											{
												sys_flag.wifi_Lock_Month = Value; 

												Write_Internal_Flash();
												ModuBusRTU_WriteResponse(Address,Value);

												
											}

										break;
								case 163:
										//执行锁机的日期
										if( Value >= 0 && Value <= 30)
											{
												sys_flag.Wifi_Lock_Day = Value; 

												Write_Internal_Flash();

												ModuBusRTU_WriteResponse(Address,Value);

												
											}
										break;
								

							default:
									break;
						}
					
					break;

			case 0x10:
						Length = U1_Inf.RX_Data[4] * 256 + U1_Inf.RX_Data[5];
						Data1 = U1_Inf.RX_Data[7]*256 + U1_Inf.RX_Data[8];
						Data2 = U1_Inf.RX_Data[9]*256 + U1_Inf.RX_Data[10];
						//01  10  00 A4  00 02  04  00 0F 3D DD 18 EE 写32位数据指令格式
						//响应： 01 10 00 A4    00 02 crc
						switch (Address)
							{
								case 164: //一级密码
										if(Length == 2)
											{
												Data32_Buffer = Data1 *65536 + Data2;
												if(Data32_Buffer >= 100000 && Data32_Buffer <=999999) //控制6位数
													{
														
														ModuBusRTU_Write0x10Response(Address,Length);
														
													}
												
												
												
											}

											

										break;
								case 166:// 二级密码
										if(Length == 2)
											{
												Data32_Buffer = Data1 *65536 + Data2;
												if(Data32_Buffer >= 100000 && Data32_Buffer <=999999) //控制6位数
													{
														
														
														ModuBusRTU_Write0x10Response(Address,Length);
													
													}
												
											}

										break;

								default:

										break;
							}


					break;

			


			default:
					//无效指令
					break;
		}


	return 0 ;
 }

 void Mcu_mail_Wifi(void)
 {
 	uint16 checksum = 0;

	
	
	//同步小程序跟控制器的设置压力
	sys_data.Data_1EH = sys_config_data.zhuan_huan_temperture_value; 
	memset(&Talk,0,sizeof(Talk));
	
	Talk.Mess.Address = 0x00;
	Talk.Mess.Cmd = 0x03;
	Talk.Mess.Length = sizeof(Talk) - 5;

	 
	Talk.Mess.Data_29 =0xFD ;
	Talk.Mess.Data_30 =0XDF  ;//1吨立式蒸汽发生器识别码
   	//1、系统运行状态
	Talk.Mess.Data_01 = sys_data.Data_10H;
	//2、故障代码
	Talk.Mess.Data_02 = sys_flag.Error_Code;
	//3、数字量状态1
	
	
	//4、数字量状态2
	Talk.Mess.Data_04 = 0;
	//5、继电器状态1
	Talk.Mess.Data_05 = sys_data.Data_l3H;
	//6、继电器状态2
	Talk.Mess.Data_06 = 0;
	//7、保留未使用
	Talk.Mess.Data_07 = 0;
	//8、保留未使用
	Talk.Mess.Data_08 = 0;
	//9、排烟温度高位
	Talk.Mess.Data_09 = Temperature_Data.Smoke_Tem >> 8;
	//10、排烟温度低位
	Talk.Mess.Data_10 = Temperature_Data.Smoke_Tem & 0x00FF;
	//11、当前蒸汽压力高位
	Talk.Mess.Data_11 = Temperature_Data.Pressure_Value >> 8;
	//12、当前蒸汽压力低位
	Talk.Mess.Data_12 = Temperature_Data.Pressure_Value & 0x00FF;
	//13、当前蒸汽温度高位
	Talk.Mess.Data_13 = Temperature_Data.steam_value >> 8;
	//14、当前蒸汽温度低位
	Talk.Mess.Data_14 = Temperature_Data.steam_value & 0x00FF;
	//15、当前风机的功率
	Talk.Mess.Data_15 = LCD10D.DLCD.Air_Power;
	
	//20、已设定的目标蒸汽压力值
	Talk.Mess.Data_20 = sys_config_data.zhuan_huan_temperture_value;
	//21、已设定的停炉蒸汽压力值
	Talk.Mess.Data_21 = sys_config_data.Auto_stop_pressure;
	//22、已设定的起炉蒸汽压力值
	Talk.Mess.Data_22 = sys_config_data.Auto_start_pressure;
	
	//27、当期获取数据模式，0则被动，1则主动
	Talk.Mess.Data_27 = Sys_Admin.Access_Data_Mode;
	//28、系统远程控制状态
	
	
	checksum = ModBusCRC16(Talk.ALL,sizeof(Talk));
	Talk.Mess.Crc_H = checksum >> 8;
	Talk.Mess.Crc_L = checksum & 0x00FF;

	
//	Usart_SendStr_length(USART1,(uint8 *)&Talk,sizeof(Talk));
			
	
 
 
 }


uint8 ModuBusRTU_WriteResponse(uint16 address,uint16 Data16)
{
	uint16 check_sum = 0;
	uint8 Device_ID = 1;

		if(U1_Inf.RX_Data[0] > 1)
		{
			//当查询地址大于1时，自动切换为对方询问的地址
			Device_ID = Sys_Admin.ModBus_Address;
		}
	U1_Inf.TX_Data[0] = Device_ID;
	 
	U1_Inf.TX_Data[1]= 0x06;

	
	U1_Inf.TX_Data[2] = address >> 8;    // 地址高字节
	U1_Inf.TX_Data[3] = address & 0x00FF;  //地址低字节
	

	U1_Inf.TX_Data[4] = Data16 >> 8;  //数据高字节
	U1_Inf.TX_Data[5] = Data16 & 0x00FF;   //数据低字节

	check_sum  = ModBusCRC16(U1_Inf.TX_Data,8);   //这个根据总字节数改变
	U1_Inf.TX_Data[6]  = check_sum >> 8 ;
	U1_Inf.TX_Data[7]  = check_sum & 0x00FF;

	Usart_SendStr_length(USART1,U1_Inf.TX_Data,8);

	return 0;
}



uint8 ModuBusRTU_Write0x10Response(uint16 address,uint16 Data16)
{
	uint16 check_sum = 0;
	uint8 Device_ID = 1;

	if(U1_Inf.RX_Data[0] > 1)
		{
			//当查询地址大于1时，自动切换为对方询问的地址
			Device_ID = Sys_Admin.ModBus_Address;
		}

	U1_Inf.TX_Data[0] = Device_ID; 
	U1_Inf.TX_Data[1]= 0x10;
	
	U1_Inf.TX_Data[2] = address >> 8;    // 地址高字节
	U1_Inf.TX_Data[3] = address & 0x00FF;  //地址低字节

	U1_Inf.TX_Data[4] = Data16 >> 8;  // 
	U1_Inf.TX_Data[5] = Data16 & 0X00FF;    

	check_sum  = ModBusCRC16(U1_Inf.TX_Data,8);   //这个根据总字节数改变
	U1_Inf.TX_Data[6]  = check_sum >> 8 ;
	U1_Inf.TX_Data[7]  = check_sum & 0x00FF;

	Usart_SendStr_length(USART1,U1_Inf.TX_Data,8);


	

		return 0;
}



uint8 Boss_Lock_Function(void)
 	{
		
			
 			return 0;
 	}



void NewSHUANG_PIN_Server_Cmd_Analyse(void)
{
   uint8 Cmd = 0;
   uint8 index = 0;
   uint8 Length = 0;
   uint16 Address = 1; //身份地址，不能错
   uint16 Value = 0;	
   uint16 Data = 0; 
   float value_buffer = 0;
   uint8  Device_ID = 1; //身份地址，不能错 ************88
   uint16 check_sum = 0;
   uint16 Data1 = 0;
   uint16 Data2 = 0;
   uint16 Data3 = 0;
   
   Cmd = U1_Inf.RX_Data[1];
   Address = U1_Inf.RX_Data[2] *256+ U1_Inf.RX_Data[3];
   Length = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];
   Value = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];;	 //06格式下，value = length
   
   if(U1_Inf.RX_Data[0] > 1)
		{
			//当查询地址大于1时，自动切换为对方询问的地址
			Device_ID = Sys_Admin.ModBus_Address;
		}
   switch (Cmd)
	   {
		   case 03:  //00 03 01 F3 00 0F F5 D0		

				   switch (Address)
					   {
						   case 100:
								   if(Address == 100 && Length == 18)
									   {
										   U1_Inf.TX_Data[0] = Device_ID;
										   U1_Inf.TX_Data[1] = 0x03;// 
										   U1_Inf.TX_Data[2] = 36; //数据长度为6， 根据情况改变***
								   
										   U1_Inf.TX_Data[3] = 0x00;
										   U1_Inf.TX_Data[4] = UnionD.UnionStartFlag;//1、总运行状态

										   U1_Inf.TX_Data[5] = 0x00;
										   U1_Inf.TX_Data[6] = sys_data.Data_10H;//2、主机运行状态

										   U1_Inf.TX_Data[7] = 0x00;
										   U1_Inf.TX_Data[8] = 0x00;//3、蒸汽温度,  需要特殊处理一下

										   U1_Inf.TX_Data[9] = 0x00;
										   U1_Inf.TX_Data[10] = LCD10D.DLCD.Water_State;//4、主水位状态

										   Data =  Temperature_Data.Smoke_Tem / 10;
										   U1_Inf.TX_Data[11] = Data >> 8;
										   U1_Inf.TX_Data[12] = Data & 0x00FF;//5、主烟气温度

										    Data3 =LCD10D.DLCD.LuNei_WenDu;
										   U1_Inf.TX_Data[13] = Data3 >> 8;
										   U1_Inf.TX_Data[14] = Data3 & 0X00ff;//6、主机内温

										   U1_Inf.TX_Data[15] = 0;
										   U1_Inf.TX_Data[16] = LCD10D.DLCD.Error_Code;//7、主机故障代码

										   U1_Inf.TX_Data[17] = 0;
										   U1_Inf.TX_Data[18] = LCD10D.DLCD.Air_Power;//8、主机风机功率

										   U1_Inf.TX_Data[19] = 0;
										   U1_Inf.TX_Data[20] = LCD10D.DLCD.Pump_State;
										   //9、水泵状态

										   U1_Inf.TX_Data[21] = 0;
										   U1_Inf.TX_Data[22] = LCD10D.DLCD.Flame_State;//10、火焰状态

										   U1_Inf.TX_Data[23] = 0;
										   U1_Inf.TX_Data[24] = LCD10JZ[2].DLCD.Water_State;//11、从水位状态

										   Data3 = LCD10JZ[2].DLCD.Smoke_WenDu;
										   U1_Inf.TX_Data[25] = Data3 >> 8;
										  
										   U1_Inf.TX_Data[26] = Data3 & 0x00FF;//12、从烟温
											
										   Data3 = LCD10JZ[2].DLCD.LuNei_WenDu;
										   U1_Inf.TX_Data[27] = Data3 >> 8;
										   U1_Inf.TX_Data[28] = Data3 & 0x00FF;//13、从内温

										   U1_Inf.TX_Data[29] = 0;
										   U1_Inf.TX_Data[30] = LCD10JZ[2].DLCD.Error_Code;//14、从故障码

										   U1_Inf.TX_Data[31] = 0;
										   U1_Inf.TX_Data[32] = LCD10JZ[2].DLCD.Air_Power;//15、从风机功率

										   
										   U1_Inf.TX_Data[33] = 0;
										   U1_Inf.TX_Data[34] = LCD10JZ[2].DLCD.Pump_State ;//16、从水泵状态

										   U1_Inf.TX_Data[35] = 0;
										   U1_Inf.TX_Data[36] = LCD10JZ[2].DLCD.Flame_State ;//17、从火焰状态


										   U1_Inf.TX_Data[37] = 0;
										   U1_Inf.TX_Data[38] = LCD10JZ[2].DLCD.Device_State ;//18、从工作状态


										   //*******************
										   
										   check_sum  = ModBusCRC16(U1_Inf.TX_Data,41);   //这个11根据总字节数改变
										   U1_Inf.TX_Data[39]  = check_sum >> 8 ;
										   U1_Inf.TX_Data[40]  = check_sum & 0x00FF;
										   
										   Usart_SendStr_length(USART1,U1_Inf.TX_Data,41);
										   
									   }
								   break;
						   case 120:  //读取蒸汽压力浮点数
								   
								   if(Address == 120 && Length == 4)
									   {
										   U1_Inf.TX_Data[0] = Device_ID;
										   U1_Inf.TX_Data[1] = 0x03;// 
										   U1_Inf.TX_Data[2] = 8; //数据长度为 ， 根据情况改变***

										  
										   Float_Int.value = LCD10JZ[1].DLCD.Steam_Pressure;
										   U1_Inf.TX_Data[3] = Float_Int.byte4.data_HH;
										   U1_Inf.TX_Data[4] = Float_Int.byte4.data_HL;//  

										   U1_Inf.TX_Data[5] = Float_Int.byte4.data_LH;
										   U1_Inf.TX_Data[6] = Float_Int.byte4.data_LL;
										   
											   
											   
										   Float_Int.value = LCD10JZ[2].DLCD.Steam_Pressure;
										   U1_Inf.TX_Data[7] = Float_Int.byte4.data_HH;
										   U1_Inf.TX_Data[8] = Float_Int.byte4.data_HL;//  

										   U1_Inf.TX_Data[9] = Float_Int.byte4.data_LH;
										   U1_Inf.TX_Data[10] = Float_Int.byte4.data_LL;

										   check_sum  = ModBusCRC16(U1_Inf.TX_Data,13);   //这个根据总字节数改变
										   U1_Inf.TX_Data[11]  = check_sum >> 8 ;
										   U1_Inf.TX_Data[12]  = check_sum & 0x00FF;
										   
										   Usart_SendStr_length(USART1,U1_Inf.TX_Data,13);
										   
									   }

								   break;
						  case 130:  //读取主从机高压侧，蒸汽压力浮点数
								   
								   if(Address == 130 && Length == 4)
									   {
										   U1_Inf.TX_Data[0] = Device_ID;
										   U1_Inf.TX_Data[1] = 0x03;// 
										   U1_Inf.TX_Data[2] = 8; //数据长度为 ， 根据情况改变***

										  
										   Float_Int.value = LCD10D.DLCD.Inside_High_Pressure;
										   U1_Inf.TX_Data[3] = Float_Int.byte4.data_HH;
										   U1_Inf.TX_Data[4] = Float_Int.byte4.data_HL;//  

										   U1_Inf.TX_Data[5] = Float_Int.byte4.data_LH;
										   U1_Inf.TX_Data[6] = Float_Int.byte4.data_LL;
										   
											   
											   
										   Float_Int.value = LCD10JZ[2].DLCD.Inside_High_Pressure;
										   U1_Inf.TX_Data[7] = Float_Int.byte4.data_HH;
										   U1_Inf.TX_Data[8] = Float_Int.byte4.data_HL;//  

										   U1_Inf.TX_Data[9] = Float_Int.byte4.data_LH;
										   U1_Inf.TX_Data[10] = Float_Int.byte4.data_LL;

										   check_sum  = ModBusCRC16(U1_Inf.TX_Data,13);   //这个根据总字节数改变
										   U1_Inf.TX_Data[11]  = check_sum >> 8 ;
										   U1_Inf.TX_Data[12]  = check_sum & 0x00FF;
										   
										   Usart_SendStr_length(USART1,U1_Inf.TX_Data,13);
										   
									   }

								   break;

							 
						   case 150:  //蒸汽压力参数设置
								   
									   if( Length == 3)  
										   {
											   U1_Inf.TX_Data[0] = Device_ID;
											   U1_Inf.TX_Data[1] = 0x03;// 
											   U1_Inf.TX_Data[2] = 6; //数据长度为 ， 根据情况改变***

											   U1_Inf.TX_Data[3] = sys_config_data.zhuan_huan_temperture_value >> 8;
											   U1_Inf.TX_Data[4] = sys_config_data.zhuan_huan_temperture_value & 0x00FF;// 1目标蒸汽压力

											   U1_Inf.TX_Data[5] = sys_config_data.Auto_stop_pressure >> 8;
											   U1_Inf.TX_Data[6] = sys_config_data.Auto_stop_pressure & 0x00FF;

											   U1_Inf.TX_Data[7] = sys_config_data.Auto_start_pressure >> 8;
											   U1_Inf.TX_Data[8] = sys_config_data.Auto_start_pressure & 0x00FF;

											   check_sum  = ModBusCRC16(U1_Inf.TX_Data,11);   //这个根据总字节数改变
											   U1_Inf.TX_Data[9]  = check_sum >> 8 ;
											   U1_Inf.TX_Data[10]  = check_sum & 0x00FF;
											   
											   Usart_SendStr_length(USART1,U1_Inf.TX_Data,11);
										   }
								   break;
							case 160:
									if( Length == 4)  
											{
												U1_Inf.TX_Data[0] = Device_ID;
												U1_Inf.TX_Data[1] = 0x03;// 
												U1_Inf.TX_Data[2] = 8 ;    //这个数值也要跟着变
												
												U1_Inf.TX_Data[3] = 0;
												U1_Inf.TX_Data[4] = sys_flag.Wifi_Lock_System;

												U1_Inf.TX_Data[5] = sys_flag.wifi_Lock_Year >> 8;
												U1_Inf.TX_Data[6] = sys_flag.wifi_Lock_Year & 0x00FF;

												U1_Inf.TX_Data[7] = 0;
												U1_Inf.TX_Data[8] = sys_flag.wifi_Lock_Month;

												U1_Inf.TX_Data[9] = 0;
												U1_Inf.TX_Data[10] = sys_flag.Wifi_Lock_Day;
												
												check_sum  = ModBusCRC16(U1_Inf.TX_Data,13);   //这个根据总字节数改变
												U1_Inf.TX_Data[11]  = check_sum >> 8 ;
												U1_Inf.TX_Data[12]  = check_sum & 0x00FF;
												
												Usart_SendStr_length(USART1,U1_Inf.TX_Data,13);
											}


									break;


						   default:

								   break;
					   }
				   
				   break;
		   case 06://单个寄存器的写入
				   
				   switch (Address)
					   {
						   case 0x0064:////地址40100 为控制字 控制启停寄存器
									   Data = U1_Inf.RX_Data[4] *256+ U1_Inf.RX_Data[5];
									   if(Data == 0x00)//待机命令
									   {
										   if(UnionD.UnionStartFlag) 
											   {
												   UnionD.UnionStartFlag = FALSE;//取消启动指令
												  
												   
												 if(sys_data.Data_10H == 2)
													sys_close_cmd();
								
								
												SlaveG[2].Startclose_Sendflag = 3;
												SlaveG[2].Startclose_Data = 0;

												ModuBusRTU_WriteResponse(Address,Value);
												
												
											   }
									   }

									   if(Data == 0x0001)//开机命令
									   {
										   //主机未启用，从机也没连接，则不能启动
										    ModuBusRTU_WriteResponse(Address,Value);
										   if(LCD10JZ[1].DLCD.YunXu_Flag == 0 && LCD10JZ[2].DLCD.YunXu_Flag == 0)
											   break;

										   
										   if(LCD10JZ[1].DLCD.YunXu_Flag  || LCD10JZ[2].DLCD.YunXu_Flag )
											   if(UnionD.UnionStartFlag  == 0) //系统处于待机状态或手动或报警
												   {
													  UnionD.UnionStartFlag = OK; //主运行状态的切换

													 
												   }
										   
									   }
									  

										   
									   break;

						   case 0x006A:// 故障复位寄存器,主机故障复位解除
										
									   if((sys_flag.Error_Code)||LCD10JZ[2].DLCD.Error_Code)
										   { 
											   LCD10JZ[2].DLCD.Error_Code = 0;
												SlaveG[2].ResetError_Flag = 3;
												SlaveG[2].ResetError_Data = 0;
											   sys_flag.Error_Code = 0;
											   Beep_Data.beep_start_flag = 0;

											  

											   Beep_Data.beep_start_flag = 0;//消音标志
											   sys_flag.Lock_Error = 0; //解除主机故障锁定
												
											  ModuBusRTU_WriteResponse(Address,Value);

											   
										   }
									   
									   break;

						   case 0x0071: //从机的故障复位，跟主机执行同样的功能
									   if(sys_flag.Error_Code ||LCD10JZ[2].DLCD.Error_Code)
										   { 
											   LCD10JZ[2].DLCD.Error_Code = 0;
												SlaveG[2].ResetError_Flag = 3;
												SlaveG[2].ResetError_Data = 0;
											   sys_flag.Error_Code = 0;
											   Beep_Data.beep_start_flag = 0;
											  
												ModuBusRTU_WriteResponse(Address,Value);
											  
												
											   
										   }

									   break;

						   case 150:  //目标蒸汽压力设置
									   
											   if(Value < sys_config_data.Auto_stop_pressure)
												   {
													   if(Value >= 20)
														   {
															   sys_config_data.zhuan_huan_temperture_value = Value;
															   Write_Internal_Flash();//对相关参数进行保存

															   ModuBusRTU_WriteResponse(Address,Value);
															  
															   
														   }
												   }

											  
									   
									   break;
						   case 151:  //停机蒸汽压力设置
								   
										   if(Value < Sys_Admin.DeviceMaxPressureSet)
											   {
												   if(Value > sys_config_data.zhuan_huan_temperture_value)
													   {
														   sys_config_data.Auto_stop_pressure = Value;
														   Write_Internal_Flash();//对相关参数进行保存
														   ModuBusRTU_WriteResponse(Address,Value);
														   
													   }
											   }

										  
									   
								   
								   break;
						   case 152: //启动蒸汽压力
									   
											   if(Value < sys_config_data.Auto_stop_pressure)
												   {
													   sys_config_data.Auto_start_pressure = Value;
													   Write_Internal_Flash();//对相关参数进行保存

													   ModuBusRTU_WriteResponse(Address,Value);
												   }
									   break;
						 	case 160:
										if( Value <= 1)//执行锁机或解锁功能
											{
												sys_flag.Wifi_Lock_System = Value; 
												
												Write_Internal_Flash();

												ModuBusRTU_WriteResponse(Address,Value);
											}

										break;

							case 161:
									//执行锁机的年份
									if( Value >= 0)
										{
											sys_flag.wifi_Lock_Year = Value; 

											Write_Internal_Flash();

											ModuBusRTU_WriteResponse(Address,Value);

											//得保存下数据
											
										}

									break;
							case 162:
									//执行锁机的月份
									if( Value >= 0 && Value <= 12)
										{
											sys_flag.wifi_Lock_Month = Value; 

											Write_Internal_Flash();
											ModuBusRTU_WriteResponse(Address,Value);
										}

									break;
							case 163:
									//执行锁机的日期
									if( Value >= 0 && Value <= 30)
										{
											sys_flag.Wifi_Lock_Day = Value; 

											Write_Internal_Flash();

											ModuBusRTU_WriteResponse(Address,Value);
										}
									break;

						   
						  

						   default:
								   break;
					   }
				   
				   break;

		  

		   default:
				   //无效指令
				   break;
	   }
}


uint32 Char_to_Int_6(uint32 number_H,uint32 number_L) 
{
    uint8 Code[6] ={0};
	uint32 Return_Value = 0;



	Code[0] = number_L & 0x000000FF - 0x30;
	number_L  = number_L >> 8;
	Code[1] = number_L & 0x000000FF - 0x30;
	 
	Code[2] = number_H & 0x000000FF - 0x30;
	number_H  = number_H >> 8;
	Code[3] = number_H & 0x000000FF - 0x30;
	number_H  = number_H >> 8;
	Code[4] = number_H & 0x000000FF - 0x30;
	number_H  = number_H >> 8;
	Code[5] = number_H & 0x000000FF - 0x30;
	
	 Return_Value =Code[0] + Code[1]* 10 + Code[2]* 100 + Code[3]* 1000 + Code[4]* 10000 + Code[5]* 100000;
	
	
	
    return Return_Value; // 返回转换后的BCD码
}






