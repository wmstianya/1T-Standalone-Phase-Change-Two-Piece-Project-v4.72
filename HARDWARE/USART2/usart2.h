#include "main.h"
#ifndef __USART2_H
#define __USART2_H	  	     
 






#define             macUSART2                                USART2

typedef struct _UNION_flags
{
volatile	uint8 Big_Timer_Flag;
volatile	uint8 Big_Timer_Count;
volatile	uint8 Small_Timer_Flag;
volatile	uint8 Small_Timer_Count;
	uint8 Zhu_Need_Flag;
	uint8 Cong_Need_Flag;
	uint8 Zhu_Less_Flag;
	uint8 Cong_Less_Flag;
	
}UNION_FLAGS;


typedef union _kdsUnion_Inf
{
	uint8 RxData[14];
	struct{

	uint8 Slave_Address;  //字节1
	uint8 Cmd;          //字节2
	uint8 Add_H;     //字节3
	uint8 Add_L;     //字节4
	
	uint8 Start_Cmd;//字节5：开机命令
	uint8 Close_Cmd;//字节6：关机命令
	uint8 Error_Reset_Cmd; //字节7：故障复位命令
	
	uint8 Big_Power;//字节8：大功率的值
	uint8 Small_Power; //字节9：小功率的值
	uint8 Big_Time; // 字节10：大功率时间
	
	
	uint8 Small_Time; //小功率时间
	uint8 unused12;

	
	uint8 check_H;
	uint8 check_L;
	 
			
		
		}True;
	
}	UtoSlave_Info;  //定义从机接收到主机命令协议的解析


typedef union _Mod_Inf
{
	uint8 All[30];
	struct{
	uint8 Address;    //字节1 ： 地址
	uint8 Cmd;			//字节2 ： 命令
	uint8 Length;		//字节3 ： 数据长度
	uint8 Dev_State;//字节4 ：联控启动还是关闭中
	uint8 NeedMore;//字节5 ：当前功率需要增加更多
	uint8 NeedLess;//字节6 ：当前功率不需要这么多

	uint8 Work_State;  //字节7 ：主机的运行状态
	uint8 Error_Code;//字节8 ：故障代码
	uint8 Water_state; //字节9 ：水位状态
	uint8 Pressure;  //字节10 ：当前压力
	uint8 Power; //字节11 ：当前功率
	uint8 Smoke;  //字节12 ：烟温
	uint8 Flame;  //字节13 ：火焰
	uint8 Water_Dev; //字节14 ：补水继电器1开启，0 关闭
	uint8 Air_Dev; //字节15 ：风机
	uint8 unused1;  //字节16 ：保留未使用
	

	uint8 CWork_State;  //字节17 ：从机的运行状态
	uint8 Alive_State;//字节18 ：从机是否在线    ***************额外不同
	uint8 CError_Code;//字节19 ：故障代码
	uint8 CWater_state; //字节20 ：水位状态
	uint8 CPressure;  //字节21 ：当前压力
	uint8 CPower; //字节22 ：当前功率
	uint8 CSmoke;  //字节23 ：烟温
	uint8 CFlame;  //字节24 ：火焰
	uint8 CWater_Dev; //字节25 ：补水继电器1开启，0 关闭
	uint8 CAir_Dev; //字节26 ：风机
	
	

	uint8 unused5;  //字节27 ：保留未使用
	uint8 Paiwu;  //字节28 ：主从机排污的标志
	uint8 check_H;//字节29 ：保留未使用
	uint8 check_L;//字节30 ：保留未使用
	
		}True;
	
}	Duble5_Info;  //双拼1吨;从机发送给主机的数据


typedef struct _LCD_UNIS
{
	
	
	uint16  Start_Close_Flag;
	uint16  Error_Code;
	uint16  Air_Power; 
	uint16  Flame_State;
	uint16  Pump_State;
	
	uint16  Water_State;
	uint16  Paiwu_State;
	uint16  Air_State;
	uint16  lianxuFa_State;
	uint16  Air_Speed;   //20个字节

	float   Steam_Pressure;
	float   Inside_High_Pressure;
	float   Pressure3;
	float   Steam_WenDu;
	float   Smoke_WenDu;
	float   LuNei_WenDu;//44个字节

	uint16  Year;
	uint16  Month;
	uint16  Day;
	uint16  Hour;
	uint16  Min;  //LCD 年 月 日 时 分   //54个字节

	uint16  First_Blow_Time;
	uint16  Last_Blow_Time;
	uint16  Dian_Huo_Power;
	uint16  Max_Work_Power;
	uint16  Danger_Smoke_Value;

	uint16  Fan_Speed_Check;
	uint16  Fan_Fire_Value;
	uint16  Paiwu_Flag; //排污标志
	uint16  Water_BianPin_Enabled; //变频功能
	uint16  Water_Max_Percent;  //调节阀最大开度

	uint16  YuRe_Enabled;  //本体温度保护使能
	uint16  Inside_WenDu_ProtectValue; //本体温度保护值
	uint16  LianXu_PaiWu_DelayTime;  //连续排污周期时间
	uint16  LianXu_PaiWu_OpenSecs; //连续排污每次开启时间
	uint16  ModBus_Address;

	uint16  Fan_Pulse_Rpm;  //每转速脉冲个数
	uint16  Input_Data;  //输入状态
	uint16  ChaYa_Water_Enabled;
	uint16  ChaYa_WaterLow_Set;
	uint16  ChaYa_WaterMid_Set;

	uint16  ChaYa_WaterHigh_Set;
	uint16  Zhu_WaterHigh;
	uint16  Cong_WaterHigh;
	uint16  System_Lock;
	uint16  Balance_Big_Time; //大需求响应时间


	uint16  Balance_Small_Time;  //小需求响应时间
	uint16  Device_State; //多机时，设备状态
	uint16  Work_Time; //主机运行时间
	uint16  YunXu_Flag;
	uint16  Device_Style; //设备风格，0常规单体蒸汽，1则相变蒸汽
	uint16  System_Version;

	float   Target_Value;  
	float   Stop_Value;  
	float   Start_Value;  
	float   Max_Pressure; //额定压力  
	float   buffer_float1;  
	

}LCD_GG;
typedef union  _LCD10_MsESS
{  //
	uint8 Datas[136]; //必须要能够被4整除，防浮点数
	
	LCD_GG DLCD;
	
}LCD10Struct;


typedef struct _LCDJZ_UNIS
{
	//用于各从机的数据显示
	
	uint16  Device_State;  //设备状态
	uint16  Error_Code;
	uint16  Air_Power; 
	uint16  Flame_State;
	uint16  Pump_State;   //10个字节
	
	uint16  Water_State;
	uint16  Paiwu_State;
	uint16  Air_State;
	uint16  lianxuFa_State;
	uint16  Air_Speed;   //20个字节

	
	uint16  First_Blow_Time;
	uint16  Last_Blow_Time;
	uint16  Dian_Huo_Power;
	uint16  Max_Work_Power;
	uint16  Danger_Smoke_Value;  //30个字节

	uint16  YuRe_Enabled;  //本体温度保护使能
	uint16  Inside_WenDu_ProtectValue; //本体温度保护值   
	uint16  YunXu_Flag;  //多台设备时      设备使能标志
	uint16  Work_Time;  //累计运行时间
	uint16  InPut_Data; //最多16位IO口数据  
	
	uint16  Data4;  
	uint16  Data5;
	uint16  Data6;
	uint16  Data7;               //48个字节
/**********************************************************/


	float   Steam_Pressure;
	float   Inside_High_Pressure;
	float   Pressure3;
	float   Pressure4;
	float   Smoke_WenDu;
	float   LuNei_WenDu;//72个字节

	
	
	
	

	
	

}LCD_JZ;
typedef union  _LCD10JZ_MsESS
{  //
	uint8 Datas[72]; //必须要能够被4整除，防浮点数
	
	LCD_JZ DLCD;
	
}LCD10_JZ_Struct;

typedef struct _LCD_WRUNIS
{
	


	uint16  Year;
	uint16  Month;
	uint16  Day;
	uint16  Hour;
	uint16  Min;  //LCD 年 月 日 时 分   //54个字节

	uint16  Data1;
	uint16  Data2;
	uint16  Data3;
	uint16  Data4;
	uint16  Data5;
	
	



	
	
}LCD_WR ;  //只写





extern UtoSlave_Info UtoSlave;

extern Duble5_Info Double5;

extern UNION_FLAGS Union_Flag;

extern LCD10Struct LCD10D;
extern LCD10_JZ_Struct LCD10JZ[13];  //用于机组的数据显示

extern LCD_WR  LCD10WR;

//////////////////////////////////////////////////////////////////////////////////////////////
void uart2_init(u32 bound);	
///////////////////////////////////////USART2 DMA发送配置部分//////////////////////////////////	   		    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//输入参数
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量
//输出参数
//无
/////////////////////////////////////////////////////////////////////////////////////////////// 
void UART_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
//////////////////////////////////////////开启一次DMA传输//////////////////////////////////////
//功能：开启一次DMA传输
//输入参数
//DMA_CHx:DMA通道CHx
//cndtr:数据传输量
//输出参数
//无
////////////////////////////////////////////////////////////////////////////////////////////////


void u2_printf(char* fmt,...)  ;

void U2_send_byte(u8 data);
void U2_send_str(u8 *str,u8 s);








void ModBus2LCD_Communication(void);


uint8 ModuBus2LCD_Write0x10Response(uint16 address,uint16 Data16);

uint8 ModuBus2LCD_WriteAdress0x0000Response(uint16 Buffer_Data16);






#endif













