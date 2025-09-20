#include "usart4.h"
#include "stdarg.h"

//串口发送缓存区 	
uint8 USART4_TX_BUF[256]; 	             //发送缓冲,最大USART3_MAX_SEND_LEN字节	 1024 






PROTOCOL_COMMAND lcd_command;

//RECE_LDATA	Rece_Lcd_Data;

uint8_t CMD_RXFRMOK = 0;

UNION_GG  UnionD;


SLAVE_GG  SlaveG[13];

USlave_Struct JiZu[12];



uint8 cmd_get_time[] ={0x5A, 0xA5, 0x03,0x81,0x20,0x10};//用于在开机时发命令给LCD，同步时间信息

uint8 cmd_get_set_time[] ={0x5A,0XA5,0X04,0X83,0X00,0XB0,0X0B};//用于读取用户设置时间变量
///////////////////////////////////////USART4 printf支持部分//////////////////////////////////
//串口2,u2_printf 函数
//确保一次发送数据不超过USART4_MAX_SEND_LEN字节
////////////////////////////////////////////////////////////////////////////////////////////////
void u4_printf(char* fmt,...)  
{  
  int len=0;
	int cnt=0;
//	unsigned  char i;
//	unsigned  char Frame_Info[5]; //指令长度
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART4_TX_BUF,fmt,ap);
	va_end(ap);
	len = strlen((const char*)USART4_TX_BUF);
	while(len--)
	  {
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=1); //等待发送结束
	  USART_SendData(UART4,USART4_TX_BUF[cnt++]);
	  }
}






//串口4发送s个字符

///////////////////////////////////////USART2 初始化配置部分//////////////////////////////////	    
//功能：初始化IO 串口2
//输入参数
//bound:波特率
//输出参数
//无
//////////////////////////////////////////////////////////////////////////////////////////////	  
void uart4_init(u32 bound)
{  	 		 
	//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能USART4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能GPIOC时钟
	USART_DeInit(UART4);  //复位串口4

     //USART4_TX   PC.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
   
    //USART4_RX	  PC.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);  

    //Usart4 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	        //根据指定的参数初始化VIC寄存器
  
    //USART4 初始化设置
	USART_InitStructure.USART_BaudRate = bound;   //一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;  //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //收发模式

    USART_Init(UART4, &USART_InitStructure);   //初始化串口
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  //开启中断
    USART_Cmd(UART4, ENABLE);                      //使能串口 
	
//	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);    //使能串口2的DMA发送
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF,1000);  //DMA1通道7,外设为串口2,存储器为USART2_TX_BUF,长度1000. 										  	
}



void ModBus_Uart4_Local_Communication(void)
{
		
		uint8  i = 0;	
		
		uint16 checksum = 0;

		if(Sys_Admin.Device_Style == 2 || Sys_Admin.Device_Style == 3) //相变机型或多拼机型，跟主机失联情况处理
			{
				if(sys_flag.Address_Number > 1)
					{
						if(sys_flag.UnTalk_Time > 15) //15秒
							{
								if(sys_data.Data_10H == 2)
									{
										sys_close_cmd();
										
									}
								if(sys_data.Data_10H == 3)
									{
										 sys_data.Data_10H = 0;
										 
										
									}
								
								GetOut_Mannual_Function();
								
							}
					}
			}

	
		if(U4_Inf.Recive_Ok_Flag)
			{
				U4_Inf.Recive_Ok_Flag = 0;//不能少哦
				 //关闭中断
				USART_ITConfig(UART4, USART_IT_RXNE, DISABLE); 
				 
				// 验证数据长度
				if(U4_Inf.RX_Length < 8) {
					// 数据太短，不是有效的ModBus命令
					USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
					return;
				}
				 
				checksum  = U4_Inf.RX_Data[U4_Inf.RX_Length - 2] * 256 + U4_Inf.RX_Data[U4_Inf.RX_Length - 1];
				
				// 注释掉调试输出，避免干扰串口4通信
				// u4_printf("收到数据长度=%d, 收到CRC=%04X, 计算CRC=%04X\n", 
				//           U4_Inf.RX_Length, checksum, ModBusCRC16(U4_Inf.RX_Data, U4_Inf.RX_Length));

				//老板键，对设备序列号进行修改
				// ModBusCRC16函数内部会计算前(size-2)个字节，所以传入总长度
				if( checksum == ModBusCRC16(U4_Inf.RX_Data, U4_Inf.RX_Length))  // 恢复CRC校验
					{
						// 注释掉调试输出
						// u4_printf("\n*地址参数= %d\n",Sys_Admin.ModBus_Address);
						
						if(sys_flag.Address_Number == 0)
							{
								//根据屏幕设定地址
								if(U4_Inf.RX_Data[0] == Sys_Admin.ModBus_Address)
									UART4_Server_Cmd_Analyse();
							}
						else
							{
								//根据主板拨码地址
								if(U4_Inf.RX_Data[0] == sys_flag.Address_Number)
									{
										sys_flag.UnTalk_Time = 0;
										UART4_Server_Cmd_Analyse();
									}
									
							}
						
						
					
					}
					
			 
				
			//对接收缓冲器清零
				for( i = 0; i < 100;i++ )
					U4_Inf.RX_Data[i] = 0x00;
			
			//重新开启中断
				USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); 
				
			}
}



void UART4_Server_Cmd_Analyse(void)
 {
 	uint8 Cmd = 0;
	uint8 index = 0;
	uint8 Length = 0;
	uint16 Value = 0;
	uint16 Address = 0;
	uint16 Data = 0; 
	uint8  Device_ID = Sys_Admin.ModBus_Address; //身份地址，不能错 ************88
	float  value_buffer = 0;
	uint16 check_sum = 0;
	uint16 Data1 = 0;
	uint16 Data2 = 0;
	uint16 Data3 = 0;
	uint16 Data_Address = 0;
	uint16 Buffer_Data16 = 0;
	uint16 Data_Length = 0;

	uint8    Save_Flag1 = 0;
	uint8    Save_Flag2 = 0;
	


	 //Float_Int.value  用于单精度的转换
	if(sys_flag.Address_Number == 0)
		Device_ID = Sys_Admin.ModBus_Address;
	else
		Device_ID = sys_flag.Address_Number;
	
	Cmd = U4_Inf.RX_Data[1];
	Address = U4_Inf.RX_Data[2] *256+ U4_Inf.RX_Data[3];
	Length = U4_Inf.RX_Data[4] *256+ U4_Inf.RX_Data[5];
	Value = U4_Inf.RX_Data[4] *256+ U4_Inf.RX_Data[5]; //06格式下， 
	
	
	switch (Cmd)
		{
			case 03:  //00 03 01 F3 00 0F F5 D0		

					switch (Address)
						{
							case 100:
									if( Length == 18)  //这边的数据长度也根据总变化，不是字节长度的变化
										{
											
											U4_Inf.TX_Data[0] = Device_ID;
											U4_Inf.TX_Data[1] = 0x03;// 
											U4_Inf.TX_Data[2] = 36; //数据长度为6， 根据情况改变***
									
											U4_Inf.TX_Data[3] = 0x00;
											U4_Inf.TX_Data[4] = sys_data.Data_10H;// 1当前的运行状态

											U4_Inf.TX_Data[5] = Temperature_Data.Pressure_Value >> 8 ;
											U4_Inf.TX_Data[6] = Temperature_Data.Pressure_Value & 0x00FF;//2当前蒸汽的压力值当前的水位状态
											
											U4_Inf.TX_Data[7] = 0x00;
											U4_Inf.TX_Data[8] = Temperature_Data.steam_value;//3当前蒸汽的温度

											U4_Inf.TX_Data[9] = 0x00;
											U4_Inf.TX_Data[10] = LCD10D.DLCD.Water_State;//4当前水位的状态

											Buffer_Data16 = Temperature_Data.Smoke_Tem /10;
											U4_Inf.TX_Data[11] = Buffer_Data16 >> 8;
											U4_Inf.TX_Data[12] = Buffer_Data16 & 0x00FF;//5烟气的温度

											U4_Inf.TX_Data[13] = sys_flag.Protect_WenDu >> 8;
											U4_Inf.TX_Data[14] = sys_flag.Protect_WenDu & 0x00FF;//6内部烟气的温度

											U4_Inf.TX_Data[15] = 0;
											U4_Inf.TX_Data[16] = sys_flag.Error_Code;//7故障代码

											U4_Inf.TX_Data[17] = 0;
											U4_Inf.TX_Data[18] = sys_flag.flame_state;//8火焰状态 

											U4_Inf.TX_Data[19] = sys_flag.Fan_Rpm >> 8;
											U4_Inf.TX_Data[20] = sys_flag.Fan_Rpm & 0x00FF;//9风机转速

											U4_Inf.TX_Data[21] = Temperature_Data.Inside_High_Pressure >> 8;
											U4_Inf.TX_Data[22] = Temperature_Data.Inside_High_Pressure & 0x00FF;      //10 内部压力

 											U4_Inf.TX_Data[23] = 0;
 											U4_Inf.TX_Data[24] = LCD10D.DLCD.Air_Power;//11风机功率 

											U4_Inf.TX_Data[25] = 0;
											U4_Inf.TX_Data[26] = Switch_Inf.air_on_flag; //12风机运行状态


											U4_Inf.TX_Data[27] = 0;
											U4_Inf.TX_Data[28] = Switch_Inf.Water_Valve_Flag;//13进水阀

											U4_Inf.TX_Data[29] = 0;
											U4_Inf.TX_Data[30] = Switch_Inf.pai_wu_flag;//14排污阀状态

											U4_Inf.TX_Data[31] = 0;
											U4_Inf.TX_Data[32] = Switch_Inf.LianXu_PaiWu_flag;//15连续排污阀状态
											
											
											U4_Inf.TX_Data[33] = 0;
											U4_Inf.TX_Data[34] = Water_State.ZSignal;//16  进水阀请求
											
											U4_Inf.TX_Data[35] = sys_flag.ChaYaWater_Value >> 8;
											U4_Inf.TX_Data[36] = sys_flag.ChaYaWater_Value & 0x00FF;//17  差压液位的实际值

											U4_Inf.TX_Data[37] = sys_flag.Inputs_State >> 8;
											U4_Inf.TX_Data[38] = sys_flag.Inputs_State & 0x00FF;//18 

											
											
										
										 
											check_sum  = ModBusCRC16(U4_Inf.TX_Data,41);   //这个根据总字节数改变
											U4_Inf.TX_Data[39]  = check_sum >> 8 ;
											U4_Inf.TX_Data[40]  = check_sum & 0x00FF;
											
											Usart_SendStr_length(UART4,U4_Inf.TX_Data,41);
											
										}

									break;
							case 120:  //读取蒸汽压力浮点数
									value_buffer  = Temperature_Data.Pressure_Value;
									Float_Int.value = value_buffer / 100;
									if(Address == 120 && Length == 2)
										{
											U4_Inf.TX_Data[0] = Device_ID;
											U4_Inf.TX_Data[1] = 0x03;// 
											U4_Inf.TX_Data[2] = 4; //数据长度为 ， 根据情况改变***

											U4_Inf.TX_Data[3] = Float_Int.byte4.data_HH;
											U4_Inf.TX_Data[4] = Float_Int.byte4.data_HL;// 1当前的运行状态

											U4_Inf.TX_Data[5] = Float_Int.byte4.data_LH;
											U4_Inf.TX_Data[6] = Float_Int.byte4.data_LL;

											check_sum  = ModBusCRC16(U4_Inf.TX_Data,9);   //这个根据总字节数改变
											U4_Inf.TX_Data[7]  = check_sum >> 8 ;
											U4_Inf.TX_Data[8]  = check_sum & 0x00FF;
											
											Usart_SendStr_length(UART4,U4_Inf.TX_Data,9);
											
										}

									break;
							case 150:  //蒸汽压力参数设置
									
										if( Length == 3)  
											{
												U4_Inf.TX_Data[0] = Device_ID;
												U4_Inf.TX_Data[1] = 0x03;// 
												U4_Inf.TX_Data[2] = 6; //数据长度为 ， 根据情况改变***

												U4_Inf.TX_Data[3] = sys_config_data.zhuan_huan_temperture_value >> 8;
												U4_Inf.TX_Data[4] = sys_config_data.zhuan_huan_temperture_value & 0x00FF;// 1目标蒸汽压力

												U4_Inf.TX_Data[5] = sys_config_data.Auto_stop_pressure >> 8;
												U4_Inf.TX_Data[6] = sys_config_data.Auto_stop_pressure & 0x00FF;

												U4_Inf.TX_Data[7] = sys_config_data.Auto_start_pressure >> 8;
												U4_Inf.TX_Data[8] = sys_config_data.Auto_start_pressure & 0x00FF;

												check_sum  = ModBusCRC16(U4_Inf.TX_Data,11);   //这个根据总字节数改变
												U4_Inf.TX_Data[9]  = check_sum >> 8 ;
												U4_Inf.TX_Data[10]  = check_sum & 0x00FF;
												
												Usart_SendStr_length(UART4,U4_Inf.TX_Data,11);
											}
										if(Length == 18)
										{
											U4_Inf.TX_Data[0] = Device_ID;
											U4_Inf.TX_Data[1] = 0x03;// 
											U4_Inf.TX_Data[2] = 36; //数据长度为6， 根据情况改变***
									
											U4_Inf.TX_Data[3] = sys_config_data.zhuan_huan_temperture_value >> 8;
											U4_Inf.TX_Data[4] = sys_config_data.zhuan_huan_temperture_value & 0x00FF;// 1目标蒸汽压力

											U4_Inf.TX_Data[5] = sys_config_data.Auto_stop_pressure >> 8;
											U4_Inf.TX_Data[6] = sys_config_data.Auto_stop_pressure & 0x00FF;  //2 停止压力

											U4_Inf.TX_Data[7] = sys_config_data.Auto_start_pressure >> 8;
											U4_Inf.TX_Data[8] = sys_config_data.Auto_start_pressure & 0x00FF; //3 启动压力

											U4_Inf.TX_Data[9] = 0x00;
											U4_Inf.TX_Data[10] = Sys_Admin.Dian_Huo_Power;//4 点火功率 机器不一致

											U4_Inf.TX_Data[11] = 0x00;
											U4_Inf.TX_Data[12] = Sys_Admin.Max_Work_Power;//5 最大运行功率 机器不一致

											U4_Inf.TX_Data[13] = Sys_Admin.Inside_WenDu_ProtectValue>> 8;
											U4_Inf.TX_Data[14] = Sys_Admin.Inside_WenDu_ProtectValue & 0x00FF;//6内温保护值，机器不一致

											U4_Inf.TX_Data[15] =  Sys_Admin.Danger_Smoke_Value >> 8;
											U4_Inf.TX_Data[16] =  Sys_Admin.Danger_Smoke_Value & 0x00FF;//7排烟温度报警值 机器不一致

											U4_Inf.TX_Data[17] = 0;
											U4_Inf.TX_Data[18] = Sys_Admin.Water_Max_Percent;//8 变频补水最大开度，所有机器一致

											U4_Inf.TX_Data[19] = Sys_Admin.LianXu_PaiWu_DelayTime;  //连续排污周期分钟
											U4_Inf.TX_Data[20] = Sys_Admin.LianXu_PaiWu_OpenSecs;//连续排污开启几秒，这个所有机器一致

											U4_Inf.TX_Data[21] = 0 ;  // 
											U4_Inf.TX_Data[22] =0 ;// 
											
 											U4_Inf.TX_Data[23] = 0;
 											U4_Inf.TX_Data[24] = 0;//11  

											U4_Inf.TX_Data[25] = 0;
											U4_Inf.TX_Data[26] = 0; //12 


											U4_Inf.TX_Data[27] = 0;
											U4_Inf.TX_Data[28] = 0;//13 

											U4_Inf.TX_Data[29] = 0;
											U4_Inf.TX_Data[30] = Sys_Admin.Fan_Speed_Check;//14 

											U4_Inf.TX_Data[31] = Sys_Admin.Water_BianPin_Enabled;
											U4_Inf.TX_Data[32] = Sys_Admin.YuRe_Enabled;//15  各种开关的标志位，本体温度保护，变频补水开启，风速检测
											
											
											U4_Inf.TX_Data[33] = 0;
											U4_Inf.TX_Data[34] = Sys_Admin.DeviceMaxPressureSet;//16  额定蒸汽压力
											
											U4_Inf.TX_Data[35] = 0;
											U4_Inf.TX_Data[36] = Sys_Admin.Device_Style ;//17  设备类型

											U4_Inf.TX_Data[37] = 0;
											U4_Inf.TX_Data[38] = sys_config_data.Sys_Lock_Set;//18   设备锁机
										
										 
											check_sum  = ModBusCRC16(U4_Inf.TX_Data,41);   //这个根据总字节数改变
											U4_Inf.TX_Data[39]  = check_sum >> 8 ;
											U4_Inf.TX_Data[40]  = check_sum & 0x00FF;
											
											Usart_SendStr_length(UART4,U4_Inf.TX_Data,41);

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
										 
										if(sys_data.Data_10H  == 0  && Value == 1)//待机状态 执行启动命令，不能有故障哦
											{
												 
												sys_start_cmd();

												//ModuBus4RTU_WriteResponse(Address,Value);
												
											}

										if(sys_data.Data_10H  == 0  && Value == 2)//待机状态 执行启动命令，不能有故障哦
											{
												sys_start_cmd();

												//ModuBus4RTU_WriteResponse(Address,Value);
											}

										if(sys_data.Data_10H  == 0  && Value == 3)//待机状态 要进入手动模式
											{
												 
												sys_data.Data_10H  = 3;
												GetOut_Mannual_Function();

												//ModuBus4RTU_WriteResponse(Address,Value);
											}

										if(sys_data.Data_10H  == 3  && Value == 0)//手动状态 要进入待机模式
											{
												 
												sys_data.Data_10H  = 0;
												GetOut_Mannual_Function();

												//ModuBus4RTU_WriteResponse(Address,Value);
											}


										if(sys_data.Data_10H == 2  && Value == 0)//运行状态 执行关闭待机命令， 
											{
												sys_close_cmd();
												//ModuBus4RTU_WriteResponse(Address,Value);
												
											}
										ModuBus4RTU_WriteResponse(Address,Value);
												
										
										break;

							case 0x006B:  //故障复位处理
										
										ModuBus4RTU_WriteResponse(Address,0);
										
										sys_flag.Error_Code = 0;
										Beep_Data.beep_start_flag = 0;//消音标志
										sys_flag.Lock_Error = 0; //解除故障锁定

											
										break;
							case 0x6F:  //手动模式下功率的开启或关闭
										if(Value <= 100)
											{
												PWM_Adjust(Value);
											}
										 ModuBus4RTU_WriteResponse(Address,Value);

										break;

							case 0x70:  //手动模式下风机的开启或关闭
										if(Value == 0)
											{
												Send_Air_Close();//关闭风机
											}

										if(Value == 1)
											{
												Send_Air_Open();//开启风机
												PWM_Adjust(100);
											}
										ModuBus4RTU_WriteResponse(Address,Value);

										break;
							case 0x71:  //手动模式下水泵的开启或关闭，如果是变频模式，考虑开度
										//sys_data.Data_10H = 3;
										//双拼模式下，从机进水电磁阀的控制权限在主机手上，不分手动或自动
										if(Value == 0)
											{
												Feed_Main_Pump_OFF();
												sys_flag.WaterClose_Time = 2;  //开启顺序有先后，手动模式，强制开启或关闭
												Second_Water_Valve_Close();
												sys_flag.Water_Percent = 0;
											}

										if(Value == 1)
											{
												sys_flag.WaterOut_Time = 3;
												Feed_Main_Pump_ON();
												Second_Water_Valve_Open();
												sys_flag.Water_Percent = 50;
												 
											}
										ModuBus4RTU_WriteResponse(Address,Value);

										break;
							case 0x72:  //手动模式下排污阀的开启或关闭，
										sys_data.Data_10H = 3;
										if(Value == 0)
											{
												Pai_Wu_Door_Close();
											}

										if(Value == 1)
											{
												Pai_Wu_Door_Open();
												 
											}
										ModuBus4RTU_WriteResponse(Address,Value);

										break;
							case 0x73:  //手动模式下连续排污阀的开启或关闭，
										sys_data.Data_10H = 3;
										if(Value == 0)
											{
												LianXu_Paiwu_Close();
											}

										if(Value == 1)
											{
												LianXu_Paiwu_Open();
												 
											}
										ModuBus4RTU_WriteResponse(Address,Value);

										break;
							case 0x75:  //自动大排污一次
										
										if(Value == 1)
											{
												sys_flag.Paiwu_Flag = OK;
											}
										ModuBus4RTU_WriteResponse(Address,Value);

										break;
							

							case 150:  //目标蒸汽压力设置
										
										if(Value < sys_config_data.Auto_stop_pressure)
											{
												if(Value >= 20)
													{
														sys_config_data.zhuan_huan_temperture_value = Value;
														Write_Internal_Flash();//对相关参数进行保存
														ModuBus4RTU_WriteResponse(Address,Value);
														
														
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
																ModuBus4RTU_WriteResponse(Address,Value);
																
															}
													}
										
										break;
							case 152: //启动蒸汽压力
										
												if(Value < sys_config_data.Auto_stop_pressure)
													{
														sys_config_data.Auto_start_pressure = Value;
														Write_Internal_Flash();//对相关参数进行保存

														ModuBus4RTU_WriteResponse(Address,Value);
	
													}
											

										break;
							

							default:
									break;
						}
					
					break;
			case 0x10:   //多个寄存器写入
						Data_Address = U4_Inf.RX_Data[2] * 256 + U4_Inf.RX_Data[3];
						Data_Length = U4_Inf.RX_Data[4] * 256 + U4_Inf.RX_Data[5];
						Buffer_Data16 = U4_Inf.RX_Data[7] *256 + U4_Inf.RX_Data[8];  //高低字节的顺序颠倒
						switch (Data_Address)
							{
							case 150:   //处理多个参数的写入问题
										
										//对比各个数据，存储的存储
										if(sys_config_data.zhuan_huan_temperture_value !=  Buffer_Data16)
											{
												sys_config_data.zhuan_huan_temperture_value = Buffer_Data16;
												Save_Flag1 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[9] *256 + U4_Inf.RX_Data[10]; 
										if(sys_config_data.Auto_stop_pressure !=  Buffer_Data16)
											{
												sys_config_data.Auto_stop_pressure = Buffer_Data16;
												Save_Flag1 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[11] *256 + U4_Inf.RX_Data[12]; 
										if(sys_config_data.Auto_start_pressure !=  Buffer_Data16)
											{
												sys_config_data.Auto_start_pressure = Buffer_Data16;
												Save_Flag1 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[13] *256 + U4_Inf.RX_Data[14]; 
										if(Sys_Admin.Dian_Huo_Power !=  Buffer_Data16)
											{
												Sys_Admin.Dian_Huo_Power = Buffer_Data16;
												Save_Flag2 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[15] *256 + U4_Inf.RX_Data[16]; 
										if(Sys_Admin.Max_Work_Power !=  Buffer_Data16)
											{
												Sys_Admin.Max_Work_Power = Buffer_Data16;
												Save_Flag2 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[17] *256 + U4_Inf.RX_Data[18]; 
										if(Sys_Admin.Inside_WenDu_ProtectValue !=  Buffer_Data16)
											{
												Sys_Admin.Inside_WenDu_ProtectValue = Buffer_Data16;
												Save_Flag2 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[19] *256 + U4_Inf.RX_Data[20]; 
										Buffer_Data16 = Buffer_Data16 * 10;
										if(Sys_Admin.Danger_Smoke_Value !=  Buffer_Data16)
											{
												Sys_Admin.Danger_Smoke_Value = Buffer_Data16 ;
												Save_Flag2 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[21] *256 + U4_Inf.RX_Data[22]; 
										if(Sys_Admin.Water_Max_Percent !=  Buffer_Data16)
											{
												Sys_Admin.Water_Max_Percent = Buffer_Data16;
												Save_Flag2 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[23] ; 
										if(Sys_Admin.LianXu_PaiWu_DelayTime !=  Buffer_Data16)
											{
												Sys_Admin.LianXu_PaiWu_DelayTime = Buffer_Data16;
												Save_Flag2 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[24] ; 
										if(Sys_Admin.LianXu_PaiWu_OpenSecs !=  Buffer_Data16)
											{
												Sys_Admin.LianXu_PaiWu_OpenSecs = Buffer_Data16;
												Save_Flag2 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[26] ; 
										if(Sys_Admin.ChaYa_WaterHigh_Enabled != Buffer_Data16)
											{
												Sys_Admin.ChaYa_WaterHigh_Enabled = Buffer_Data16;
												Save_Flag2 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[27] *256 + U4_Inf.RX_Data[28]; 
										if(Sys_Admin.ChaYa_WaterLow_Set != Buffer_Data16)
											{
												Sys_Admin.ChaYa_WaterLow_Set = Buffer_Data16;
												Save_Flag2 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[29] *256 + U4_Inf.RX_Data[30]; 
										if(Sys_Admin.ChaYa_WaterMid_Set != Buffer_Data16)
											{
												Sys_Admin.ChaYa_WaterMid_Set = Buffer_Data16;
												Save_Flag2 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[31] *256 + U4_Inf.RX_Data[32]; 
										if(Sys_Admin.ChaYa_WaterHigh_Set != Buffer_Data16)
											{
												Sys_Admin.ChaYa_WaterHigh_Set = Buffer_Data16;
												Save_Flag2 = OK;
											}

										 


										Buffer_Data16 = U4_Inf.RX_Data[33] *256 + U4_Inf.RX_Data[34]; 
										if(Sys_Admin.Fan_Speed_Check !=  Buffer_Data16)
											{
												Sys_Admin.Fan_Speed_Check = Buffer_Data16;
												Save_Flag2 = OK;
											}
										Buffer_Data16 = U4_Inf.RX_Data[35] *256 + U4_Inf.RX_Data[36]; 
										if(Sys_Admin.Water_BianPin_Enabled !=  Buffer_Data16)
											{
												Sys_Admin.Water_BianPin_Enabled = Buffer_Data16;
												Save_Flag2 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[37] *256 + U4_Inf.RX_Data[38]; 
										if(Sys_Admin.DeviceMaxPressureSet !=  Buffer_Data16)
											{
												Sys_Admin.DeviceMaxPressureSet = Buffer_Data16;
												Save_Flag2 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[39] *256 + U4_Inf.RX_Data[40]; 
										if(Sys_Admin.Device_Style !=  Buffer_Data16)
											{
												Sys_Admin.Device_Style = Buffer_Data16;
												Save_Flag2 = OK;
											}

										Buffer_Data16 = U4_Inf.RX_Data[41] *256 + U4_Inf.RX_Data[42]; 
										if(sys_config_data.Sys_Lock_Set !=  Buffer_Data16)
											{
												sys_config_data.Sys_Lock_Set = Buffer_Data16;
												Save_Flag1 = OK;
											}
										

										if(Save_Flag1)
											{
												Write_Internal_Flash();
												BEEP_TIME(50); 
											}
										if(Save_Flag2)
											{
												Write_Admin_Flash();
												BEEP_TIME(50); 
											}
										
										ModuBus4_Write0x10Response(Data_Address,Data_Length);
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


uint8 ModuBus4RTU_WriteResponse(uint16 address,uint16 Data16)
{
	uint16 check_sum = 0;
	
	U4_Inf.TX_Data[0] = U4_Inf.RX_Data[0];
	U4_Inf.TX_Data[1]= 0x06;

	
	U4_Inf.TX_Data[2] = address >> 8;    // 地址高字节
	U4_Inf.TX_Data[3] = address & 0x00FF;  //地址低字节
	

	U4_Inf.TX_Data[4] = Data16 >> 8;  //数据高字节
	U4_Inf.TX_Data[5] = Data16 & 0x00FF;   //数据低字节

	check_sum  = ModBusCRC16(U4_Inf.TX_Data,8);   //这个根据总字节数改变
	U4_Inf.TX_Data[6]  = check_sum >> 8 ;
	U4_Inf.TX_Data[7]  = check_sum & 0x00FF;

	Usart_SendStr_length(UART4,U4_Inf.TX_Data,8);

	return 0;
}







uint8 ModBus4_RTU_Read03(uint8 Target_Address,uint16 Data_Address,uint8 Data_Length )
{
		uint16 Check_Sum = 0;
		U4_Inf.TX_Data[0] = Target_Address;
		U4_Inf.TX_Data[1] = 0x03;

		U4_Inf.TX_Data[2]= Data_Address >> 8;
		U4_Inf.TX_Data[3]= Data_Address & 0x00FF;

		
		U4_Inf.TX_Data[4]= Data_Length >> 8;
		U4_Inf.TX_Data[5]= Data_Length & 0x00FF;

		
		Check_Sum = ModBusCRC16(U4_Inf.TX_Data,8);
		U4_Inf.TX_Data[6]= Check_Sum >> 8;
		U4_Inf.TX_Data[7]= Check_Sum & 0x00FF;
		
		Usart_SendStr_length(UART4,U4_Inf.TX_Data,8);


	return 0;
}

uint8 ModBus4_RTU_Write06(uint8 Target_Address,uint16 Data_Address,uint16 Data16)
{
		uint16 Check_Sum = 0;
		U4_Inf.TX_Data[0] = Target_Address;
		U4_Inf.TX_Data[1] = 0x06;

		U4_Inf.TX_Data[2]= Data_Address >> 8;
		U4_Inf.TX_Data[3]= Data_Address & 0x00FF;

		U4_Inf.TX_Data[4]= Data16 >> 8;
		U4_Inf.TX_Data[5]= Data16 & 0x00FF;

		Check_Sum = ModBusCRC16(U4_Inf.TX_Data,8);
		U4_Inf.TX_Data[6]= Check_Sum >> 8;
		U4_Inf.TX_Data[7]= Check_Sum & 0x00FF;

		Usart_SendStr_length(UART4,U4_Inf.TX_Data,8);

		

		return 0;
}

uint8 ModuBus4_Write0x10Response(uint16 address,uint16 Data16)
{
	uint16 check_sum = 0;
	
	U4_Inf.TX_Data[0] = 0x01;
	U4_Inf.TX_Data[1]= 0x10;

	
	U4_Inf.TX_Data[2] = address >> 8;    // 地址高字节
	U4_Inf.TX_Data[3] = address & 0x00FF;  //地址低字节
	

	U4_Inf.TX_Data[4] = Data16 >> 8;  //数据高字节
	U4_Inf.TX_Data[5] = Data16 & 0x00ff;   //数据低字节

	check_sum  = ModBusCRC16(U4_Inf.TX_Data,8);   //这个根据总字节数改变
	U4_Inf.TX_Data[6]  = check_sum >> 8 ;
	U4_Inf.TX_Data[7]  = check_sum & 0x00FF;

	Usart_SendStr_length(UART4,U4_Inf.TX_Data,8);

	return 0;
}





