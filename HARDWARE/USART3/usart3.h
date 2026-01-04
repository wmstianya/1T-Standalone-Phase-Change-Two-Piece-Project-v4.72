#include "main.h"
#ifndef __USART3_H
#define __USART3_H	  	     

/*============================================================================
 * 联动优化模块 - 数据结构定义
 *===========================================================================*/

/* 压力预测器配置 */
#define PRESSURE_HISTORY_SIZE   5       /* 压力历史记录数量 (秒) */
#define PRESSURE_PREDICT_TIME   30      /* 预测时间窗口 (秒) */
#define PRESSURE_RATE_THRESHOLD 0.01f   /* 压力下降速率阈值 (MPa/s) */

/* 自适应周期配置 */
#define PERIOD_CRITICAL   3             /* 临界负荷周期 (秒) */
#define PERIOD_HEAVY      5             /* 重负荷周期 (秒) */
#define PERIOD_MEDIUM     10            /* 中负荷周期 (秒) */
#define PERIOD_LIGHT      15            /* 轻负荷周期 (秒) */

/* 轮换配置 */
#define ROTATION_PERIOD_MINUTES  240    /* 轮换周期 (分钟) = 4小时 */
#define ROTATION_TIME_THRESHOLD  60     /* 运行时间差异阈值 (分钟) */

/* 优先级权重配置 */
#define WEIGHT_WORK_TIME    0.5f        /* 运行时间权重 */
#define WEIGHT_IDLE_TIME    0.3f        /* 空闲时间权重 */
#define WEIGHT_EFFICIENCY   0.2f        /* 效率权重 */

/**
 * @brief 压力预测器结构
 */
typedef struct {
    float pressureHistory[PRESSURE_HISTORY_SIZE];   /* 压力历史环形缓冲 */
    uint8_t historyIndex;                           /* 环形缓冲索引 */
    float pressureRate;                             /* 压力变化率 (MPa/s) */
    float predictedPressure;                        /* 预测压力值 */
    uint8_t initialized;                            /* 初始化标志 */
} PressurePredictor;

/**
 * @brief 自适应周期结构
 */
typedef struct {
    uint8_t currentPeriod;              /* 当前评估周期 (秒) */
    uint8_t loadLevel;                  /* 负荷等级: 0=轻载, 1=中载, 2=重载, 3=临界 */
} AdaptivePeriod;

/**
 * @brief 轮换控制结构
 */
typedef struct {
    uint32_t lastRotationMinutes;       /* 上次轮换时间 (分钟) */
    uint8_t rotationEnabled;            /* 轮换使能标志 */
} RotationCtrl;

/* 外部变量声明 */
extern PressurePredictor gPressurePredictor;
extern AdaptivePeriod gAdaptivePeriod;
extern RotationCtrl gRotationCtrl;

/* 联动优化函数声明 */
void Linkage_Init(void);
void Linkage_UpdatePressurePredictor(float currentPressure);
void Linkage_UpdateAdaptivePeriod(uint8_t avgPower, float pressure, float setpoint);
uint8_t Linkage_ShouldAddUnit(float setpoint);
uint8_t Linkage_CheckRotation(uint32_t currentMinutes);
void Linkage_DoRotation(void);
uint8_t Linkage_SelectBestUnit(void);
float Linkage_CalcPriority(uint8_t address);

/*============================================================================*/

extern uint8 USART3_RX_BUF[256];
extern uint16 USART3_RX_STA;
extern uint8 re3_time_falg;
extern uint16 re3_time;
extern uint8 send3_falg;


///////////////////////////////////////USART2 printf֧�ֲ���//////////////////////////////////
//����2,u2_printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
////////////////////////////////////////////////////////////////////////////////////////////////
void u3_printf(char* fmt, ...);
///////////////////////////////////////USART2 ��ʼ�����ò���//////////////////////////////////	    
//���ܣ���ʼ��IO ����2
//�������
//bound:������
//�������
//��
//////////////////////////////////////////////////////////////////////////////////////////////
void uart3_init(u32 bound);	

uint8 JiZu_ReadAndWrite_Function(uint8 address);
void ModBus_Uart3_LocalRX_Communication(void);

uint8 Modbus3_UnionTx_Communication(void);
uint8 ModBus3_RTU_Write06(uint8 Target_Address,uint16 Data_Address,uint16 Data16);
uint8 ModBus3_RTU_Read03(uint8 Target_Address,uint16 Data_Address,uint8 Data_Length );
uint8 Modbus3_UnionRx_DataProcess(uint8 Cmd,uint8 address);

uint8 ModBus3_RTU_Write10(uint8 Target_Address,uint16 Data_Address);
uint8 Union_MuxJiZu_Control_Function(void);



#endif

