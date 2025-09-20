#include "usart2.h"
#include "stdarg.h"	
 



 UtoSlave_Info UtoSlave;
 Duble5_Info Double5;

 UNION_FLAGS Union_Flag; 	 

LCD10Struct LCD10D;
LCD10_JZ_Struct LCD10JZ[13];  //用于机组的数据显示

LCD_WR  LCD10WR;





void U2_send_byte(u8 data)
{
	
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		USART_SendData(USART2,data);
	 
}

//串口4发送s个字符
void U2_send_str(u8 *str,u8 s)
{
	u8 i;
	for(i=0;i<s;i++)
	{
		U2_send_byte(*str);
		str++;
	}
}


void u2_printf(char* fmt,...)  
{  
  	int len=0;
	int cnt=0;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)U2_Inf.TX_Data,fmt,ap);
	va_end(ap);
	len = strlen((const char*)U2_Inf.TX_Data);
	while(len--)
	  {
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=1); //等待发送结束
	  USART_SendData(USART2,U2_Inf.TX_Data[cnt++]);
	  }
}




///////////////////////////////////////USART2 初始化配置部分//////////////////////////////////	    
//功能：初始化IO 串口2
//输入参数
//bound:波特率
//输出参数
//无
//////////////////////////////////////////////////////////////////////////////////////////////	  
void uart2_init(u32 bound)
{  	 		 
	//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	USART_DeInit(USART2);  //复位串口2

     //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	        //根据指定的参数初始化VIC寄存器
  
    //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;   //一般设置为115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;  //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //收发模式

    USART_Init(USART2, &USART_InitStructure);   //初始化串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //开启中断
    USART_Cmd(USART2, ENABLE);                      //使能串口 
	
//	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);    //使能串口2的DMA发送
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF,1000);  //DMA1通道7,外设为串口2,存储器为USART2_TX_BUF,长度1000. 										  	
}




void ModBus2LCD_Communication(void)
{
		
		uint8  i = 0;	
		uint8 index = 0;
		uint8 Bytes = 0;
		uint8 JiZu_Address = 0;
		
		uint16 checksum = 0;
		uint8  Cmd_Data = 0;
		uint16 Data_Address = 0;
		uint16 Buffer_Data16 = 0;
		uint16 Data_Length = 0;
		float  Buffer_float = 0;
		uint16 Buffer_Int1 = 0;
	
		 if(sys_flag.Lcd_Unconnect_Time >= 15)//失联15秒
 			{
 				
		 		if(Sys_Admin.Device_Style == 0 || Sys_Admin.Device_Style == 1)
		 			{
		 				if(sys_data.Data_10H == 2)
							{
								sys_close_cmd();
								
							}
						
						if(sys_data.Data_10H == 3)
							{
								 sys_data.Data_10H = 0;
								 GetOut_Mannual_Function();
								
							}
		 			}
				
				if(Sys_Admin.Device_Style == 2 || Sys_Admin.Device_Style == 3)  //常规双拼或相变双拼
					{
						//失联情况下，全关闭，手动的，也退出手动，全关闭
						 	if(UnionD.UnionStartFlag == 1)
						 		{
						 			UnionD.UnionStartFlag = 0;
									if(sys_data.Data_10H == 2)
										sys_close_cmd();

									SlaveG[2].Startclose_Sendflag = 3;
									SlaveG[2].Startclose_Data = 0;
						 		}
							if(UnionD.UnionStartFlag == 3)
								{
									UnionD.UnionStartFlag = 0;
									if(sys_data.Data_10H == 3)
										{
											sys_data.Data_10H = 0 ;
											GetOut_Mannual_Function();
										}
									SlaveG[2].Startclose_Sendflag = 3;
									SlaveG[2].Startclose_Data = 0;
								}
								
					}
				

				sys_flag.Alarm_Out = OK;
				Beep_Data.beep_start_flag = 1;
		 		//报警输出，强制b报警
 			}



		
		if(U2_Inf.Recive_Ok_Flag)
			{
				U2_Inf.Recive_Ok_Flag = 0;//不能少哦
				 //关闭中断
				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); 
				 
				checksum  = U2_Inf.RX_Data[U2_Inf.RX_Length - 2] * 256 + U2_Inf.RX_Data[U2_Inf.RX_Length - 1];
				
			
				if(checksum == ModBusCRC16(U2_Inf.RX_Data,U2_Inf.RX_Length))
					{	
						Cmd_Data = U2_Inf.RX_Data[1];

						sys_flag.Lcd_Unconnect_Time  = 0;
						
						if(Cmd_Data == 0x03)
							{
								
								Data_Address = U2_Inf.RX_Data[2] * 256 + U2_Inf.RX_Data[3];
								Buffer_Data16 = U2_Inf.RX_Data[4] * 256 + U2_Inf.RX_Data[5];
								Data_Length = Buffer_Data16;

								
								switch (Data_Address)
									{
									 case 0x0000:

												
									 			Bytes = sizeof(LCD10D.Datas);
												U2_Inf.TX_Data[0] = 0x01;
												U2_Inf.TX_Data[1]= 0x03;
									 			U2_Inf.TX_Data[2] = Bytes; //根据数据长度改编
									 			for(index = 3; index < (Bytes + 3); index ++)
													U2_Inf.TX_Data[index] = LCD10D.Datas[index -3];
											
									 			checksum  = ModBusCRC16(U2_Inf.TX_Data,Bytes + 5);
												U2_Inf.TX_Data[Bytes + 3] = checksum >> 8;
												U2_Inf.TX_Data[Bytes + 4] = checksum & 0x00FF;
												
									 			Usart_SendStr_length(USART2,U2_Inf.TX_Data,Bytes +5);

											
									 			break;
									case 200: //机组2的数据
												JiZu_Address = 2; 
									 			Bytes = sizeof(LCD10JZ[JiZu_Address].Datas);
												U2_Inf.TX_Data[0] = 0x01;
												U2_Inf.TX_Data[1]= 0x03;
									 			U2_Inf.TX_Data[2] = Bytes; //根据数据长度改编
									 			for(index = 3; index < (Bytes + 3); index ++)
													U2_Inf.TX_Data[index] = LCD10JZ[JiZu_Address].Datas[index -3];
											
									 			checksum  = ModBusCRC16(U2_Inf.TX_Data,Bytes + 5);
												U2_Inf.TX_Data[Bytes + 3] = checksum >> 8;
												U2_Inf.TX_Data[Bytes + 4] = checksum & 0x00FF;
												
									 			Usart_SendStr_length(USART2,U2_Inf.TX_Data,Bytes +5);

									 			break;

									
								
									default:
										

											break;
									}
							}



							if(Cmd_Data == 0x10)
							{
								Data_Address = U2_Inf.RX_Data[2] * 256 + U2_Inf.RX_Data[3];
								Data_Length = U2_Inf.RX_Data[4] * 256 + U2_Inf.RX_Data[5];
								Buffer_Data16 = U2_Inf.RX_Data[7]  + U2_Inf.RX_Data[8] * 256;  //高低字节的顺序颠倒
						
								switch (Data_Address)
									{
									 case 0x0000:  //联控启动开关指令

												ModuBus2LCD_WriteAdress0x0000Response(Buffer_Data16);
									 			
									 			
									 			  ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 		
									 			break;
									 case 0x0001:  //故障复位指令
									 			if(Buffer_Data16 == 0)
									 				{
									 					 LCD10D.DLCD.Error_Code = Buffer_Data16;
									 					sys_flag.Error_Code = Buffer_Data16;
									 				}

									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 		
									 			break;
									case 0x0002:  //风机功率调节
									 			if(Buffer_Data16 <=  100)
									 				{
									 					 LCD10D.DLCD.Air_Power = Buffer_Data16;
									 					 PWM_Adjust(Buffer_Data16);
									 				}

									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 		
									 			break;
									case 0x0004:  //水泵的状态
									 			if(Buffer_Data16 <=  1)
									 				{
									 					
									 					if(Buffer_Data16 == 1) //开启
									 						{
									 							LCD10D.DLCD.Pump_State = 1;
																sys_flag.WaterOut_Time = 3;
									 							Feed_Main_Pump_ON();
																sys_flag.Water_Percent = 50;
																Second_Water_Valve_Open();
									 						}

														if(Buffer_Data16 == 0) //关闭
									 						{
									 							LCD10D.DLCD.Pump_State = 0;
									 							Feed_Main_Pump_OFF();
																sys_flag.WaterClose_Time = 2;
																Second_Water_Valve_Close();
																sys_flag.Water_Percent = 0;
									 						}
									 					 
									 				}

									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 		
									 			break;
									case 0x0006:  //排污阀开启或关闭操作
									 			if(Buffer_Data16 <=  1)
									 				{
									 					 
									 					if(Buffer_Data16 == 1) //开启
									 						{
									 							LCD10D.DLCD.Paiwu_State = 1;
									 							Pai_Wu_Door_Open();
									 						}

														if(Buffer_Data16 == 0) //关闭
									 						{
									 							LCD10D.DLCD.Paiwu_State = 0;
									 							Pai_Wu_Door_Close();
									 						}
									 					 
									 				}

									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 		
									 			break;
									case 0x0007:  //风机的状态
									 			if(Buffer_Data16 <=  1)
									 				{
									 					 LCD10D.DLCD.Air_State = Buffer_Data16;
									 					if(Buffer_Data16 == 1) //开启
									 						{
									 							LCD10D.DLCD.Air_State = 1;
									 							Send_Air_Open();
																PWM_Adjust(100);
									 						}

														if(Buffer_Data16 == 0) //关闭
									 						{
									 							LCD10D.DLCD.Air_State = 0;
									 							Send_Air_Close();
																PWM_Adjust(0);
									 						}
									 				}

									 				ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 		
									 			break;
									
									case 0x0008:  //连续排污阀的手动测试
													if(Buffer_Data16 <=  1)
														{
															 LCD10D.DLCD.lianxuFa_State = Buffer_Data16;
															if(Buffer_Data16 == 1) //开启
																{
																	LCD10D.DLCD.lianxuFa_State = 1;
																	LianXu_Paiwu_Open();
																	
																}
	
															if(Buffer_Data16 == 0) //关闭
																{
																	LCD10D.DLCD.lianxuFa_State = 0;
																	LianXu_Paiwu_Close();
																	
																}
														}
	
														ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
													break;

												
									case 0x0016:  //年
												 LCD10D.DLCD.Year = Buffer_Data16;
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 		
									 			break;
									case 0x0017:  //月
												LCD10D.DLCD.Month = Buffer_Data16;
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0018:  //日
												LCD10D.DLCD.Day = Buffer_Data16;
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0019:  //时
												LCD10D.DLCD.Hour = Buffer_Data16;
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x001A:  //分
												LCD10D.DLCD.Min = Buffer_Data16;
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x001B:  //前吹扫时间 
												if(Buffer_Data16 >= 20 && Buffer_Data16 <= 120)
													{
														LCD10D.DLCD.First_Blow_Time = Buffer_Data16 ;
														Sys_Admin.First_Blow_Time = Buffer_Data16 * 1000;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x001C:  //后吹扫时间 
												if(Buffer_Data16 >= 20 && Buffer_Data16 <= 120)
													{
														LCD10D.DLCD.Last_Blow_Time = Buffer_Data16 ;
														Sys_Admin.Last_Blow_Time = Buffer_Data16 * 1000;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x001D:  //点火功率 
												if(Buffer_Data16 >= 20 && Buffer_Data16 <= 60)
													{
														LCD10D.DLCD.Dian_Huo_Power = Buffer_Data16 ;
														Sys_Admin.Dian_Huo_Power = Buffer_Data16 ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x001E:  //最大运行功率 
												if(Buffer_Data16 >= 30 && Buffer_Data16 <= 100)
													{
														LCD10D.DLCD.Max_Work_Power = Buffer_Data16 ;
														Sys_Admin.Max_Work_Power = Buffer_Data16 ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x001F:  //排烟温度报警值 
												if(Buffer_Data16 >= 80 && Buffer_Data16 <= 220)
													{
														LCD10D.DLCD.Danger_Smoke_Value = Buffer_Data16 ;
														Sys_Admin.Danger_Smoke_Value = Buffer_Data16 * 10 ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0020:  //风速检测标志
												if(Buffer_Data16 <= 1)
													{
														LCD10D.DLCD.Fan_Speed_Check = Buffer_Data16 ;
														Sys_Admin.Fan_Speed_Check = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0021:  //点火转速
												if(Buffer_Data16 >= 500 && Buffer_Data16 <= 3000)
													{
														LCD10D.DLCD.Fan_Fire_Value = Buffer_Data16 ;
														Sys_Admin.Fan_Fire_Value = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0022:  //排污标志命令
												if(Buffer_Data16 <= 1)
													{
														LCD10D.DLCD.Paiwu_Flag = Buffer_Data16 ;
														sys_flag.Paiwu_Flag = OK;

														SlaveG[2].Paiwu_Data = OK;
														SlaveG[2].Paiwu_Flag = 3;

														SlaveG[3].Paiwu_Data = OK;
														SlaveG[3].Paiwu_Flag = 3;
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0023:  //变频补水使能
												if(Buffer_Data16 <= 1)
													{
														LCD10D.DLCD.Water_BianPin_Enabled = Buffer_Data16 ;
														Sys_Admin.Water_BianPin_Enabled = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0024:  //进水阀最大开度
												if(Buffer_Data16 >= 25 &&Buffer_Data16 <= 100)
													{
														LCD10D.DLCD.Water_Max_Percent = Buffer_Data16 ;
														Sys_Admin.Water_Max_Percent = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									
									case 0x0025:  //本体温度保护使能
												if(Buffer_Data16 <= 1 )
													{
														LCD10D.DLCD.YuRe_Enabled = Buffer_Data16 ;
														Sys_Admin.YuRe_Enabled = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
												ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x0026:  //本体温度保护值
												if(Buffer_Data16 >= 200 && Buffer_Data16 <= 350)
													{
														LCD10D.DLCD.Inside_WenDu_ProtectValue = Buffer_Data16 ;
														Sys_Admin.Inside_WenDu_ProtectValue = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0027:  //连续排污周期时间设定  分钟
												if(Buffer_Data16 >= 1 && Buffer_Data16 <= 60)
													{
														LCD10D.DLCD.LianXu_PaiWu_DelayTime = Buffer_Data16 ;
														Sys_Admin.LianXu_PaiWu_DelayTime = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0028:  //连续排污每次开启时间设定       秒
												if(Buffer_Data16 >= 1 && Buffer_Data16 <= 60)
													{
														LCD10D.DLCD.LianXu_PaiWu_OpenSecs = Buffer_Data16 ;
														Sys_Admin.LianXu_PaiWu_OpenSecs = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0029:  //通信地址设定
												if(Buffer_Data16 >= 0 && Buffer_Data16 <= 250)
													{
														LCD10D.DLCD.ModBus_Address = Buffer_Data16 ;
														Sys_Admin.ModBus_Address = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
									 			break;
									case 0x002A:  //风机转速脉冲数设定
												if(Buffer_Data16 >= 1 && Buffer_Data16 <= 10)
													{
														LCD10D.DLCD.Fan_Pulse_Rpm = Buffer_Data16 ;
														Sys_Admin.Fan_Pulse_Rpm = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
									 			break;
									case 44:  //差压液位使能信号
												if( Buffer_Data16 <= 1)
													{
														LCD10D.DLCD.ChaYa_Water_Enabled = Buffer_Data16 ;
														Sys_Admin.ChaYa_WaterHigh_Enabled = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
									 			break;
									case 45:  //差压液位低校准
												if( Buffer_Data16 <= 1000)
													{
														LCD10D.DLCD.ChaYa_WaterLow_Set = Buffer_Data16 ;
														Sys_Admin.ChaYa_WaterLow_Set = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
									 			break;
									case 46:  //差压液位中校准
												if( Buffer_Data16 <= 1000)
													{
														LCD10D.DLCD.ChaYa_WaterMid_Set = Buffer_Data16 ;
														Sys_Admin.ChaYa_WaterMid_Set = Buffer_Data16  ;
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
									 			break;
									case 47:  //差压液位高校准
												if( Buffer_Data16 <= 1000)
													{
														LCD10D.DLCD.ChaYa_WaterHigh_Set = Buffer_Data16 ;
														Sys_Admin.ChaYa_WaterHigh_Set = Buffer_Data16  ;

														//u1_printf("\n* 通信*****地址测试 = %d\n",Sys_Admin.ChaYa_WaterHigh_Set);
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
									 			break;
									case 0x0032:  //系统锁机标志
												if( Buffer_Data16 <= 1)
													{
														LCD10D.DLCD.System_Lock = Buffer_Data16 ;
														sys_config_data.Sys_Lock_Set = Buffer_Data16  ;
														Write_Internal_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												
									 			break;
									
									case 0x0033:  // 多拼情况,设备负荷平衡时间
												if(Buffer_Data16 > 20)  // 
													{
														LCD10D.DLCD.Balance_Big_Time = Buffer_Data16;
														Sys_Admin.Balance_Big_Time =  Buffer_Data16;
														
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0034:  // 多拼情况,设备负荷平衡时间
												if(Buffer_Data16 > 20)  // 
													{
														LCD10D.DLCD.Balance_Small_Time = Buffer_Data16;
														Sys_Admin.Balance_Small_Time =  Buffer_Data16;
														
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0036:  //主机设备的使用时间
												if(Buffer_Data16 <= 1)  //多拼情况，设备使用使能
													{
														LCD10D.DLCD.Work_Time = Buffer_Data16;
														
														Write_Admin_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;
									case 0x0037:  //
												if(Buffer_Data16 <= 1)  //多拼情况，设备使用使能
													{
														LCD10JZ[1].DLCD.YunXu_Flag = Buffer_Data16;
														 LCD10D.DLCD.YunXu_Flag = LCD10JZ[1].DLCD.YunXu_Flag;
														 SlaveG[1].Key_Power = Buffer_Data16;
														 Write_Second_Flash();
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;

									case 0x0038:  //设备类型设定
												if(Buffer_Data16 <= 3)  //四种设备类型
													{
														LCD10D.DLCD.Device_Style = Buffer_Data16 ;
														Sys_Admin.Device_Style = Buffer_Data16;
														Write_Admin_Flash();

														Sys_Admin.Device_Style = *(uint32 *)(Device_Style_Choice_ADDRESS);
													}
												
									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
									 			break;

												
									

									case 0x003A:  //目标压力 浮点数
												if(Data_Length == 0x02)
													{
														Float_Int.byte4.data_LL = U2_Inf.RX_Data[7];
														Float_Int.byte4.data_LH = U2_Inf.RX_Data[8];
														Float_Int.byte4.data_HL = U2_Inf.RX_Data[9];
														Float_Int.byte4.data_HH = U2_Inf.RX_Data[10];
														LCD10D.DLCD.Target_Value = Float_Int.value;
														
														Buffer_Int1 = LCD10D.DLCD.Target_Value * 100;
														
														if(Buffer_Int1 >= 10 && Buffer_Int1 <=  Sys_Admin.DeviceMaxPressureSet)
															{
																 sys_config_data.zhuan_huan_temperture_value = Buffer_Int1;
																 LCD10D.DLCD.Target_Value = Float_Int.value;
																 if(sys_config_data.zhuan_huan_temperture_value > sys_config_data.Auto_stop_pressure)
																 	{
																 		sys_config_data.Auto_stop_pressure = sys_config_data.zhuan_huan_temperture_value + 10;
																		if(sys_config_data.Auto_stop_pressure >= Sys_Admin.DeviceMaxPressureSet )
																			sys_config_data.Auto_stop_pressure = Sys_Admin.DeviceMaxPressureSet - 1;

																		LCD10D.DLCD.Stop_Value = (float) sys_config_data.Auto_stop_pressure / 100;
																 	}
																 else
																 	{
																 		
																 	}
																 
																  Write_Internal_Flash();
																

																 sys_config_data.zhuan_huan_temperture_value = *(uint32 *)(ZHUAN_HUAN_TEMPERATURE);
																 
																
															}
														
														
														ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);	
													}



												break;
									case 0x003C:  //停止压力 浮点数
												if(Data_Length == 0x02)
													{
														Float_Int.byte4.data_LL = U2_Inf.RX_Data[7];
														Float_Int.byte4.data_LH = U2_Inf.RX_Data[8];
														Float_Int.byte4.data_HL = U2_Inf.RX_Data[9];
														Float_Int.byte4.data_HH = U2_Inf.RX_Data[10];
														LCD10D.DLCD.Stop_Value = Float_Int.value;
														Buffer_Int1 = LCD10D.DLCD.Stop_Value * 100;

														

														if(Buffer_Int1 >= sys_config_data.zhuan_huan_temperture_value && Buffer_Int1 <=  Sys_Admin.DeviceMaxPressureSet)
															{
																sys_config_data.Auto_stop_pressure = Buffer_Int1;
																LCD10D.DLCD.Stop_Value = Float_Int.value;
																Write_Internal_Flash();
																 sys_config_data.Auto_stop_pressure = *(uint32 *)(AUTO_STOP_PRESSURE_ADDRESS);
															}
														
														ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);	
													}

												break;
									case 0x003E:  //启动压力 浮点数
												if(Data_Length == 0x02)
													{
														Float_Int.byte4.data_LL = U2_Inf.RX_Data[7];
														Float_Int.byte4.data_LH = U2_Inf.RX_Data[8];
														Float_Int.byte4.data_HL = U2_Inf.RX_Data[9];
														Float_Int.byte4.data_HH = U2_Inf.RX_Data[10];
														LCD10D.DLCD.Start_Value = Float_Int.value;
														
														Buffer_Int1 = LCD10D.DLCD.Start_Value * 100;
														if( Buffer_Int1 <  sys_config_data.Auto_stop_pressure)
															{
																sys_config_data.Auto_start_pressure = Buffer_Int1;
																LCD10D.DLCD.Start_Value = Float_Int.value;
																Write_Internal_Flash();
															}
														ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);	
													}

												break;

									case 0x0040:  //额定压力参数设置
												if(Data_Length == 0x02)
													{
														Float_Int.byte4.data_LL = U2_Inf.RX_Data[7];
														Float_Int.byte4.data_LH = U2_Inf.RX_Data[8];
														Float_Int.byte4.data_HL = U2_Inf.RX_Data[9];
														Float_Int.byte4.data_HH = U2_Inf.RX_Data[10];
														LCD10D.DLCD.Max_Pressure = Float_Int.value;
														
														Buffer_Int1 = LCD10D.DLCD.Max_Pressure * 100;
														if( Buffer_Int1 <= 250) //最大额定压力小于2.5Mpa
															{
																Sys_Admin.DeviceMaxPressureSet = Buffer_Int1;
																Write_Admin_Flash();
															}
														ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);	
													}

												break;
	/*********************************************************************************************************/
									case 0x00C9:  //2机的风机功率调整
												//JiZu_ReadAndWrite_Function()  ----区分写的命令
												//UART4_Server_Cmd_Analyse();----0X06功能接收
												//Modbus3_UnionRx_DataProcess();---0X10 功能接收，标志清除
												if(Buffer_Data16 <=  1)
									 				{
														LCD10JZ[2].DLCD.Error_Code = Buffer_Data16;
														SlaveG[2].ResetError_Flag = 3;
														SlaveG[2].ResetError_Data = Buffer_Data16;
									 				}
									 				ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00CA:  //2机的风机功率调整
												//JiZu_ReadAndWrite_Function()  ----区分写的命令
												//UART4_Server_Cmd_Analyse();----0X06功能接收
												//Modbus3_UnionRx_DataProcess();---0X10 功能接收，标志清除
												if(Buffer_Data16 <=  100)
									 				{
														LCD10JZ[2].DLCD.Air_Power = Buffer_Data16;
														SlaveG[2].AirPower_Flag = 3;
														SlaveG[2].AirPower_Data = Buffer_Data16;
									 				}
									 				ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									
									case 0x00CC:  //2机的水泵和进水电磁阀  控制
												//JiZu_ReadAndWrite_Function()	----区分写的命令
												//UART4_Server_Cmd_Analyse();----0X06功能接收
												//Modbus3_UnionRx_DataProcess();---0X10 功能接收，标志清除
												if(Buffer_Data16 <=  1)
													{
														LCD10JZ[2].DLCD.Pump_State = Buffer_Data16;
														SlaveG[2].PumpOpen_Flag = 3;
														SlaveG[2].PumpOpen_Data = Buffer_Data16;
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00CE:  //2机的 排污阀控制
												//JiZu_ReadAndWrite_Function()	----区分写的命令
												//UART4_Server_Cmd_Analyse();----0X06功能接收
												//Modbus3_UnionRx_DataProcess();---0X10 功能接收，标志清除
												if(Buffer_Data16 <=  1)
													{
														LCD10JZ[2].DLCD.Paiwu_State = Buffer_Data16;
														SlaveG[2].PaiWuOpen_Flag = 3;
														SlaveG[2].PaiWuOpen_Data = Buffer_Data16;
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;


									case 0x00CF:  //2机的风机手动模式
												if(Buffer_Data16 <=  1)
									 				{ 
									 					if(Buffer_Data16 == 1) //开启
									 						{
									 							LCD10JZ[2].DLCD.Air_State = 1;
																LCD10JZ[2].DLCD.Air_Power = 100;

																SlaveG[2].AirOpen_Flag = 3;
																SlaveG[2].AirOpen_Data = Buffer_Data16;
									 						}

														if(Buffer_Data16 == 0) //关闭
									 						{
									 							LCD10JZ[2].DLCD.Air_State= 0;
																LCD10JZ[2].DLCD.Air_Power = 0;
																SlaveG[2].AirOpen_Flag = 3;
																SlaveG[2].AirOpen_Data = Buffer_Data16;
									 						}
									 					 
									 				}

									 			ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00D0:  //2机的连续排污阀控制
												//JiZu_ReadAndWrite_Function()	----区分写的命令
												//UART4_Server_Cmd_Analyse();----0X06功能接收
												//Modbus3_UnionRx_DataProcess();---0X10 功能接收，标志清除
												if(Buffer_Data16 <=  1)
													{
														LCD10JZ[2].DLCD.lianxuFa_State = Buffer_Data16;
														SlaveG[2].LianxuFa_Flag = 3;
														SlaveG[2].LianxuFa_Data = Buffer_Data16;
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00D4:  //2机的点火功率
												
												if(Buffer_Data16 <=  60)
													{
														LCD10JZ[2].DLCD.Dian_Huo_Power = Buffer_Data16;
														SlaveG[2].Fire_Power = Buffer_Data16;
														Write_Second_Flash();
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00D5:  //2机的最大功率
												
												if(Buffer_Data16 <=  100 && Buffer_Data16 >=30)
													{
														LCD10JZ[2].DLCD.Max_Work_Power = Buffer_Data16;
														SlaveG[2].Max_Power = Buffer_Data16;
														Write_Second_Flash();
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00D6:  //2机的排烟温度报警值
												
												if(Buffer_Data16 <=  200)
													{
														LCD10JZ[2].DLCD.Danger_Smoke_Value = Buffer_Data16;
														SlaveG[2].Smoke_Protect = Buffer_Data16;
														Write_Second_Flash();
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00D8:  //2机的内部温度报警值
												
												if(Buffer_Data16 <=  400)
													{
														LCD10JZ[2].DLCD.Inside_WenDu_ProtectValue = Buffer_Data16;
														SlaveG[2].Inside_WenDu_ProtectValue = Buffer_Data16;
														Write_Second_Flash();
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00D9:  //2机的设备使能控制
												
												if(Buffer_Data16 <=  1)
													{
														LCD10JZ[2].DLCD.YunXu_Flag = Buffer_Data16;	
														SlaveG[2].Key_Power = Buffer_Data16;
														Write_Second_Flash();
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
									case 0x00DA:  //2机的设备使用时间
												
												if(Buffer_Data16 <=  1)
													{
														LCD10JZ[2].DLCD.Work_Time = Buffer_Data16;	
													}
													ModuBus2LCD_Write0x10Response(Data_Address,Data_Length);
												break;
												

								
									default:
										

											break;
									}
							}
						
						
					}
					
				 
				
			//对接收缓冲器清零
				for( i = 0; i < 200;i++ )
					U2_Inf.RX_Data[i] = 0x00;
			
			//重新开启中断
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
				
			}
}

uint8 ModuBus2LCD_Write0x10Response(uint16 address,uint16 Data16)
{
	uint16 check_sum = 0;
	
	U2_Inf.TX_Data[0] = 0x01;
	U2_Inf.TX_Data[1]= 0x10;

	
	U2_Inf.TX_Data[2] = address >> 8;    // 地址高字节
	U2_Inf.TX_Data[3] = address & 0x00FF;  //地址低字节
	

	U2_Inf.TX_Data[4] = Data16 >> 8;  //数据高字节
	U2_Inf.TX_Data[5] = Data16 & 0x00ff;   //数据低字节

	check_sum  = ModBusCRC16(U2_Inf.TX_Data,8);   //这个根据总字节数改变
	U2_Inf.TX_Data[6]  = check_sum >> 8 ;
	U2_Inf.TX_Data[7]  = check_sum & 0x00FF;

	Usart_SendStr_length(USART2,U2_Inf.TX_Data,8);

	return 0;
}

uint8 ModuBus2LCD_WriteAdress0x0000Response(uint16 Buffer_Data16)
{

		switch (Sys_Admin.Device_Style)
			{
				case 0:
				case 1:  //单机模式
						
						if(Buffer_Data16 == 1) 
							{
								if(sys_data.Data_10H == 0)
									sys_start_cmd();
								 
							}
						if(Buffer_Data16 == 0) 
							{
								if(sys_data.Data_10H == 2)
									sys_close_cmd();
			
								if(sys_data.Data_10H == 3)
									{
										sys_data.Data_10H = 0;
										GetOut_Mannual_Function(); 
									}
								 
							}
						if(Buffer_Data16 == 3) 
							{
								if(sys_data.Data_10H == 0)
									{
										sys_data.Data_10H = 3 ;
										GetOut_Mannual_Function();
									}
							}
			
						break;
				case 2: //双拼模式   要考虑 手动模式和 启动联控的命令
				case 3:
						if(Buffer_Data16 == 1) 
							{
								//启动联控
								UnionD.UnionStartFlag = OK;
								
								//if(sys_data.Data_10H == 0)
								//	sys_start_cmd();
								//if(LCD10JZ[2].DLCD.Device_State == 1)
								//	{
								//		SlaveG[2].Startclose_Sendflag = 3;
								//		SlaveG[2].Startclose_Data = Buffer_Data16;
								//	}
								 
							}
						if(Buffer_Data16 == 0) 
							{
								if(sys_data.Data_10H == 2)
									sys_close_cmd();

								
								//关闭联控
								UnionD.UnionStartFlag = FALSE;
								
								SlaveG[2].Startclose_Sendflag = 3;
								SlaveG[2].Startclose_Data = Buffer_Data16;
									
								
								if(sys_data.Data_10H == 3)
									{
										sys_data.Data_10H = 0;
										GetOut_Mannual_Function(); 
									}
							}
					

						if(Buffer_Data16 == 3) 
							{
								UnionD.UnionStartFlag  = 3;
								
								if(sys_data.Data_10H == 0)
									{
										sys_data.Data_10H = 3 ;
										GetOut_Mannual_Function();
									}
								SlaveG[2].Startclose_Sendflag = 3;
								SlaveG[2].Startclose_Data = Buffer_Data16;
							}
			
						break;
				default:
			
						break;
			}


	return 0;
}










