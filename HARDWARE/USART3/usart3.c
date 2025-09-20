#include "usart3.h"
#include "stdarg.h"

  



  	 
///////////////////////////////////////USART3 printf支持部分//////////////////////////////////
//串口2,u2_printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
////////////////////////////////////////////////////////////////////////////////////////////////
void u3_printf(char* fmt,...)  
{  
  
}
///////////////////////////////////////USART2 初始化配置部分//////////////////////////////////	    
//功能：初始化IO 串口2
//输入参数
//bound:波特率
//输出参数
//无
//////////////////////////////////////////////////////////////////////////////////////////////	  
void uart3_init(u32 bound)
{  	 		 
	//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能GPIOB时钟
	USART_DeInit(USART3);  //复位串口3

     //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

    //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	        //根据指定的参数初始化VIC寄存器
  
    //USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;   //一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;  //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //收发模式

    USART_Init(USART3, &USART_InitStructure);   //初始化串口
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //开启中断
    USART_Cmd(USART3, ENABLE);                      //使能串口 
	
//	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);    //使能串口2的DMA发送
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF,1000);  //DMA1通道7,外设为串口2,存储器为USART2_TX_BUF,长度1000. 										  	
}




uint8 FuNiSen_Read_WaterDevice_Function(void)
{
	static uint8  Jump_Index = 0;
	static uint8 Jump_Count = 0;
	uint8 Address = 8; //默认本机内地址为 8 ，用于进水调节阀
	uint16 check_sum = 0;
	uint16 Percent = 0;


	//200ms调节一次
	if(sys_flag.WaterAJ_Flag == 0)
		return 0 ;

	sys_flag.WaterAJ_Flag = 0;
	sys_flag.waterSend_Flag = OK;

	if(sys_flag.Water_Percent <= 100)
		Percent =  sys_flag.Water_Percent * 10;
	else
		Percent = 0 ; //值不对则关闭

	Jump_Count ++;
	if(Jump_Count > 5)
		{
			Jump_Index = 0;
			Jump_Count = 0;
		}
	else
		Jump_Index = 1;
		
	
	switch (Jump_Index)
			{
			case 0: //读取实时的开度数值
					U3_Inf.TX_Data[0] = Address;
					U3_Inf.TX_Data[1] = 0x03;// 
					
					U3_Inf.TX_Data[2] = 0x00;
					U3_Inf.TX_Data[3] = 0x00;//地址

					U3_Inf.TX_Data[4] = 0x00;
					U3_Inf.TX_Data[5] = 0x01;//读取数据个数
				 
					check_sum  = ModBusCRC16(U3_Inf.TX_Data,8);
					U3_Inf.TX_Data[6]  = check_sum >> 8 ;
					U3_Inf.TX_Data[7]  = check_sum & 0x00FF;
					
					Usart_SendStr_length(USART3,U3_Inf.TX_Data,8);
				   
					//Jump_Index = 1;
					break;
			case 1://写入开度的值
					//Jump_Index = 0;

					U3_Inf.TX_Data[0] = Address;
					U3_Inf.TX_Data[1] = 0x06;// 
					
					U3_Inf.TX_Data[2] = 0x00;
					U3_Inf.TX_Data[3] = 0x01;//地址

					U3_Inf.TX_Data[4] = Percent >> 8;
					U3_Inf.TX_Data[5] = Percent & 0x00FF;//写入的开度值
				 
					check_sum  = ModBusCRC16(U3_Inf.TX_Data,8);
					U3_Inf.TX_Data[6]  = check_sum >> 8 ;
					U3_Inf.TX_Data[7]  = check_sum & 0x00FF;			
					Usart_SendStr_length(USART3,U3_Inf.TX_Data,8);
					break;
			default:
					Jump_Index = 0;
					break;
			}
	
	return 0;
}

uint8 FuNiSen_RecData_WaterDevice_Processs(void)
{
	uint16 checksum = 0;
	uint8 address = 0;
	uint8 i = 0;
	uint8 command = 0;
	uint8 Length = 0;
	uint16 Value_Buffer = 0;

	if(sys_flag.waterSend_Count >= 10)
	   sys_flag.Error_Code = Error19_SupplyWater_UNtalk;
	
	if(U3_Inf.Recive_Ok_Flag)
		{
			U3_Inf.Recive_Ok_Flag = 0;//不能少哦
			 //关闭中断
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); 
			 
			checksum  = U3_Inf.RX_Data[U3_Inf.RX_Length - 2] * 256 + U3_Inf.RX_Data[U3_Inf.RX_Length - 1];

			address = U3_Inf.RX_Data[0]; 
			command = U3_Inf.RX_Data[1];
			Length = U3_Inf.RX_Data[2];
		
			//01 03 06 07 CF 07 CF 00 00 45 99
			//01 06 00 02 07 CF 6A 6E 
			if(address == 0x08 &&checksum == ModBusCRC16(U3_Inf.RX_Data,U3_Inf.RX_Length) )
				{
					 sys_flag.waterSend_Flag = FALSE;
					 sys_flag.waterSend_Count = 0;
					if(command == 0x03 && Length == 0x02)
						{
						   //读取当前阀的状态
						  
						   
							Value_Buffer = U3_Inf.RX_Data[3] * 256 + U3_Inf.RX_Data[4];
							if(Value_Buffer == 0xEA)
								sys_flag.Water_Error_Code = OK; //故障状态
							else
							   sys_flag.Water_Error_Code = 0;

							if(sys_flag.Water_Error_Code == OK)
							   sys_flag.Error_Code = Error18_SupplyWater_Error;
							
						}
					
				}
				

		//对接收缓冲器清零
			for( i = 0; i < 100;i++ )
				U3_Inf.RX_Data[i] = 0x00;
		
		//重新开启中断
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
			
		}
	
	return 0;	
}



void ModBus_Uart3_LocalRX_Communication(void)
{
		
		uint8  i = 0;	
		uint8  Target_Address = 0;
		uint8 command = 0;
		uint8 Length = 0;
		uint16 Value_Buffer = 0;
		float Value20mA = 0;
		
		uint16 checksum = 0;

		
		if(sys_flag.Address_Number == 0)
			{
				//主机检查各分机是否在线
				for(i = 1; i < 13; i++)
					{
						if(SlaveG[i].Send_Flag > 30)//发送6次未回应，则失联
							{
								SlaveG[i].Alive_Flag = FALSE;
								LCD10JZ[i].DLCD.Device_State = 0;   
								memset(&LCD10JZ[i].Datas,0,sizeof(LCD10JZ[i].Datas)); //然后对数据清零
							}
					}

				if(SlaveG[2].Alive_Flag == 0)
					{
						 Water_State.Cstate_Flag = 0;
						Water_State.CSignal = 0 ;  //进水阀 请求状态
					}

				
				
			}

		if(Sys_Admin.Water_BianPin_Enabled)
			{
				if(sys_flag.waterSend_Count >= 10)
					sys_flag.Error_Code = Error19_SupplyWater_UNtalk;
			}
		else
			{
				sys_flag.waterSend_Count = 0;	
			}
		
		
		if(U3_Inf.Recive_Ok_Flag)
			{
				U3_Inf.Recive_Ok_Flag = 0;//不能少哦
				 //关闭中断
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); 
				 
				checksum  = U3_Inf.RX_Data[U3_Inf.RX_Length - 2] * 256 + U3_Inf.RX_Data[U3_Inf.RX_Length - 1];
				command = U3_Inf.RX_Data[1];
				Length = U3_Inf.RX_Data[2];
				
			
				if(checksum == ModBusCRC16(U3_Inf.RX_Data,U3_Inf.RX_Length))
					{
						Target_Address = U3_Inf.RX_Data[0];
						if(sys_flag.Address_Number == 0)
							{
								//必须是主机才有权利接收处理数据
								if(Target_Address < 8)
								{
									Modbus3_UnionRx_DataProcess(U3_Inf.RX_Data[1],Target_Address);
								}
							}
						

						if(Target_Address == 8) //变频进水阀
							{
								 sys_flag.waterSend_Flag = FALSE;
								 sys_flag.waterSend_Count = 0;
									if(command == 0x03 && Length == 0x02)
										{
										   //读取当前阀的状态
											Value_Buffer = U3_Inf.RX_Data[3] * 256 + U3_Inf.RX_Data[4];
											if(Value_Buffer == 0xEA)
												sys_flag.Water_Error_Code = OK; //故障状态
											else
											   sys_flag.Water_Error_Code = 0;

											if(sys_flag.Water_Error_Code == OK)
											   sys_flag.Error_Code = Error18_SupplyWater_Error;
											
										}
				
							}



						if(Target_Address == 10) //两路4-20mA扩展模块
							{
								 
									if(U3_Inf.RX_Data[1] == 0x04 )
										{
										   //读取当前阀的状态
										   
											Value_Buffer = U3_Inf.RX_Data[3] * 256 + U3_Inf.RX_Data[4];

											if(Value_Buffer >= 819)
												{
													//Value_Buffer  = Value_Buffer - 819;  //4mA  = 819  20mA = 4095
													Value20mA = (float)Value_Buffer * 20 /4095 - 4;  //换算成实际的mA值
												}
										   
											if(Value_Buffer > 0 )
												{
													sys_flag.ChaYaWater_Value = Value20mA * 1500 / 16;  // 20 = 20mA , 75 = 1500量程/ 16mA
													 
												}
											else
												{
													sys_flag.ChaYaWater_Value = 0;
												}
										//	u1_printf("\n* 返回值 = %d\n",Value_Buffer);//126
										//	u1_printf("\n* 返回值4-20mA = %f\n",Value20mA);
										//	u1_printf("\n* 液位高度 = %d\n",sys_flag.ChaYaWater_Value);
											
											
										}
				
							}
						
							

					}
					
			 
				
			//对接收缓冲器清零
				for( i = 0; i < 100;i++ )
					U3_Inf.RX_Data[i] = 0x00;
			
			//重新开启中断
				USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
				
			}
}


uint8 Modbus3_UnionTx_Communication(void)
{
	static uint8 Address = 1;
	uint16 check_sum = 0;
	//	SlaveG[1].Alive_Flag  
	//	JiZu[1].Slave_D.Device_State  //这用于跟屏数据的同步


	if(Sys_Admin.Device_Style == 0 || Sys_Admin.Device_Style == 1)
		{
			switch(Address)
				{
					case 1:
							if(Sys_Admin.Water_BianPin_Enabled)
			 					{
				 					if(sys_flag.Special_100msFlag)
					 					{
					 						FuNiSen_Read_WaterDevice_Function();
											sys_flag.Special_100msFlag = 0;
											Address++;
					 					}
								}
							else
								{
									Address++;
								}

							break;
					case 2:
							if(sys_flag.Special_100msFlag)
								{
									sys_flag.Special_100msFlag = 0;
									Address = 1;
								}

							break;

					default:
							Address = 1;

							break;
				}
		}

	//明确设备类型
	if(Sys_Admin.Device_Style == 2 || Sys_Admin.Device_Style == 3)
		{
			
			switch (Address)
			{
				case 1:
							SlaveG[1].Alive_Flag = OK;
							SlaveG[1].Send_Flag  = 0;
							SlaveG[1].UnBack_time = 0;
							if(sys_flag.Error_Code == 0)
								SlaveG[1].Ok_Flag = OK;   //对些判定变量清零

							LCD10JZ[1].DLCD.Error_Code = sys_flag.Error_Code;
							
							if(SlaveG[1].Startclose_Sendflag)
								{
									SlaveG[1].Startclose_Sendflag = 0; //先清除标志
									if(SlaveG[1].Startclose_Data == 0)
										{
											//处于联控，要关闭设备
											if(sys_data.Data_10H == 2)
												sys_close_cmd();
										}
									if(SlaveG[1].Startclose_Data == 1)
										{
											//处于联控，要启动设备
											if(sys_data.Data_10H == 0)
												sys_start_cmd();
										}
								}
							if(sys_data.Data_10H == 0)
								LCD10JZ[1].DLCD.Device_State = 1;
							if(sys_data.Data_10H == 2)
								LCD10JZ[1].DLCD.Device_State = 2;
							if(sys_flag.Error_Code )
								LCD10JZ[1].DLCD.Device_State = 3;
							
							LCD10D.DLCD.Device_State = LCD10JZ[1].DLCD.Device_State;
							LCD10JZ[1].DLCD.YunXu_Flag = LCD10D.DLCD.YunXu_Flag ;
							LCD10JZ[1].DLCD.Work_Time = LCD10D.DLCD.Work_Time ;
							LCD10JZ[1].DLCD.Air_Power = LCD10D.DLCD.Air_Power;

							LCD10JZ[1].DLCD.Max_Work_Power = LCD10D.DLCD.Max_Work_Power;

							LCD10JZ[1].DLCD.InPut_Data = LCD10D.DLCD.Input_Data;
							
							
							LCD10JZ[1].DLCD.Steam_Pressure  = LCD10D.DLCD.Steam_Pressure;
							LCD10JZ[1].DLCD.Inside_High_Pressure = LCD10D.DLCD.Inside_High_Pressure;

							//规避从机
							if(sys_flag.Address_Number == 0)
								{
									Address++;
								}
							else
								{
									Address = 4;
								}
							
						
						break;
				case 2:
					
						if(JiZu_ReadAndWrite_Function(Address))
							Address++;
						
						break;
				case 3:
					
						if(JiZu_ReadAndWrite_Function(Address))
							{
								if(Sys_Admin.Water_BianPin_Enabled)//主机的变频功能
									{
										Address++;
									}
								else
									{
										Address = 1;
										if(Sys_Admin.ChaYa_WaterHigh_Enabled)
											Address = 5;
									}
									
							}
						
						break;
						
				case 4:
						if(Sys_Admin.Water_BianPin_Enabled)
				 			{
				 				if(sys_flag.Special_100msFlag)
				 					{
				 						FuNiSen_Read_WaterDevice_Function();
										sys_flag.Special_100msFlag = 0;
										Address++;
				 					}
				 					
							}
						else
							{
								Address = 1;
							}

						break;
				case 5:
						if(sys_flag.Special_100msFlag)
							{
								sys_flag.Special_100msFlag = 0;
								Address = 1;
								if(Sys_Admin.ChaYa_WaterHigh_Enabled)
									{
										U3_Inf.TX_Data[0] = 10; // 地址默认为10
										U3_Inf.TX_Data[1] = 0x04;// 
										
										U3_Inf.TX_Data[2] = 0x00;
										U3_Inf.TX_Data[3] = 0x00;//地址

										U3_Inf.TX_Data[4] = 0x00;
										U3_Inf.TX_Data[5] = 0x02;//读取数据个数
									 
										check_sum  = ModBusCRC16(U3_Inf.TX_Data,8);
										U3_Inf.TX_Data[6]  = check_sum >> 8 ;
										U3_Inf.TX_Data[7]  = check_sum & 0x00FF;
										
										Usart_SendStr_length(USART3,U3_Inf.TX_Data,8);
										
										Address = 6;
									}
								
							}
						break;
				case 6:
						if(sys_flag.Special_100msFlag)
							{
								sys_flag.Special_100msFlag = 0;
								Address = 1;
								
								
							}
						break;
			
				
				

				default :
						Address = 1;
						break;
			}
		}
		
	
		

		return 0;
}

uint8 Modbus3_UnionRx_DataProcess(uint8 Cmd,uint8 address)
{
		uint8 Data_Length = 0;
		uint16 Data_Address = 0;
		float  Buffer_Float = 0;

		uint16 Value_Buffer = 0;
	//接收到从机的数据进行处理
		
	SlaveG[address].Send_Flag = 0;  //清楚发送标志
	SlaveG[address].Alive_Flag = OK;
	if(Cmd == 0x03)
		{
			Data_Length = U3_Inf.RX_Data[2];

			if(Data_Length == 36)
				{
					if(SlaveG[address].Startclose_Sendflag == 0) //解决命令下发和实际不一致，等发送次数完成
						{
						//	if(U3_Inf.RX_Data[4]  == 2)   //启动或待机的标志
						//		JiZu[address].Slave_D.StartFlag = OK;  //  ***********************????????????
						//	else
						//		JiZu[address].Slave_D.StartFlag = 0;  
						}
					

					if(SlaveG[address].Send_Flag > 15)//发送6次未回应，则失联
						{
							LCD10JZ[address].DLCD.Device_State = 0;   //一会移植到外部去
						}
					else
						{
							if(U3_Inf.RX_Data[4]  == 2)
								LCD10JZ[address].DLCD.Device_State = 2;  //四种状态的切换
							if(U3_Inf.RX_Data[4]  == 0)
								LCD10JZ[address].DLCD.Device_State = 1;
							if(U3_Inf.RX_Data[16])
								LCD10JZ[address].DLCD.Device_State = 3;  //故障状态
						}
					LCD10JZ[address].DLCD.Error_Code = U3_Inf.RX_Data[16];  //第2个———— 故障代码
					LCD10JZ[address].DLCD.Air_Power = U3_Inf.RX_Data[24];	//第3个———— 风机功率
					LCD10JZ[address].DLCD.Flame_State = U3_Inf.RX_Data[18];	//第4个———— 火焰
					LCD10JZ[address].DLCD.Pump_State = U3_Inf.RX_Data[28];	//第5个———— 水泵的状态
					LCD10JZ[address].DLCD.Water_State = U3_Inf.RX_Data[10];	//第6个———— 水位状态
					LCD10JZ[address].DLCD.Paiwu_State = U3_Inf.RX_Data[30];	//第7个———— 排污阀状态
					LCD10JZ[address].DLCD.Air_State = U3_Inf.RX_Data[26];	//第8个———— 风机状态
					LCD10JZ[address].DLCD.lianxuFa_State = U3_Inf.RX_Data[32];	//第9个———— 连续排污阀状态
					LCD10JZ[address].DLCD.Air_Speed = U3_Inf.RX_Data[19] * 256  + U3_Inf.RX_Data[20];	//第9个———— 风机转速

					
					sys_flag.Cong_ChaYaWater_Value = U3_Inf.RX_Data[35] * 256  + U3_Inf.RX_Data[36];// **********************
					LCD10JZ[address].DLCD.InPut_Data = U3_Inf.RX_Data[37] * 256  + U3_Inf.RX_Data[38];

					LCD10JZ[address].DLCD.Dian_Huo_Power = SlaveG[address].Fire_Power; 
					LCD10JZ[address].DLCD.Max_Work_Power = SlaveG[address].Max_Power; 
					LCD10JZ[address].DLCD.Danger_Smoke_Value = SlaveG[address].Smoke_Protect; 
					LCD10JZ[address].DLCD.Inside_WenDu_ProtectValue = SlaveG[address].Inside_WenDu_ProtectValue; 

					LCD10JZ[address].DLCD.YunXu_Flag = SlaveG[address].Key_Power; 
					if(address == 2)
						{
							//只针对于双拼机型的补水做处理
							Water_State.Cstate_Flag = LCD10JZ[address].DLCD.Pump_State;
							Water_State.CSignal = U3_Inf.RX_Data[34];  //进水阀 请求状态
						}
					
					

					Buffer_Float = U3_Inf.RX_Data[5] * 256  + U3_Inf.RX_Data[6];
					if(Buffer_Float > 0)  //将压力转换为浮点数
						LCD10JZ[address].DLCD.Steam_Pressure  = Buffer_Float / 100;   //蒸汽压力
					else
						LCD10JZ[address].DLCD.Steam_Pressure = 0;

					Buffer_Float = U3_Inf.RX_Data[21] * 256  + U3_Inf.RX_Data[22];
					if(Buffer_Float > 0)  //将压力转换为浮点数
						LCD10JZ[address].DLCD.Inside_High_Pressure  = Buffer_Float / 100;   //内部高压侧压力
					else
						LCD10JZ[address].DLCD.Inside_High_Pressure = 0;
						
					Buffer_Float = U3_Inf.RX_Data[13] * 256  + U3_Inf.RX_Data[14];
					LCD10JZ[address].DLCD.LuNei_WenDu = Buffer_Float;              //内部温度

					Buffer_Float = U3_Inf.RX_Data[11] * 256  +U3_Inf.RX_Data[12] ;
					LCD10JZ[address].DLCD.Smoke_WenDu = Buffer_Float;              //排烟温度
					
				}

		
		}

	if(Cmd == 0x06)
		{
				Data_Address = U3_Inf.RX_Data[2]*256 + U3_Inf.RX_Data[3] ;
				Value_Buffer  = U3_Inf.RX_Data[4]*256 + U3_Inf.RX_Data[5] ;
				if(Data_Address == 0x0064)
					{
						SlaveG[address].Startclose_Sendflag = 0;

						if(Value_Buffer == 1 || Value_Buffer == 2)
							{
								//代表开机的指令反馈，
								LCD10JZ[address].DLCD.Device_State = 2; //说明该机组已经收到指令，反馈数据还没过来
							}

						if(Value_Buffer == 0)
							{
								//代表关机的指令反馈，
								LCD10JZ[address].DLCD.Device_State = 1; //说明该机组已经收到指令，反馈数据还没过来
							}

						if(Value_Buffer == 3)
							{
								//代表手动的指令反馈，
								LCD10JZ[address].DLCD.Device_State = 3; //说明该机组已经收到指令，反馈数据还没过来
							}

						
					}

				if(Data_Address == 0x006B)
					{
						SlaveG[address].ResetError_Flag = 0;
					}

				if(Data_Address == 111)  //风机功率调节手动测试   111根据串口4的地址顺序
					{
						SlaveG[address].AirPower_Flag = 0;
					}

				if(Data_Address == 112)  //风机手动测试   112根据串口4的地址顺序
					{
						SlaveG[address].AirOpen_Flag = 0;
					}
				if(Data_Address == 113)  //水泵手动测试   113根据串口4的地址顺序
					{
						SlaveG[address].PumpOpen_Flag = 0;
					}
				if(Data_Address == 114)  //排污阀手动测试   114根据串口4的地址顺序
					{
						SlaveG[address].PaiWuOpen_Flag = 0;
					}
				if(Data_Address == 115)  //LianXu排污测试   115根据串口4的地址顺序
					{
						SlaveG[address].LianxuFa_Flag = 0;
					}
				if(Data_Address == 117)  //自动大排污一次   117根据串口4的地址顺序
					{
						SlaveG[address].Paiwu_Flag = 0;
					}
		}
		


		return 0;
}


uint8 JiZu_ReadAndWrite_Function(uint8 address)
{
		
		static uint8 index  = 0;
		uint8 Tx_Index = 0;  //发送方式的选择
		uint8 return_value = 0;
		 


	//	SlaveG[1].Alive_Flag  
	//	JiZu[1].Slave_D.Device_State  //这用于跟屏数据的同步
		switch (index)
			{
			case 0:
					Tx_Index = 0; //默认是等于0 

					if(SlaveG[address].Startclose_Sendflag)
						{
							SlaveG[address].Startclose_Sendflag--; //对发送次数递减
							Tx_Index = 1;
						}
					if(SlaveG[address].AirOpen_Flag && Tx_Index == 0)  //每次只处理一种通信控制
						{
							SlaveG[address].AirOpen_Flag--; //对发送次数递减
							Tx_Index = 2;
						}

					if(SlaveG[address].AirPower_Flag && Tx_Index == 0)  //每次只处理一种通信控制
						{
							SlaveG[address].AirPower_Flag--; //对发送次数递减
							Tx_Index = 3;
						}
					if(SlaveG[address].PumpOpen_Flag && Tx_Index == 0)  //每次只处理一种通信控制
						{
							SlaveG[address].PumpOpen_Flag--; //对发送次数递减
							Tx_Index = 4;
						}
					if(SlaveG[address].PaiWuOpen_Flag && Tx_Index == 0)  //每次只处理一种通信控制
						{
							SlaveG[address].PaiWuOpen_Flag--; //对发送次数递减
							Tx_Index = 5;
						}
					if(SlaveG[address].LianxuFa_Flag && Tx_Index == 0)  //每次只处理一种通信控制
						{
							SlaveG[address].LianxuFa_Flag--; //对发送次数递减
							Tx_Index = 6;
						}

					if(SlaveG[address].ResetError_Flag && Tx_Index == 0)  //每次只处理一种通信控制
						{
							SlaveG[address].ResetError_Flag--; //对发送次数递减
							Tx_Index = 7;
						}

					if(SlaveG[address].Paiwu_Flag && Tx_Index == 0)  //每次只处理一种通信控制
						{
							SlaveG[address].Paiwu_Flag--; //对发送次数递减
							Tx_Index = 9;
						}

					
					if(Tx_Index == 0)
						{
							if(SlaveG[address].Send_Count > 10)
								{
									SlaveG[address].Send_Count = 0;
									Tx_Index = 8;                 //写一组数据进去
								}
						}
					

					switch (Tx_Index)
						{
							case 0 :
									SlaveG[address].Send_Count++;
									ModBus3_RTU_Read03(address,100,18); //这个是默认读取数据的指令
									break;
							case 1:	  //启动或关闭机器,进入手动模式的指令
									ModBus3_RTU_Write06(address,100,SlaveG[address].Startclose_Data);
									break;
							case 2:  //手动开启风机或关闭风机
									ModBus3_RTU_Write06(address,112,SlaveG[address].AirOpen_Data);	
									break;
							case 3: //风机功率的调节
									ModBus3_RTU_Write06(address,111,SlaveG[address].AirPower_Data);
									break;
							case 4:  //手动开启水泵或关闭
									ModBus3_RTU_Write06(address,113,SlaveG[address].PumpOpen_Data);	
									break;
							case 5:  //手动开启排污阀或关闭
									ModBus3_RTU_Write06(address,114,SlaveG[address].PaiWuOpen_Data);	
									break;
							case 6:  //手动开启LIANXU排污阀或关闭
									ModBus3_RTU_Write06(address,115,SlaveG[address].LianxuFa_Data);	
									break;

							case 7:  //故障复位指令
									ModBus3_RTU_Write06(address,107,SlaveG[address].ResetError_Data);	
									break;
							case 8:

									ModBus3_RTU_Write10(address,150);  //写入18个数据
									break;

							case 9:  //主机命令从机开始自动大排污一次
									ModBus3_RTU_Write06(address,117,SlaveG[address].Paiwu_Data);	
									break;
							

							default:
									Tx_Index = 0;

									break;
						}

								
					SlaveG[address].Send_Flag ++; //对发送进行计数
					sys_flag.Special_100msFlag = 0;
					
					index++;

					break;
			case 1:
					if(sys_flag.Special_100msFlag)
						{
							sys_flag.Special_100msFlag = 0;
							
							index = 0;
							return_value = OK;
							
						}

					break;
			default:
					index = 0;
					break;
			}


		return return_value;
}

uint8 ModBus3_RTU_Read03(uint8 Target_Address,uint16 Data_Address,uint8 Data_Length )
{
		uint16 Check_Sum = 0;
		U3_Inf.TX_Data[0] = Target_Address;
		U3_Inf.TX_Data[1] = 0x03;

		U3_Inf.TX_Data[2]= Data_Address >> 8;
		U3_Inf.TX_Data[3]= Data_Address & 0x00FF;

		
		U3_Inf.TX_Data[4]= Data_Length >> 8;
		U3_Inf.TX_Data[5]= Data_Length & 0x00FF;

		
		Check_Sum = ModBusCRC16(U3_Inf.TX_Data,8);
		U3_Inf.TX_Data[6]= Check_Sum >> 8;
		U3_Inf.TX_Data[7]= Check_Sum & 0x00FF;
		
		Usart_SendStr_length(USART3,U3_Inf.TX_Data,8);


	return 0;
}

uint8 ModBus3_RTU_Write06(uint8 Target_Address,uint16 Data_Address,uint16 Data16)
{
		uint16 Check_Sum = 0;
		U3_Inf.TX_Data[0] = Target_Address;
		U3_Inf.TX_Data[1] = 0x06;

		U3_Inf.TX_Data[2]= Data_Address >> 8;
		U3_Inf.TX_Data[3]= Data_Address & 0x00FF;

		U3_Inf.TX_Data[4]= Data16 >> 8;
		U3_Inf.TX_Data[5]= Data16 & 0x00FF;

		Check_Sum = ModBusCRC16(U3_Inf.TX_Data,8);
		U3_Inf.TX_Data[6]= Check_Sum >> 8;
		U3_Inf.TX_Data[7]= Check_Sum & 0x00FF;

		Usart_SendStr_length(USART3,U3_Inf.TX_Data,8);

		

		return 0;
}

uint8 ModBus3_RTU_Write10(uint8 Target_Address,uint16 Data_Address)
{
		uint16 Check_Sum = 0;
		uint8 index = 0;
		U3_Inf.TX_Data[0] = Target_Address;
		U3_Inf.TX_Data[1] = 0x10;

		U3_Inf.TX_Data[2]= Data_Address >> 8;
		U3_Inf.TX_Data[3]= Data_Address & 0x00FF;

		U3_Inf.TX_Data[4]=0 ;
		U3_Inf.TX_Data[5]= 18 ;  //数据个数得修改

		U3_Inf.TX_Data[6]= 36 ; 
		
		U3_Inf.TX_Data[7] = sys_config_data.zhuan_huan_temperture_value >> 8;
		U3_Inf.TX_Data[8] = sys_config_data.zhuan_huan_temperture_value & 0x00FF;// 1目标蒸汽压力

		U3_Inf.TX_Data[9] = sys_config_data.Auto_stop_pressure >> 8;
		U3_Inf.TX_Data[10] = sys_config_data.Auto_stop_pressure & 0x00FF;  //2 停止压力

		U3_Inf.TX_Data[11] = sys_config_data.Auto_start_pressure >> 8;
		U3_Inf.TX_Data[12] = sys_config_data.Auto_start_pressure & 0x00FF; //3 启动压力

		U3_Inf.TX_Data[13] = 0x00;
		U3_Inf.TX_Data[14] = SlaveG[Target_Address].Fire_Power;//4 点火功率 机器不一致

		U3_Inf.TX_Data[15] = 0x00;
		U3_Inf.TX_Data[16] = SlaveG[Target_Address].Max_Power;//5 最大运行功率 机器不一致

		U3_Inf.TX_Data[17] = SlaveG[Target_Address].Inside_WenDu_ProtectValue>> 8;
		U3_Inf.TX_Data[18] = SlaveG[Target_Address].Inside_WenDu_ProtectValue & 0x00FF;//6内温保护值，机器不一致

		U3_Inf.TX_Data[19] =  0;
		U3_Inf.TX_Data[20] = SlaveG[Target_Address].Smoke_Protect ;//7排烟温度报警值 机器不一致

		U3_Inf.TX_Data[21] = 0;
		U3_Inf.TX_Data[22] = Sys_Admin.Water_Max_Percent;//8 变频补水最大开度，所有机器一致

		U3_Inf.TX_Data[23] = Sys_Admin.LianXu_PaiWu_DelayTime;  //连续排污周期分钟
		U3_Inf.TX_Data[24] = Sys_Admin.LianXu_PaiWu_OpenSecs;//9连续排污开启几秒，这个所有机器一致



		
		U3_Inf.TX_Data[25] =0 ;
		U3_Inf.TX_Data[26] = Sys_Admin.ChaYa_WaterHigh_Enabled ; // 10	 

		U3_Inf.TX_Data[27] = Sys_Admin.ChaYa_WaterLow_Set >> 8;
		U3_Inf.TX_Data[28] = Sys_Admin.ChaYa_WaterLow_Set & 0x00FF;//11  

		U3_Inf.TX_Data[29] = Sys_Admin.ChaYa_WaterMid_Set >> 8;
		U3_Inf.TX_Data[30] = Sys_Admin.ChaYa_WaterMid_Set & 0X00FF; //12 


		U3_Inf.TX_Data[31] = Sys_Admin.ChaYa_WaterHigh_Set >> 8;
		U3_Inf.TX_Data[32] = Sys_Admin.ChaYa_WaterHigh_Set & 0X00FF;//13 

		U3_Inf.TX_Data[33] = 0;
		U3_Inf.TX_Data[34] = Sys_Admin.Fan_Speed_Check;//14 

		U3_Inf.TX_Data[35] = 0;
		U3_Inf.TX_Data[36] = Sys_Admin.Water_BianPin_Enabled;//15  各种开关的标志位，本体温度保护，变频补水开启，风速检测
		
		
		U3_Inf.TX_Data[37] = 0;
		U3_Inf.TX_Data[38] = Sys_Admin.DeviceMaxPressureSet;//16  额定蒸汽压力
		
		U3_Inf.TX_Data[39] = 0;
		U3_Inf.TX_Data[40] = Sys_Admin.Device_Style ;//17  设备类型

		U3_Inf.TX_Data[41] = 0;
		U3_Inf.TX_Data[42] = sys_config_data.Sys_Lock_Set;//18   设备锁机

		Check_Sum = ModBusCRC16(U3_Inf.TX_Data,45);
		U3_Inf.TX_Data[43]= Check_Sum >> 8;
		U3_Inf.TX_Data[44]= Check_Sum & 0x00FF;

		Usart_SendStr_length(USART3,U3_Inf.TX_Data,45);

		

		return 0;
}


uint8 Union_MuxJiZu_Control_Function(void)
{
	uint8 Address = 0;
	uint8 AliveOk_Numbres = 0;  //在线OK的设备数量
	uint8 Already_WorkNumbers = 0; //对已经在运行的设备进行统计
	uint8 AliveOK[13] = {0};    //对在线设备的地址进行统计,1---10
	
	uint8 Need_flag = 0;
	uint8 Loss_flag = 0;
	uint8 Value_Buffer = 0;

	uint32 Max_time = 0;
	uint32 Max_Address = 0;
	uint32 Min_Time = 0;
	uint32 Min_Address = 0;
	float Max_Pressure = 0;
	uint16 Pressure_uint16 = 0;
	
	uint32 Value_Buffer2 = 0;
	uint8 index = 0;
	

	static uint32 Loop_time = 0;
	uint8 Loop_Flag = 0;

static 	uint16 Period_Check = 0;




	
	if(UnionD.UnionStartFlag == 0 ||  UnionD.UnionStartFlag == 3)  //联控标志取消了，则直接返回
		{
			Loop_Flag = 0;
			Loop_time = 0;

			//得5秒检查一次，外部设备的运行情况，防止没有将设备关掉
			if(UnionD.UnionStartFlag == 0)
				{
					if(sys_flag.Union_1A_Sec)
					{
						sys_flag.Union_1A_Sec = 0;   //5秒检查一次
						
						for(Address = 2; Address <= 10; Address ++)
							{
								if(LCD10JZ[Address].DLCD.Device_State == 2)
									{
									 	//设备仍然在运行
									 	SlaveG[Address].Startclose_Sendflag = 3; //连续发三次
										SlaveG[Address].Startclose_Data = 0;//关闭该机器
									}
							}
						
						
					}
				}
			

			//在这判定运行状态？ 重新发送控制指令？   需要验证  各从机长时间未接收到指令同样关闭运行
			
			return 0;
		}
		

	
  //第一步； 找到所有在线的机器，确定在线机器的台数
  if(sys_flag.Union_1_Sec)
  	{
  		sys_flag.Union_1_Sec = 0; //每秒检查一次
		
		for(Address = 1; Address <= 10; Address ++)
				{
					if(LCD10JZ[Address].DLCD.YunXu_Flag) //取联控的标志
						{
							if(SlaveG[Address].Alive_Flag)//首先在线
								{
									if(LCD10JZ[Address].DLCD.Error_Code == 0) //没有故障
										{
											AliveOk_Numbres ++;
											AliveOK[AliveOk_Numbres] = Address;

											//找出待机的，谁工作时间最少，用于下次启动使用
											if(LCD10JZ[Address].DLCD.Device_State == 1)
												{
													if(Min_Address == 0)//初始值
														{
															Min_Address = Address;
															Min_Time= LCD10JZ[Address].DLCD.Work_Time;
														}
													else
														{
															if(Min_Time > LCD10JZ[Address].DLCD.Work_Time)
																{
																	Min_Time = LCD10JZ[Address].DLCD.Work_Time;
																	Min_Address = Address;
																}
														}
													
												}
										}

									
		
									if(LCD10JZ[Address].DLCD.Device_State == 2)
										{
											Already_WorkNumbers ++;

											//检查在运行的谁时间最长
											if(Max_Address == 0)
												{
													Max_Address = Address;
													Max_time = LCD10JZ[Address].DLCD.Work_Time;
												}
											else
												{
													if(Max_time < LCD10JZ[Address].DLCD.Work_Time )
														{
															Max_Address = Address;
															Max_time = SlaveG[Address].Work_Time;
														}
												}

											//对功率的保持时间进行计算
											if(LCD10JZ[Address].DLCD.Air_Power >=(LCD10JZ[Address].DLCD.Max_Work_Power ) )
												{
													SlaveG[Address].Big_time ++;
													SlaveG[Address].Small_time = 0;
													SlaveG[Address].Zero_time = 0;
												}
												

											if(LCD10JZ[Address].DLCD.Air_Power <= 45 && LCD10JZ[Address].DLCD.Air_Power >=25 )
												{
													//功率小，且压力要大于等于设定压力，才能对小功率计数
													if(Temperature_Data.Pressure_Value >= sys_config_data.zhuan_huan_temperture_value)
														{
															SlaveG[Address].Small_time ++;
															SlaveG[Address].Big_time = 0;
															SlaveG[Address].Zero_time = 0;
														}
													
												}

											if(LCD10JZ[Address].DLCD.Air_Power > 45  &&  LCD10JZ[Address].DLCD.Air_Power < LCD10JZ[Address].DLCD.Max_Work_Power )
												{
													//处于中间地带，则不算大，也不算小，两者一起运行
													
													SlaveG[Address].Small_time  = 0;
													SlaveG[Address].Big_time = 0;
													SlaveG[Address].Zero_time = 0;
												}

											if(LCD10JZ[Address].DLCD.Air_Power == 0) //超压停炉的状态
												{
													SlaveG[Address].Big_time = 0;
													SlaveG[Address].Small_time = 0;
													SlaveG[Address].Zero_time ++;
												}
											//检查大功率运行的时间 ,全满负荷，肯定得开，有2个以上的小负荷，就得关一个
										}
								}
							else
								{
									SlaveG[Address].Small_time= 0;
									SlaveG[Address].Big_time = 0;
									SlaveG[Address].Zero_time = 0;
									
									/*当权限在运行状态时被取消，则主动关闭相应的从机*/
									if(LCD10JZ[Address].DLCD.Device_State == 2)
										{
											SlaveG[Address].Startclose_Sendflag = 3; //连续发三次
											SlaveG[Address].Startclose_Data = 0;//关闭该机器
										}
								}
						}
					else
						{
							SlaveG[Address].Small_time= 0;
							SlaveG[Address].Big_time = 0;
							SlaveG[Address].Zero_time = 0;
							if(LCD10JZ[Address].DLCD.Device_State == 2)
								{
									SlaveG[Address].Startclose_Sendflag = 3; //连续发三次
									SlaveG[Address].Startclose_Data = 0;//关闭该机器
								}
						}
					
				}

			
			for(Address = 1; Address <= 10; Address ++)
				{
					
					//找出运行中的最大压力值
					if(LCD10JZ[Address].DLCD.Steam_Pressure > Max_Pressure)
						{
							if(LCD10JZ[Address].DLCD.Steam_Pressure < 9.99)  //防止压力变送器异常，数值无效
								Max_Pressure = LCD10JZ[Address].DLCD.Steam_Pressure ;
						}
						
				}

			
			UnionD.Need_Numbers  = 1 ;  //至少保留一台
			
			if(Max_Pressure < 0.35)  //压力小于0.30Mpa,需要快速启动
				{
					if(AliveOk_Numbres <= 2)
						UnionD.Need_Numbers = AliveOk_Numbres;

					if(AliveOk_Numbres > 2)
						UnionD.Need_Numbers = AliveOk_Numbres - 1;
						
				}
			else
				{
					if(AliveOk_Numbres > 1)
						UnionD.Need_Numbers = AliveOk_Numbres - 1; //三台则比需要的少一台  
				}
		
			
			
			
			
		
			Period_Check ++;
			if(Period_Check >= 15)  //15秒检查一次所有的运行状态
				{
					Period_Check = 0;//周期标志清零
					Value_Buffer = 0;
					Value_Buffer2 = 0;
					for(Address = 1; Address <= 10; Address ++)
						{
							if(SlaveG[Address].Big_time > Sys_Admin.Balance_Big_Time)
								{
									Value_Buffer2 ++;
								}
							else
								Need_flag = FALSE;


							if(SlaveG[Address].Small_time > Sys_Admin.Balance_Small_Time) //这个时间要大于从机最慢升速的时间
								Value_Buffer++;

							if(SlaveG[Address].Zero_time > 500) //超压停炉超过8分钟，考虑自动排污的时间
								Value_Buffer++;
							
						}

					if(Value_Buffer2 == Already_WorkNumbers) //在运行的全满负荷了
						{
							if(Already_WorkNumbers < AliveOk_Numbres) //仍有待机的机器
								{
									
									Need_flag = OK;
									//清除计时标志，防止二次需求
									for(Address = 1; Address <= 10; Address ++)
										SlaveG[Address].Big_time = 0;
										
								}
						}
					
					if(Value_Buffer >= 2)//有两台以上存在小功率的状态，就减少
						{
							if(Already_WorkNumbers > UnionD.Need_Numbers)//已经运行的机器大于正常需要的数量，就减少
								{
									Loss_flag = OK;
									//清除计时标志，防止二次需求
									for(Address = 1; Address <= 10; Address ++)
										{
											SlaveG[Address].Small_time = 0;
											SlaveG[Address].Zero_time = 0;
										}
								}
								
						}
						
					else
						Loss_flag = FALSE;
						
						
				}


			if(Already_WorkNumbers < UnionD.Need_Numbers)   //如果在运行的台数小于正常需求的台数
				{
					Need_flag = OK;
				}

			if(AliveOk_Numbres == 0)
				{
					UnionD.UnionStartFlag = 0;  //没有一台正常的机器，则关闭
				}
			else
				{
					if(Already_WorkNumbers == AliveOk_Numbres)
					{
						Loop_Flag = FALSE;  //都工作了，就没办法轮休
					}
				}

		
  	}

  	 
	
  	

  //检查正常需求的台数，和现在的运行功率
	

	

	//对在线的机器运行时间，从大到小进行排序
	
		
		if(Already_WorkNumbers <  AliveOk_Numbres)//工作的设备数，比在线正常的少
			{
				if(Loop_Flag)
					{
						Loop_Flag = 0;
						Need_flag = OK;  //增加一台
						Loss_flag =OK;  //减少一台
					}
			}
	
		if(Need_flag)
			{
				//增加待机中，还没参与运行的时间最短机器，开启
				Need_flag = FALSE;

				SlaveG[Min_Address].Startclose_Sendflag = 3; //连续发三次
				SlaveG[Min_Address].Startclose_Data = OK; //启动该机器

			}
		

		if(Loss_flag)
			{
				//减少，找谁运行的时间最长，关闭
				Loss_flag = FALSE;
				SlaveG[Max_Address].Startclose_Sendflag = 3; //连续发三次
				SlaveG[Max_Address].Startclose_Data = 0;//关闭该机器
			}

	

		return 0;
}



