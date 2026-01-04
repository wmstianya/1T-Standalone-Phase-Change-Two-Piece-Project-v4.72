#include "usart3.h"
#include "stdarg.h"

/*============================================================================
 * 联动优化模块 - 全局变量
 *===========================================================================*/
PressurePredictor gPressurePredictor = {0};
AdaptivePeriod gAdaptivePeriod = {PERIOD_LIGHT, 0};
RotationCtrl gRotationCtrl = {0, 1};  /* 默认使能轮换 */

/*============================================================================
 * 联动优化模块 - 函数实现
 *===========================================================================*/

/**
 * @brief  联动优化模块初始化
 */
void Linkage_Init(void)
{
    uint8_t i;
    
    /* 初始化压力预测器 */
    for(i = 0; i < PRESSURE_HISTORY_SIZE; i++) {
        gPressurePredictor.pressureHistory[i] = 0.0f;
    }
    gPressurePredictor.historyIndex = 0;
    gPressurePredictor.pressureRate = 0.0f;
    gPressurePredictor.predictedPressure = 0.0f;
    gPressurePredictor.initialized = 0;
    
    /* 初始化自适应周期 */
    gAdaptivePeriod.currentPeriod = PERIOD_LIGHT;
    gAdaptivePeriod.loadLevel = 0;
    
    /* 初始化轮换控制 */
    gRotationCtrl.lastRotationMinutes = 0;
    gRotationCtrl.rotationEnabled = 1;
}

/**
 * @brief  更新压力预测器 (每秒调用一次)
 * @param  currentPressure: 当前压力值 (MPa)
 */
void Linkage_UpdatePressurePredictor(float currentPressure)
{
    float oldestPressure;
    
    /* 存储当前压力到环形缓冲 */
    gPressurePredictor.pressureHistory[gPressurePredictor.historyIndex] = currentPressure;
    gPressurePredictor.historyIndex = (gPressurePredictor.historyIndex + 1) % PRESSURE_HISTORY_SIZE;
    
    /* 初始化阶段: 填满缓冲区 */
    if(!gPressurePredictor.initialized) {
        if(gPressurePredictor.historyIndex == 0) {
            gPressurePredictor.initialized = 1;
        }
        gPressurePredictor.pressureRate = 0.0f;
        gPressurePredictor.predictedPressure = currentPressure;
        return;
    }
    
    /* 获取最老的压力值 (环形缓冲区当前索引位置) */
    oldestPressure = gPressurePredictor.pressureHistory[gPressurePredictor.historyIndex];
    
    /* 计算压力变化率 (MPa/s) */
    gPressurePredictor.pressureRate = (currentPressure - oldestPressure) / (float)PRESSURE_HISTORY_SIZE;
    
    /* 预测30秒后的压力 */
    gPressurePredictor.predictedPressure = currentPressure + gPressurePredictor.pressureRate * PRESSURE_PREDICT_TIME;
    
    /* 限制预测值范围 */
    if(gPressurePredictor.predictedPressure < 0.0f) {
        gPressurePredictor.predictedPressure = 0.0f;
    }
    if(gPressurePredictor.predictedPressure > 2.0f) {
        gPressurePredictor.predictedPressure = 2.0f;
    }
}

/**
 * @brief  更新自适应评估周期
 * @param  avgPower: 平均功率 (%)
 * @param  pressure: 当前压力 (MPa)
 * @param  setpoint: 设定压力 (MPa)
 */
void Linkage_UpdateAdaptivePeriod(uint8_t avgPower, float pressure, float setpoint)
{
    float pressureRatio;
    
    if(setpoint < 0.1f) {
        setpoint = 0.5f;  /* 防止除零 */
    }
    
    pressureRatio = pressure / setpoint;
    
    /* 根据负荷和压力状态确定评估周期 */
    if(avgPower >= 90 || pressureRatio < 0.85f) {
        gAdaptivePeriod.loadLevel = 3;  /* 临界 */
        gAdaptivePeriod.currentPeriod = PERIOD_CRITICAL;
    }
    else if(avgPower >= 70 || pressureRatio < 0.95f) {
        gAdaptivePeriod.loadLevel = 2;  /* 重载 */
        gAdaptivePeriod.currentPeriod = PERIOD_HEAVY;
    }
    else if(avgPower >= 45) {
        gAdaptivePeriod.loadLevel = 1;  /* 中载 */
        gAdaptivePeriod.currentPeriod = PERIOD_MEDIUM;
    }
    else {
        gAdaptivePeriod.loadLevel = 0;  /* 轻载 */
        gAdaptivePeriod.currentPeriod = PERIOD_LIGHT;
    }
}

/**
 * @brief  判断是否需要增载 (基于压力预测)
 * @param  setpoint: 设定压力 (MPa)
 * @retval 1=需要增载, 0=不需要
 */
uint8_t Linkage_ShouldAddUnit(float setpoint)
{
    /* 条件: 压力快速下降，且预测压力将低于设定值的90% */
    if(gPressurePredictor.initialized &&
       gPressurePredictor.pressureRate < -PRESSURE_RATE_THRESHOLD &&
       gPressurePredictor.predictedPressure < setpoint * 0.9f) {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  检查是否需要轮换
 * @param  currentMinutes: 当前系统运行总分钟数
 * @retval 1=需要轮换, 0=不需要
 */
uint8_t Linkage_CheckRotation(uint32_t currentMinutes)
{
    if(!gRotationCtrl.rotationEnabled) {
        return 0;
    }
    
    /* 检查是否到达轮换周期 */
    if((currentMinutes - gRotationCtrl.lastRotationMinutes) >= ROTATION_PERIOD_MINUTES) {
        gRotationCtrl.lastRotationMinutes = currentMinutes;
        return 1;
    }
    
    return 0;
}

/**
 * @brief  执行机组轮换
 */
void Linkage_DoRotation(void)
{
    uint8_t maxWorkAddr = 0;
    uint8_t minWorkAddr = 0;
    uint32_t maxWorkTime = 0;
    uint32_t minWorkTime = 0xFFFFFFFF;
    uint8_t i;
    uint8_t runningCount = 0;  /* Bug fix P1: 统计运行中机组数 */
    
    /* 找出运行中工作时间最长的机组 */
    /* 找出待机中工作时间最短的机组 */
    for(i = 1; i <= 10; i++) {
        if(SlaveG[i].Alive_Flag && LCD10JZ[i].DLCD.YunXu_Flag && 
           LCD10JZ[i].DLCD.Error_Code == 0) {
            
            if(LCD10JZ[i].DLCD.Device_State == 2) {  /* 运行中 */
                runningCount++;  /* Bug fix P1: 计数运行机组 */
                if(LCD10JZ[i].DLCD.Work_Time > maxWorkTime) {
                    maxWorkTime = LCD10JZ[i].DLCD.Work_Time;
                    maxWorkAddr = i;
                }
            }
            else if(LCD10JZ[i].DLCD.Device_State == 1) {  /* 待机 */
                if(LCD10JZ[i].DLCD.Work_Time < minWorkTime) {
                    minWorkTime = LCD10JZ[i].DLCD.Work_Time;
                    minWorkAddr = i;
                }
            }
        }
    }
    
    /* Bug fix P1: 至少保持2台运行时才执行轮换，防止供热中断 */
    if(runningCount < 2) {
        return;
    }
    
    /* 如果工作时间差异超过阈值，执行轮换 */
    if(maxWorkAddr != 0 && minWorkAddr != 0) {
        if(maxWorkTime > minWorkTime && 
           (maxWorkTime - minWorkTime) > ROTATION_TIME_THRESHOLD) {
            /* 关闭运行时间最长的 */
            SlaveG[maxWorkAddr].Startclose_Sendflag = 3;
            SlaveG[maxWorkAddr].Startclose_Data = 0;
            
            /* 启动运行时间最短的 */
            SlaveG[minWorkAddr].Startclose_Sendflag = 3;
            SlaveG[minWorkAddr].Startclose_Data = 1;
        }
    }
}

/**
 * @brief  计算机组优先级分数 (分数越高，越优先启动)
 * @param  address: 机组地址 (1-10)
 * @retval 优先级分数
 */
float Linkage_CalcPriority(uint8_t address)
{
    float score = 0.0f;
    uint32_t maxWorkTime = 0;
    uint32_t myWorkTime;
    uint8_t i;
    
    /* Bug fix P1: 添加参数边界检查 */
    if(address < 1 || address > 10) {
        return 0.0f;  /* 无效地址返回最低分 */
    }
    
    /* 找出最大运行时间 */
    for(i = 1; i <= 10; i++) {
        if(SlaveG[i].Alive_Flag && LCD10JZ[i].DLCD.YunXu_Flag) {
            if(LCD10JZ[i].DLCD.Work_Time > maxWorkTime) {
                maxWorkTime = LCD10JZ[i].DLCD.Work_Time;
            }
        }
    }
    
    myWorkTime = LCD10JZ[address].DLCD.Work_Time;
    
    /* 运行时间越短，分数越高 (权重0.5) */
    if(maxWorkTime > 0) {
        score += WEIGHT_WORK_TIME * (1.0f - (float)myWorkTime / (float)maxWorkTime);
    } else {
        score += WEIGHT_WORK_TIME;  /* 如果都是0，给满分 */
    }
    
    /* 空闲时间越长，分数越高 (权重0.3) */
    /* 使用 Small_time 作为空闲时间的近似 */
    if(SlaveG[address].Zero_time > 0) {
        /* 每空闲100秒加0.1分，最高0.3分 */
        float idleScore = (float)SlaveG[address].Zero_time / 1000.0f;
        if(idleScore > WEIGHT_IDLE_TIME) {
            idleScore = WEIGHT_IDLE_TIME;
        }
        score += idleScore;
    }
    
    /* 效率权重 (保留扩展，暂用固定值0.1) */
    score += WEIGHT_EFFICIENCY * 0.5f;
    
    return score;
}

/**
 * @brief  选择最优机组启动
 * @retval 最优机组地址 (1-10), 0表示无可用机组
 */
uint8_t Linkage_SelectBestUnit(void)
{
    float maxScore = -1.0f;
    uint8_t bestAddr = 0;
    uint8_t i;
    float score;
    
    for(i = 1; i <= 10; i++) {
        /* 检查机组是否可用且处于待机状态 */
        if(SlaveG[i].Alive_Flag && 
           LCD10JZ[i].DLCD.YunXu_Flag && 
           LCD10JZ[i].DLCD.Error_Code == 0 &&
           LCD10JZ[i].DLCD.Device_State == 1) {
            
            score = Linkage_CalcPriority(i);
            if(score > maxScore) {
                maxScore = score;
                bestAddr = i;
            }
        }
    }
    
    return bestAddr;
}

/*============================================================================*/

///////////////////////////////////////USART3 printf֧�ֲ���//////////////////////////////////
//����2,u2_printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
////////////////////////////////////////////////////////////////////////////////////////////////
void u3_printf(char* fmt,...)  
{  
  
}
///////////////////////////////////////USART2 ��ʼ�����ò���//////////////////////////////////	    
//���ܣ���ʼ��IO ����2
//�������
//bound:������
//�������
//��
//////////////////////////////////////////////////////////////////////////////////////////////	  
void uart3_init(u32 bound)
{  	 		 
	//GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��
	USART_DeInit(USART3);  //��λ����3

     //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

    //Usart3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	        //����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
    //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;   //һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;  //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure);   //��ʼ������
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //�����ж�
    USART_Cmd(USART3, ENABLE);                      //ʹ�ܴ��� 
	
//	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);    //ʹ�ܴ���2��DMA����
//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF,1000);  //DMA1ͨ��7,����Ϊ����2,�洢��ΪUSART2_TX_BUF,����1000. 										  	
}




uint8 FuNiSen_Read_WaterDevice_Function(void)
{
	static uint8  Jump_Index = 0;
	static uint8 Jump_Count = 0;
	uint8 Address = 8; //Ĭ�ϱ����ڵ�ַΪ 8 �����ڽ�ˮ���ڷ�
	uint16 check_sum = 0;
	uint16 Percent = 0;


	//200ms����һ��
	if(sys_flag.WaterAJ_Flag == 0)
		return 0 ;

	sys_flag.WaterAJ_Flag = 0;
	sys_flag.waterSend_Flag = OK;

	if(sys_flag.Water_Percent <= 100)
		Percent =  sys_flag.Water_Percent * 10;
	else
		Percent = 0 ; //ֵ������ر�

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
			case 0: //��ȡʵʱ�Ŀ�����ֵ
					U3_Inf.TX_Data[0] = Address;
					U3_Inf.TX_Data[1] = 0x03;// 
					
					U3_Inf.TX_Data[2] = 0x00;
					U3_Inf.TX_Data[3] = 0x00;//��ַ

					U3_Inf.TX_Data[4] = 0x00;
					U3_Inf.TX_Data[5] = 0x01;//��ȡ���ݸ���
				 
					check_sum  = ModBusCRC16(U3_Inf.TX_Data,8);
					U3_Inf.TX_Data[6]  = check_sum >> 8 ;
					U3_Inf.TX_Data[7]  = check_sum & 0x00FF;
					
					Usart_SendStr_length(USART3,U3_Inf.TX_Data,8);
				   
					//Jump_Index = 1;
					break;
			case 1://д�뿪�ȵ�ֵ
					//Jump_Index = 0;

					U3_Inf.TX_Data[0] = Address;
					U3_Inf.TX_Data[1] = 0x06;// 
					
					U3_Inf.TX_Data[2] = 0x00;
					U3_Inf.TX_Data[3] = 0x01;//��ַ

					U3_Inf.TX_Data[4] = Percent >> 8;
					U3_Inf.TX_Data[5] = Percent & 0x00FF;//д��Ŀ���ֵ
				 
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
			U3_Inf.Recive_Ok_Flag = 0;//������Ŷ
			 //�ر��ж�
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
						   //��ȡ��ǰ����״̬
						  
						   
							Value_Buffer = U3_Inf.RX_Data[3] * 256 + U3_Inf.RX_Data[4];
							if(Value_Buffer == 0xEA)
								sys_flag.Water_Error_Code = OK; //����״̬
							else
							   sys_flag.Water_Error_Code = 0;

							if(sys_flag.Water_Error_Code == OK)
							   sys_flag.Error_Code = Error18_SupplyWater_Error;
							
						}
					
				}
				

		//�Խ��ջ���������
			for( i = 0; i < 100;i++ )
				U3_Inf.RX_Data[i] = 0x00;
		
		//���¿����ж�
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
				//���������ֻ��Ƿ�����
				for(i = 1; i < 13; i++)
					{
						if(SlaveG[i].Send_Flag > 30)//����6��δ��Ӧ����ʧ��
							{
								SlaveG[i].Alive_Flag = FALSE;
								LCD10JZ[i].DLCD.Device_State = 0;   
								memset(&LCD10JZ[i].Datas,0,sizeof(LCD10JZ[i].Datas)); //Ȼ�����������
							}
					}

				if(SlaveG[2].Alive_Flag == 0)
					{
						 Water_State.Cstate_Flag = 0;
						Water_State.CSignal = 0 ;  //��ˮ�� ����״̬
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
				U3_Inf.Recive_Ok_Flag = 0;//������Ŷ
				 //�ر��ж�
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); 

				// #region agent log
				// 记录“短帧/异常帧”现象（用于验证H2/H4：截断/拼包/干扰导致误解析）
				if(U3_Inf.RX_Length < 8)
				{
					U5_Printf("{\"sessionId\":\"debug-session\",\"runId\":\"run1-pre\",\"hypothesisId\":\"H2\",\"location\":\"usart3.c:ModBus_Uart3_LocalRX_Communication\",\"message\":\"u3 rxLen too small\",\"data\":{\"boardAddr\":%d,\"rxLen\":%d},\"timestamp\":%lu}\r\n",
					          sys_flag.Address_Number,
					          U3_Inf.RX_Length,
					          (unsigned long)sys_time_inf.sec * 1000UL);
				}
				// #endregion
				 
				checksum  = U3_Inf.RX_Data[U3_Inf.RX_Length - 2] * 256 + U3_Inf.RX_Data[U3_Inf.RX_Length - 1];
				command = U3_Inf.RX_Data[1];
				Length = U3_Inf.RX_Data[2];
				
			
				if(checksum == ModBusCRC16(U3_Inf.RX_Data,U3_Inf.RX_Length))
					{
						Target_Address = U3_Inf.RX_Data[0];

						// #region agent log
						// 只在“可能导致误报码/误停机”的场景输出关键帧信息（用于验证H1/H2/H4）
						{
							uint8 errByte = 0xFF;
							uint8 flameByte = 0xFF;
							uint16 expLen = 0;
							uint16 crcCalc = 0;

							if(U3_Inf.RX_Length > 18)
							{
								errByte = U3_Inf.RX_Data[16];
								flameByte = U3_Inf.RX_Data[18];
							}
							if(command == 0x03)
							{
								expLen = (uint16)3 + (uint16)Length + (uint16)2;
							}
							if(U3_Inf.RX_Length >= 2)
							{
								crcCalc = ModBusCRC16(U3_Inf.RX_Data, U3_Inf.RX_Length);
							}

							if((command == 0x03 && expLen != 0 && expLen != U3_Inf.RX_Length) || (errByte == 12))
							{
								U5_Printf("{\"sessionId\":\"debug-session\",\"runId\":\"run1-pre\",\"hypothesisId\":\"H2\",\"location\":\"usart3.c:ModBus_Uart3_LocalRX_Communication\",\"message\":\"u3 frame anomaly\",\"data\":{\"boardAddr\":%d,\"rxLen\":%d,\"targetAddr\":%d,\"cmd\":%d,\"byteCount\":%d,\"expLen\":%d,\"errByte\":%d,\"flameByte\":%d,\"crcRx\":%u,\"crcCalc\":%u},\"timestamp\":%lu}\r\n",
								          sys_flag.Address_Number,
								          U3_Inf.RX_Length,
								          Target_Address,
								          command,
								          Length,
								          expLen,
								          errByte,
								          flameByte,
								          checksum,
								          crcCalc,
								          (unsigned long)sys_time_inf.sec * 1000UL);
							}
						}
						// #endregion

						if(sys_flag.Address_Number == 0)
							{
								//��������������Ȩ�����մ�������
								if(Target_Address < 8)
								{
									Modbus3_UnionRx_DataProcess(U3_Inf.RX_Data[1],Target_Address);
								}
							}
						

						if(Target_Address == 8) //��Ƶ��ˮ��
							{
								 sys_flag.waterSend_Flag = FALSE;
								 sys_flag.waterSend_Count = 0;
									if(command == 0x03 && Length == 0x02)
										{
										   //��ȡ��ǰ����״̬
											Value_Buffer = U3_Inf.RX_Data[3] * 256 + U3_Inf.RX_Data[4];
											if(Value_Buffer == 0xEA)
												sys_flag.Water_Error_Code = OK; //����״̬
											else
											   sys_flag.Water_Error_Code = 0;

											if(sys_flag.Water_Error_Code == OK)
											   sys_flag.Error_Code = Error18_SupplyWater_Error;
											
										}
				
							}



						if(Target_Address == 10) //��·4-20mA��չģ��
							{
								 
									if(U3_Inf.RX_Data[1] == 0x04 )
										{
										   //��ȡ��ǰ����״̬
										   
											Value_Buffer = U3_Inf.RX_Data[3] * 256 + U3_Inf.RX_Data[4];

											if(Value_Buffer >= 819)
												{
													//Value_Buffer  = Value_Buffer - 819;  //4mA  = 819  20mA = 4095
													Value20mA = (float)Value_Buffer * 20 /4095 - 4;  //�����ʵ�ʵ�mAֵ
												}
										   
											if(Value_Buffer > 0 )
												{
													sys_flag.ChaYaWater_Value = Value20mA * 1500 / 16;  // 20 = 20mA , 75 = 1500����/ 16mA
													 
												}
											else
												{
													sys_flag.ChaYaWater_Value = 0;
												}
										//	u1_printf("\n* ����ֵ = %d\n",Value_Buffer);//126
										//	u1_printf("\n* ����ֵ4-20mA = %f\n",Value20mA);
										//	u1_printf("\n* Һλ�߶� = %d\n",sys_flag.ChaYaWater_Value);
											
											
										}
				
							}
						
							

					}
					
			 
				
			//�Խ��ջ���������
				for( i = 0; i < 100;i++ )
					U3_Inf.RX_Data[i] = 0x00;
			
			//���¿����ж�
				USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
				
			}
}


uint8 Modbus3_UnionTx_Communication(void)
{
	static uint8 Address = 1;
	uint16 check_sum = 0;
	//	SlaveG[1].Alive_Flag  
	//	JiZu[1].Slave_D.Device_State  //�����ڸ������ݵ�ͬ��


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

	//��ȷ�豸����
	if(Sys_Admin.Device_Style == 2 || Sys_Admin.Device_Style == 3)
		{
			
			switch (Address)
			{
				case 1:
							SlaveG[1].Alive_Flag = OK;
							SlaveG[1].Send_Flag  = 0;
							SlaveG[1].UnBack_time = 0;
							if(sys_flag.Error_Code == 0)
								SlaveG[1].Ok_Flag = OK;   //��Щ�ж���������

							LCD10JZ[1].DLCD.Error_Code = sys_flag.Error_Code;
							
							if(SlaveG[1].Startclose_Sendflag)
								{
									SlaveG[1].Startclose_Sendflag = 0; //�������־
									if(SlaveG[1].Startclose_Data == 0)
										{
											//�������أ�Ҫ�ر��豸
											if(sys_data.Data_10H == 2)
												sys_close_cmd();
										}
									if(SlaveG[1].Startclose_Data == 1)
										{
											//�������أ�Ҫ�����豸
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

							//��ܴӻ�
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
								if(Sys_Admin.Water_BianPin_Enabled)//�����ı�Ƶ����
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
										U3_Inf.TX_Data[0] = 10; // ��ַĬ��Ϊ10
										U3_Inf.TX_Data[1] = 0x04;// 
										
										U3_Inf.TX_Data[2] = 0x00;
										U3_Inf.TX_Data[3] = 0x00;//��ַ

										U3_Inf.TX_Data[4] = 0x00;
										U3_Inf.TX_Data[5] = 0x02;//��ȡ���ݸ���
									 
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
	//���յ��ӻ������ݽ��д���
		
	SlaveG[address].Send_Flag = 0;  //������ͱ�־
	SlaveG[address].Alive_Flag = OK;
	if(Cmd == 0x03)
		{
			Data_Length = U3_Inf.RX_Data[2];

			if(Data_Length == 36)
				{
					// #region agent log
					// 从机上报故障码变化（重点关注=12）（用于验证H1/H2：从机真实上报 vs 主机误解析）
					{
						uint8 oldErr = LCD10JZ[address].DLCD.Error_Code;
						uint8 newErr = U3_Inf.RX_Data[16];
						if(newErr == 12 && oldErr != newErr)
						{
							uint8 flameByte = 0xFF;
							if(U3_Inf.RX_Length > 18)
								flameByte = U3_Inf.RX_Data[18];

							U5_Printf("{\"sessionId\":\"debug-session\",\"runId\":\"run1-pre\",\"hypothesisId\":\"H1\",\"location\":\"usart3.c:Modbus3_UnionRx_DataProcess\",\"message\":\"slave errCode -> 12\",\"data\":{\"boardAddr\":%d,\"slaveAddr\":%d,\"oldErr\":%d,\"newErr\":%d,\"rxLen\":%d,\"flameByte\":%d},\"timestamp\":%lu}\r\n",
							          sys_flag.Address_Number,
							          address,
							          oldErr,
							          newErr,
							          U3_Inf.RX_Length,
							          flameByte,
							          (unsigned long)sys_time_inf.sec * 1000UL);
						}
					}
					// #endregion

					if(SlaveG[address].Startclose_Sendflag == 0) //��������·���ʵ�ʲ�һ�£��ȷ��ʹ������
						{
						//	if(U3_Inf.RX_Data[4]  == 2)   //����������ı�־
						//		JiZu[address].Slave_D.StartFlag = OK;  //  ***********************????????????
						//	else
						//		JiZu[address].Slave_D.StartFlag = 0;  
						}
					

					if(SlaveG[address].Send_Flag > 15)//����6��δ��Ӧ����ʧ��
						{
							LCD10JZ[address].DLCD.Device_State = 0;   //һ����ֲ���ⲿȥ
						}
					else
						{
							if(U3_Inf.RX_Data[4]  == 2)
								LCD10JZ[address].DLCD.Device_State = 2;  //����״̬���л�
							if(U3_Inf.RX_Data[4]  == 0)
								LCD10JZ[address].DLCD.Device_State = 1;
							if(U3_Inf.RX_Data[16])
								LCD10JZ[address].DLCD.Device_State = 3;  //����״̬
						}
					LCD10JZ[address].DLCD.Error_Code = U3_Inf.RX_Data[16];  //��2���������� ���ϴ���
					LCD10JZ[address].DLCD.Air_Power = U3_Inf.RX_Data[24];	//��3���������� �������
					LCD10JZ[address].DLCD.Flame_State = U3_Inf.RX_Data[18];	//��4���������� ����
					LCD10JZ[address].DLCD.Pump_State = U3_Inf.RX_Data[28];	//��5���������� ˮ�õ�״̬
					LCD10JZ[address].DLCD.Water_State = U3_Inf.RX_Data[10];	//��6���������� ˮλ״̬
					LCD10JZ[address].DLCD.Paiwu_State = U3_Inf.RX_Data[30];	//��7���������� ���۷�״̬
					LCD10JZ[address].DLCD.Air_State = U3_Inf.RX_Data[26];	//��8���������� ���״̬
					LCD10JZ[address].DLCD.lianxuFa_State = U3_Inf.RX_Data[32];	//��9���������� �������۷�״̬
					LCD10JZ[address].DLCD.Air_Speed = U3_Inf.RX_Data[19] * 256  + U3_Inf.RX_Data[20];	//��9���������� ���ת��

					
					sys_flag.Cong_ChaYaWater_Value = U3_Inf.RX_Data[35] * 256  + U3_Inf.RX_Data[36];// **********************
					LCD10JZ[address].DLCD.InPut_Data = U3_Inf.RX_Data[37] * 256  + U3_Inf.RX_Data[38];

					LCD10JZ[address].DLCD.Dian_Huo_Power = SlaveG[address].Fire_Power; 
					LCD10JZ[address].DLCD.Max_Work_Power = SlaveG[address].Max_Power; 
					LCD10JZ[address].DLCD.Danger_Smoke_Value = SlaveG[address].Smoke_Protect; 
					LCD10JZ[address].DLCD.Inside_WenDu_ProtectValue = SlaveG[address].Inside_WenDu_ProtectValue; 

					LCD10JZ[address].DLCD.YunXu_Flag = SlaveG[address].Key_Power; 
					if(address == 2)
						{
							//ֻ�����˫ƴ���͵Ĳ�ˮ������
							Water_State.Cstate_Flag = LCD10JZ[address].DLCD.Pump_State;
							Water_State.CSignal = U3_Inf.RX_Data[34];  //��ˮ�� ����״̬
						}
					
					

					Buffer_Float = U3_Inf.RX_Data[5] * 256  + U3_Inf.RX_Data[6];
					if(Buffer_Float > 0)  //��ѹ��ת��Ϊ������
						LCD10JZ[address].DLCD.Steam_Pressure  = Buffer_Float / 100;   //����ѹ��
					else
						LCD10JZ[address].DLCD.Steam_Pressure = 0;

					Buffer_Float = U3_Inf.RX_Data[21] * 256  + U3_Inf.RX_Data[22];
					if(Buffer_Float > 0)  //��ѹ��ת��Ϊ������
						LCD10JZ[address].DLCD.Inside_High_Pressure  = Buffer_Float / 100;   //�ڲ���ѹ��ѹ��
					else
						LCD10JZ[address].DLCD.Inside_High_Pressure = 0;
						
					Buffer_Float = U3_Inf.RX_Data[13] * 256  + U3_Inf.RX_Data[14];
					LCD10JZ[address].DLCD.LuNei_WenDu = Buffer_Float;              //�ڲ��¶�

					Buffer_Float = U3_Inf.RX_Data[11] * 256  +U3_Inf.RX_Data[12] ;
					LCD10JZ[address].DLCD.Smoke_WenDu = Buffer_Float;              //�����¶�
					
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
								//����������ָ�����
								LCD10JZ[address].DLCD.Device_State = 2; //˵���û����Ѿ��յ�ָ��������ݻ�û����
							}

						if(Value_Buffer == 0)
							{
								//�����ػ���ָ�����
								LCD10JZ[address].DLCD.Device_State = 1; //˵���û����Ѿ��յ�ָ��������ݻ�û����
							}

						if(Value_Buffer == 3)
							{
								//�����ֶ���ָ�����
								LCD10JZ[address].DLCD.Device_State = 3; //˵���û����Ѿ��յ�ָ��������ݻ�û����
							}

						
					}

				if(Data_Address == 0x006B)
					{
						SlaveG[address].ResetError_Flag = 0;
					}

				if(Data_Address == 111)  //������ʵ����ֶ�����   111���ݴ���4�ĵ�ַ˳��
					{
						SlaveG[address].AirPower_Flag = 0;
					}

				if(Data_Address == 112)  //����ֶ�����   112���ݴ���4�ĵ�ַ˳��
					{
						SlaveG[address].AirOpen_Flag = 0;
					}
				if(Data_Address == 113)  //ˮ���ֶ�����   113���ݴ���4�ĵ�ַ˳��
					{
						SlaveG[address].PumpOpen_Flag = 0;
					}
				if(Data_Address == 114)  //���۷��ֶ�����   114���ݴ���4�ĵ�ַ˳��
					{
						SlaveG[address].PaiWuOpen_Flag = 0;
					}
				if(Data_Address == 115)  //LianXu���۲���   115���ݴ���4�ĵ�ַ˳��
					{
						SlaveG[address].LianxuFa_Flag = 0;
					}
				if(Data_Address == 117)  //�Զ�������һ��   117���ݴ���4�ĵ�ַ˳��
					{
						SlaveG[address].Paiwu_Flag = 0;
					}
		}
		


		return 0;
}


uint8 JiZu_ReadAndWrite_Function(uint8 address)
{
		
		static uint8 index  = 0;
		uint8 Tx_Index = 0;  //���ͷ�ʽ��ѡ��
		uint8 return_value = 0;
		 


	//	SlaveG[1].Alive_Flag  
	//	JiZu[1].Slave_D.Device_State  //�����ڸ������ݵ�ͬ��
		switch (index)
			{
			case 0:
					Tx_Index = 0; //Ĭ���ǵ���0 

					if(SlaveG[address].Startclose_Sendflag)
						{
							SlaveG[address].Startclose_Sendflag--; //�Է��ʹ����ݼ�
							Tx_Index = 1;
						}
					if(SlaveG[address].AirOpen_Flag && Tx_Index == 0)  //ÿ��ֻ����һ��ͨ�ſ���
						{
							SlaveG[address].AirOpen_Flag--; //�Է��ʹ����ݼ�
							Tx_Index = 2;
						}

					if(SlaveG[address].AirPower_Flag && Tx_Index == 0)  //ÿ��ֻ����һ��ͨ�ſ���
						{
							SlaveG[address].AirPower_Flag--; //�Է��ʹ����ݼ�
							Tx_Index = 3;
						}
					if(SlaveG[address].PumpOpen_Flag && Tx_Index == 0)  //ÿ��ֻ����һ��ͨ�ſ���
						{
							SlaveG[address].PumpOpen_Flag--; //�Է��ʹ����ݼ�
							Tx_Index = 4;
						}
					if(SlaveG[address].PaiWuOpen_Flag && Tx_Index == 0)  //ÿ��ֻ����һ��ͨ�ſ���
						{
							SlaveG[address].PaiWuOpen_Flag--; //�Է��ʹ����ݼ�
							Tx_Index = 5;
						}
					if(SlaveG[address].LianxuFa_Flag && Tx_Index == 0)  //ÿ��ֻ����һ��ͨ�ſ���
						{
							SlaveG[address].LianxuFa_Flag--; //�Է��ʹ����ݼ�
							Tx_Index = 6;
						}

					if(SlaveG[address].ResetError_Flag && Tx_Index == 0)  //ÿ��ֻ����һ��ͨ�ſ���
						{
							SlaveG[address].ResetError_Flag--; //�Է��ʹ����ݼ�
							Tx_Index = 7;
						}

					if(SlaveG[address].Paiwu_Flag && Tx_Index == 0)  //ÿ��ֻ����һ��ͨ�ſ���
						{
							SlaveG[address].Paiwu_Flag--; //�Է��ʹ����ݼ�
							Tx_Index = 9;
						}

					
					if(Tx_Index == 0)
						{
							if(SlaveG[address].Send_Count > 10)
								{
									SlaveG[address].Send_Count = 0;
									Tx_Index = 8;                 //дһ�����ݽ�ȥ
								}
						}
					

					switch (Tx_Index)
						{
							case 0 :
									SlaveG[address].Send_Count++;
									ModBus3_RTU_Read03(address,100,18); //�����Ĭ�϶�ȡ���ݵ�ָ��
									break;
							case 1:	  //������رջ���,�����ֶ�ģʽ��ָ��
									ModBus3_RTU_Write06(address,100,SlaveG[address].Startclose_Data);
									break;
							case 2:  //�ֶ����������رշ��
									ModBus3_RTU_Write06(address,112,SlaveG[address].AirOpen_Data);	
									break;
							case 3: //������ʵĵ���
									ModBus3_RTU_Write06(address,111,SlaveG[address].AirPower_Data);
									break;
							case 4:  //�ֶ�����ˮ�û�ر�
									ModBus3_RTU_Write06(address,113,SlaveG[address].PumpOpen_Data);	
									break;
							case 5:  //�ֶ��������۷���ر�
									ModBus3_RTU_Write06(address,114,SlaveG[address].PaiWuOpen_Data);	
									break;
							case 6:  //�ֶ�����LIANXU���۷���ر�
									ModBus3_RTU_Write06(address,115,SlaveG[address].LianxuFa_Data);	
									break;

							case 7:  //���ϸ�λָ��
									ModBus3_RTU_Write06(address,107,SlaveG[address].ResetError_Data);	
									break;
							case 8:

									ModBus3_RTU_Write10(address,150);  //д��18������
									break;

							case 9:  //��������ӻ���ʼ�Զ�������һ��
									ModBus3_RTU_Write06(address,117,SlaveG[address].Paiwu_Data);	
									break;
							

							default:
									Tx_Index = 0;

									break;
						}

								
					SlaveG[address].Send_Flag ++; //�Է��ͽ��м���
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
		U3_Inf.TX_Data[5]= 18 ;  //���ݸ������޸�

		U3_Inf.TX_Data[6]= 36 ; 
		
		U3_Inf.TX_Data[7] = sys_config_data.zhuan_huan_temperture_value >> 8;
		U3_Inf.TX_Data[8] = sys_config_data.zhuan_huan_temperture_value & 0x00FF;// 1Ŀ������ѹ��

		U3_Inf.TX_Data[9] = sys_config_data.Auto_stop_pressure >> 8;
		U3_Inf.TX_Data[10] = sys_config_data.Auto_stop_pressure & 0x00FF;  //2 ֹͣѹ��

		U3_Inf.TX_Data[11] = sys_config_data.Auto_start_pressure >> 8;
		U3_Inf.TX_Data[12] = sys_config_data.Auto_start_pressure & 0x00FF; //3 ����ѹ��

		U3_Inf.TX_Data[13] = 0x00;
		U3_Inf.TX_Data[14] = SlaveG[Target_Address].Fire_Power;//4 ����� ������һ��

		U3_Inf.TX_Data[15] = 0x00;
		U3_Inf.TX_Data[16] = SlaveG[Target_Address].Max_Power;//5 ������й��� ������һ��

		U3_Inf.TX_Data[17] = SlaveG[Target_Address].Inside_WenDu_ProtectValue>> 8;
		U3_Inf.TX_Data[18] = SlaveG[Target_Address].Inside_WenDu_ProtectValue & 0x00FF;//6���±���ֵ��������һ��

		U3_Inf.TX_Data[19] =  0;
		U3_Inf.TX_Data[20] = SlaveG[Target_Address].Smoke_Protect ;//7�����¶ȱ���ֵ ������һ��

		U3_Inf.TX_Data[21] = 0;
		U3_Inf.TX_Data[22] = Sys_Admin.Water_Max_Percent;//8 ��Ƶ��ˮ��󿪶ȣ����л���һ��

		U3_Inf.TX_Data[23] = Sys_Admin.LianXu_PaiWu_DelayTime;  //�����������ڷ���
		U3_Inf.TX_Data[24] = Sys_Admin.LianXu_PaiWu_OpenSecs;//9�������ۿ������룬������л���һ��



		
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
		U3_Inf.TX_Data[36] = Sys_Admin.Water_BianPin_Enabled;//15  ���ֿ��صı�־λ�������¶ȱ�������Ƶ��ˮ���������ټ��
		
		
		U3_Inf.TX_Data[37] = 0;
		U3_Inf.TX_Data[38] = Sys_Admin.DeviceMaxPressureSet;//16  �����ѹ��
		
		U3_Inf.TX_Data[39] = 0;
		U3_Inf.TX_Data[40] = Sys_Admin.Device_Style ;//17  �豸����

		U3_Inf.TX_Data[41] = 0;
		U3_Inf.TX_Data[42] = sys_config_data.Sys_Lock_Set;//18   �豸����

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
	uint8 Already_WorkNumbers = 0; //已经运行的设备数量统计
	uint8 AliveOK[13] = {0};    //在线设备地址数组统计,1---10
	
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
	
	/* 联动优化: 新增变量 */
	uint32_t totalPower = 0;
	uint8_t avgPower = 0;
	uint8_t bestUnit = 0;
	float setpointPressure = 0.0f;
	static uint8_t rotationCheckCounter = 0;  /* 轮换检查计数器 (秒) */

	static uint32 Loop_time = 0;
	uint8 Loop_Flag = 0;

	static uint16 Period_Check = 0;




	
	if(UnionD.UnionStartFlag == 0 ||  UnionD.UnionStartFlag == 3)  //���ر�־ȡ���ˣ���ֱ�ӷ���
		{
			Loop_Flag = 0;
			Loop_time = 0;

			//��5����һ�Σ��ⲿ�豸�������������ֹû�н��豸�ص�
			if(UnionD.UnionStartFlag == 0)
				{
					if(sys_flag.Union_1A_Sec)
					{
						sys_flag.Union_1A_Sec = 0;   //5����һ��
						
						for(Address = 2; Address <= 10; Address ++)
							{
								if(LCD10JZ[Address].DLCD.Device_State == 2)
									{
									 	//�豸��Ȼ������
									 	SlaveG[Address].Startclose_Sendflag = 3; //����������
										SlaveG[Address].Startclose_Data = 0;//�رոû���
									}
							}
						
						
					}
				}
			

			//�����ж�����״̬�� ���·��Ϳ���ָ�   ��Ҫ��֤  ���ӻ���ʱ��δ���յ�ָ��ͬ���ر�����
			
			return 0;
		}
		

	
  /* 第一步: 找到所有在线的机组，确定在线机组台数 */
  if(sys_flag.Union_1_Sec)
  	{
  		sys_flag.Union_1_Sec = 0; //每秒调用一次
		
		/* ========== 联动优化: 压力预测器更新 ========== */
		Linkage_UpdatePressurePredictor(Temperature_Data.Pressure_Value);
		
		/* ========== 联动优化: 轮换检查 (每60秒检查一次) ========== */
		rotationCheckCounter++;
		if(rotationCheckCounter >= 60) {
			rotationCheckCounter = 0;
			if(Linkage_CheckRotation(sys_time_inf.All_Minutes)) {
				Linkage_DoRotation();
			}
		}
		
		for(Address = 1; Address <= 10; Address ++)
				{
					if(LCD10JZ[Address].DLCD.YunXu_Flag) //ȡ���صı�־
						{
							if(SlaveG[Address].Alive_Flag)//��������
								{
									if(LCD10JZ[Address].DLCD.Error_Code == 0) //û�й���
										{
											AliveOk_Numbres ++;
											AliveOK[AliveOk_Numbres] = Address;

											//�ҳ������ģ�˭����ʱ�����٣������´�����ʹ��
											if(LCD10JZ[Address].DLCD.Device_State == 1)
												{
													if(Min_Address == 0)//��ʼֵ
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

											//��������е�˭ʱ���
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

											//�Թ��ʵı���ʱ����м���
											if(LCD10JZ[Address].DLCD.Air_Power >=(LCD10JZ[Address].DLCD.Max_Work_Power ) )
												{
													SlaveG[Address].Big_time ++;
													SlaveG[Address].Small_time = 0;
													SlaveG[Address].Zero_time = 0;
												}
												

											if(LCD10JZ[Address].DLCD.Air_Power <= 45 && LCD10JZ[Address].DLCD.Air_Power >=25 )
												{
													//����С����ѹ��Ҫ���ڵ����趨ѹ�������ܶ�С���ʼ���
													if(Temperature_Data.Pressure_Value >= sys_config_data.zhuan_huan_temperture_value)
														{
															SlaveG[Address].Small_time ++;
															SlaveG[Address].Big_time = 0;
															SlaveG[Address].Zero_time = 0;
														}
													
												}

											if(LCD10JZ[Address].DLCD.Air_Power > 45  &&  LCD10JZ[Address].DLCD.Air_Power < LCD10JZ[Address].DLCD.Max_Work_Power )
												{
													//�����м�ش��������Ҳ����С������һ������
													
													SlaveG[Address].Small_time  = 0;
													SlaveG[Address].Big_time = 0;
													SlaveG[Address].Zero_time = 0;
												}

											if(LCD10JZ[Address].DLCD.Air_Power == 0) //��ѹͣ¯��״̬
												{
													SlaveG[Address].Big_time = 0;
													SlaveG[Address].Small_time = 0;
													SlaveG[Address].Zero_time ++;
												}
											//���������е�ʱ�� ,ȫ�����ɣ��϶��ÿ�����2�����ϵ�С���ɣ��͵ù�һ��
										}
								}
							else
								{
									SlaveG[Address].Small_time= 0;
									SlaveG[Address].Big_time = 0;
									SlaveG[Address].Zero_time = 0;
									
									/*��Ȩ��������״̬ʱ��ȡ�����������ر���Ӧ�Ĵӻ�*/
									if(LCD10JZ[Address].DLCD.Device_State == 2)
										{
											SlaveG[Address].Startclose_Sendflag = 3; //����������
											SlaveG[Address].Startclose_Data = 0;//�رոû���
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
									SlaveG[Address].Startclose_Sendflag = 3; //����������
									SlaveG[Address].Startclose_Data = 0;//�رոû���
								}
						}
					
				}

			
		for(Address = 1; Address <= 10; Address ++)
			{
				
				//找出运行中的最大压力值
				if(LCD10JZ[Address].DLCD.Steam_Pressure > Max_Pressure)
					{
						if(LCD10JZ[Address].DLCD.Steam_Pressure < 9.99)  //防止压力传感器异常导致值无效
							Max_Pressure = LCD10JZ[Address].DLCD.Steam_Pressure ;
					}
				
				//累计所有运行机组的功率
				if(LCD10JZ[Address].DLCD.Device_State == 2) {
					totalPower += LCD10JZ[Address].DLCD.Air_Power;
				}
					
			}

		/* ========== 联动优化: 计算平均功率和更新自适应周期 ========== */
		if(Already_WorkNumbers > 0) {
			avgPower = (uint8_t)(totalPower / Already_WorkNumbers);
		} else {
			avgPower = 0;
		}
		setpointPressure = sys_config_data.zhuan_huan_temperture_value;  /* 设定压力 */
		Linkage_UpdateAdaptivePeriod(avgPower, Max_Pressure, setpointPressure);
		
		UnionD.Need_Numbers  = 1 ;  //最少保持一台
		
		if(Max_Pressure < 0.35)  //压力小于0.30Mpa,需要增加机组
			{
				if(AliveOk_Numbres <= 2)
					UnionD.Need_Numbers = AliveOk_Numbres;

				if(AliveOk_Numbres > 2)
					UnionD.Need_Numbers = AliveOk_Numbres - 1;
					
			}
		else
			{
				if(AliveOk_Numbres > 1)
					UnionD.Need_Numbers = AliveOk_Numbres - 1; //多台机组需要留一台备用  
			}
	
		/* ========== 联动优化: 压力预测增载判断 ========== */
		if(Already_WorkNumbers < AliveOk_Numbres) {
			if(Linkage_ShouldAddUnit(setpointPressure)) {
				Need_flag = OK;  /* 压力预测触发增载 */
			}
		}
		
		/* ========== 联动优化: 使用自适应周期替代固定15秒 ========== */
		Period_Check ++;
		if(Period_Check >= gAdaptivePeriod.currentPeriod)  /* 自适应周期评估 */
				{
					Period_Check = 0;//���ڱ�־����
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


							if(SlaveG[Address].Small_time > Sys_Admin.Balance_Small_Time) //���ʱ��Ҫ���ڴӻ��������ٵ�ʱ��
								Value_Buffer++;

							if(SlaveG[Address].Zero_time > 500) //��ѹͣ¯����8���ӣ������Զ����۵�ʱ��
								Value_Buffer++;
							
						}

					if(Value_Buffer2 == Already_WorkNumbers) //�����е�ȫ��������
						{
							if(Already_WorkNumbers < AliveOk_Numbres) //���д����Ļ���
								{
									
									Need_flag = OK;
									//�����ʱ��־����ֹ��������
									for(Address = 1; Address <= 10; Address ++)
										SlaveG[Address].Big_time = 0;
										
								}
						}
					
					if(Value_Buffer >= 2)//����̨���ϴ���С���ʵ�״̬���ͼ���
						{
							if(Already_WorkNumbers > UnionD.Need_Numbers)//�Ѿ����еĻ�������������Ҫ���������ͼ���
								{
									Loss_flag = OK;
									//�����ʱ��־����ֹ��������
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


			if(Already_WorkNumbers < UnionD.Need_Numbers)   //��������е�̨��С�����������̨��
				{
					Need_flag = OK;
				}

			if(AliveOk_Numbres == 0)
				{
					UnionD.UnionStartFlag = 0;  //û��һ̨�����Ļ�������ر�
				}
			else
				{
					if(Already_WorkNumbers == AliveOk_Numbres)
					{
						Loop_Flag = FALSE;  //�������ˣ���û�취����
					}
				}

		
  	}

  	 
	
  	

  //������������̨���������ڵ����й���
	

	

	//�����ߵĻ�������ʱ�䣬�Ӵ�С��������
	
		
		if(Already_WorkNumbers <  AliveOk_Numbres)//�������豸������������������
			{
				if(Loop_Flag)
					{
						Loop_Flag = 0;
						Need_flag = OK;  //����һ̨
						Loss_flag =OK;  //����һ̨
					}
			}
	
	if(Need_flag)
		{
			/* 从待机中选择没有运行的最短时间机组启动 */
			Need_flag = FALSE;

			/* ========== 联动优化: 使用加权优先级选择最优机组 ========== */
			bestUnit = Linkage_SelectBestUnit();
			if(bestUnit > 0 && bestUnit <= 10) {
				SlaveG[bestUnit].Startclose_Sendflag = 3;  /* 发送启动命令 */
				SlaveG[bestUnit].Startclose_Data = OK;     /* 启动该机组 */
			}
			else if(Min_Address > 0 && Min_Address <= 10) {
				/* 回退: 使用原有逻辑 */
				SlaveG[Min_Address].Startclose_Sendflag = 3;
				SlaveG[Min_Address].Startclose_Data = OK;
			}
		}
		

		if(Loss_flag)
			{
				//���٣���˭���е�ʱ������ر�
				Loss_flag = FALSE;
				
				/* Bug fix P0: 添加 Max_Address 边界检查防止越界 */
				if(Max_Address > 0 && Max_Address <= 10) {
					SlaveG[Max_Address].Startclose_Sendflag = 3;
					SlaveG[Max_Address].Startclose_Data = 0;
				}
			}

	

		return 0;
}



