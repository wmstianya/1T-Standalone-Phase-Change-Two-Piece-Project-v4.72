 
#ifndef __SYS_CONTROL_H
#define	__SYS_CONTROL_H

#include "main.h"


#define OK  1
#define FALSE  0

#define  NO_DEBUG 0  //NO_DEBUG = 0 时，表示调试状态, = 1时需输入激活码

#define TURE_FLAG 1  //该标志为0时，不检测传感器如温度和压力，去除传感器报警

//关键参数设置,流量，火焰，风门相关状态量的标志

#define  FLAME_OK  0X01  //有火焰
#define  FLAME_OUT 0X00  //无火焰

#define  WATER_OK 0X01	//有水流信号

#define  WATER_LOSE 0X00 //无水流信号


#define  GAS_ON  0X00   //燃气压力正常
#define  GAS_OUT 0X01	//没有燃气


#define  AIR_OPEN  0X00  //风门正常开启

#define  AIR_CLOSE  0X01 
#define   THERMAL_OK  	0X01
#define     THERMAL_BAD   0X00


#define  PRESSURE_OK  0X01  //机械式压力信号正常
#define  PRESSURE_ERROR 0X00 //机械式压力超压报警




#define Set_Bit_0   0x01
#define Set_Bit_1   0x02
#define Set_Bit_2   0x04
#define Set_Bit_3   0x08
#define Set_Bit_4		0x10
#define Set_Bit_5   0x20
#define Set_Bit_6   0x40
#define Set_Bit_7		0x80


#define Rst_Bit_0		0xFE
#define Rst_Bit_1		0xFD
#define Rst_Bit_2		0xFB
#define Rst_Bit_3		0xF7
#define Rst_Bit_4		0xEF
#define Rst_Bit_5		0xDF
#define Rst_Bit_6		0xBF
#define Rst_Bit_7		0x7F


#define SET_BIT_N(x,n)  ((x) | (1 << ((n) - 1)))

#define CLR_BIT_N(x,n)  ((x) & ~(1 << ((n) -1)))

#define GETBITS(x,n,m)  ((x &~(~(0U)<<(m-n+1)) << (n-1)) >>(n-1))





//==============================

#define CLOSE_INSIDE_LOOP_TEM     950 //用于标定循环泵后的温度

#define MAIN_PUMP_START_TIME     4000   //两次补水间隔要大于5秒



#define Max_Pressure_Set          100  //用户最大可调节的压力

#define Stop_Pressure_Set        100

#define  Max_Ignition_Times    3  //最大点火次数

#define Max_Dian_Huo_Power   60  //最大点火功率

#define Min_Dian_Huo_Power   15  //最小点火功率


// -----------系统模式--------------
enum
{
    SYS_IDLE  = 0,  //待机
    SYS_ALARM,  //报警
    SYS_WORK,  // 工作
    SYS_MANUAL,//手动
    SYS_CLEAN_MODE//除垢模式
};


 enum
 {
	 NO_ERROR  = 0,  //没有故障
	 Error1_YakongProtect,  //1 机械压控保护
	 Error2_YaBianProtect,	//2  压力变送器保护
	 Error3_LowGas,//3 燃气低保护
	 Numbers_4, //4 未使用
	 Error5_LowWater,//5极低水位保护,
	 Error6_SupplyWater_tooLong,//6水补不上来
	 Error7_FlameZiJian,//7火焰自检
	 Error8_WaterLogic,//8水位逻辑故障
	 Error9_AirPressureBad,//9 风门异常
	 Error10_SteamValueBad,//10 蒸汽温度超过设定值（外置温控盒）
	 Error11_DianHuo_Bad,//11点火失败
	 Error12_FlameLose, //12运行中火焰熄灭
	 Error13_AirControlFail, //13 风机控制失效
	 Error14_BenTiValueVBad,//14 本体温度异常（外置温控盒）
	 Error15_RebaoBad,  //15 本体热保护
	 Error16_SmokeValueHigh,//16 烟气温度超温
	 Error17_OutWenKong_TxBad,// 17 外置安全通信超时
	 Error18_SupplyWater_Error,//18 进水阀故障
	 Error19_SupplyWater_UNtalk,//19 进水阀通信丢失
	 Error20_XB_HighPressureYabian_Bad,//20 相变高压侧压力变送器超压
	 Error21_XB_HighPressureYAKONG_Bad,//20 相变高压侧机械压控超压
	 Error22_XB_HighPressureWater_Low,
	 unused// 22 未使用
	
	 
	   
 };


 typedef enum {First, Second, Third, Fourth,Five,Six,Seven,Eight,Nine,Ten,ELeven,Twleve,Thirteen,Fourteen,Fifteen,Sixteen} JUMP_TYPE;


typedef struct _SYS
{
	uint8_t Data_10H; 
	/*10H = 0：待命
	10H = 2: 运行
	10H = 3; 手动
	 
*/
	uint8_t Data_l1H;  //用于I/O输入数据存储
/*
11H.0：低水位电极状态
11H.1：高水位电极状态
11H.2：风门开关状态 	
11H.3：极低水位状态
11H.4：超高水位状态
11H.5：燃气压力状态	 
11H.6：火焰信号
11H.7：后吹扫状态

*/
uint8_t Data_12H; //用于异常状态保存
/*
12H.0：压力高于停炉值
12H.1： 
12H.2： 
12H.3： 
12H.4: 自动启停，调节压力中
12H.5:  

*/
	uint8_t Data_l3H;
/*
13H.0：风机启停控制   ，1则启动，0则关闭	
13H.1： 
13H.2：燃气阀组， 1则开启，0关闭	
13H.3：点火，			1则开启，0关闭
13H.4： 
13H.5： 
13H.6: 补水泵   1 	则开启， 0 则关闭
13H.7: 排污阀  
*/
	uint8_t Data_14H; 
/*
14H.0： 风门信号异常
14H.1： 蒸汽温度超过设定值 
14H.2：点火未成功
14H.3：运行中火焰熄灭
14H.4：风机控制失灵 
14H.5： //本体温度超过设定值 
14H.6： 热保护故障
14H.7： 排烟温度超高异常
*/
	uint8_t Data_l5H;

/*
15H.0：蒸汽压力超出安全范围 
15H.1：燃气压力低 	 	
15H.2：温度传感器或压力变送器异常
15H.3： 
15H.4：极高水位异常 
15H.5：极低水位保护
15H.6：火焰传感器自检异常
15H.7：水位探针逻辑故障
*/
	uint8_t Data_16H; //下集箱温度高位  
	uint8_t Data_l7H;//下集箱温度低位

 

	uint8_t Data_18H; 
	uint8_t Data_19H;//蒸汽压力值 一个字节 最大 1.0Mpa = 100

	uint8_t Data_1AH;//蒸汽温度高位
	uint8_t Data_1BH;//蒸汽温度低位

	uint8_t Data_1CH;//排烟温度高位  ，一个字节，最大100度
	uint8_t Data_1DH;//排烟温度低位

	uint8_t Data_1EH;// 同步控制器的设置值
	uint8_t Data_1FH;//风机转速百分比(1Byte)



			uint8_t Data_20H; 
			uint8_t Data_21H; 

			uint8_t Data_22H; 
			uint8_t Data_23H;  
	

	
			uint8_t Data_24H; 
			uint8_t Data_25H; 

			uint8_t Data_26H; 
			uint8_t Data_27H;  

			uint8_t Data_28H;
			uint8_t Data_29H;
			uint8_t Data_2AH;
			uint8_t Data_2BH; 


			uint8_t Data_2CH;  
			uint8_t Data_2DH;  
			uint8_t Data_2EH;  


			
			uint8_t Data_2FH;
			uint8_t Data_30H;//本次运行时间，小时（2BYTE）
			uint8_t Data_31H;//本次运行时间，分钟(1BYTE)

			uint8_t Data_32H;
			uint8_t Data_33H;// 
			uint8_t Data_34H;
			uint8_t Data_35H;// 
			uint8_t Data_36H;// 
			uint8_t Data_37H;// 
			uint8_t Data_38H;// 
			uint8_t Data_39H;// 
			
			 
	
	
	
}SYS_INF;



typedef union _COPY_SYS_INF
{
	uint8 array[100];
	SYS_INF sym_data;
}SYS_COPY;




typedef struct _AB_EVENT
{
		uint8 flameout_event;  //运行中，火焰熄灭异常
		uint8 flameout_count;	//记录运行中，火焰熄灭次数
	
		uint8 overheat_event;  //炉内超温异常
		uint8 overheat_count;  //炉内超温次数记录
	
		uint8 airdoor_event;  //磁力开关异常
		uint8 burner_heat_protect_event; //燃烧器热保护异常
		uint8 burner_heat_protect_count;//对燃烧器热保护异常次数进行记录，在一次系统运行中，出现两次，则待机报警

		uint8 target_complete_event;//达到设定温度两度以上
}AB_EVENTS;





typedef struct _CONFIG
{

		
		uint32 zhuan_huan_temperture_value; 	
		uint32 Auto_stop_pressure; //自动停炉压力
		uint32 Auto_start_pressure; //自动启炉压力
		
		uint32 Sys_Lock_Set; //远程一键锁停控制
		
		uint32  ignition_time;//统计点火次数
		uint32  wifi_record;  //用于记录wifi是否配置过
		uint32  WIFI_MAC;//用于记录WIF的MAC地址后四个字节

			
}SYS_CONFIG;



typedef struct _ADMIN
{
	uint32 First_Blow_Time;
	uint32 Last_Blow_Time;
	uint32 Wen_Huo_Time;
	uint32 Stop_Temperature;
	uint32 Dian_Huo_Power;
	uint32 Admin_Work_Day;
	uint32 Admin_Save_Year;
	uint32 Admin_Save_Month;
	uint32 Admin_Save_Day;

	uint32 Max_Work_Power;

	uint32 Fan_Speed_Check;//是否检测风速，默认是检测
	uint32 Fan_Speed_Value; //风机最大转速
	uint32 Fan_Pulse_Rpm; //风机脉冲数跟转速的关系   

	uint32 Fan_Fire_Value;

	uint32 Danger_Smoke_Value; //排烟温度报警值
	uint32 Auto_Blow_Set; //自动排污设置，0为没有，1为有

	uint32 Fire_Mode_Set; //引火方式设置  0为传统，1为明火

	uint32 ModBus_Address_Set;//ModBUS地址

	uint32 ModBus_Function;
	uint32 ModBus_Address;

	uint32 Supply_Max_Time;

	uint32 DeviceMaxPressureSet;

	uint32 Access_Data_Mode;  //用于远程获取数据的模式  ，0默认为被动获取，1 则为主动获取


	uint32 PaiwuStage1_Time;
	uint32 PaiwuStage1_OpenFlag;  //为 1时，才启用
	uint32 PaiwuStage2_Time;
	uint32 PaiwuStage2_OpenFlag;  //为 1时，才启用
	uint32 PaiwuStage3_Time;
	uint32 PaiwuStage3_OpenFlag;  //为 1时，才启用


	uint32 Balance_Big_Power;  //负荷平衡大功率
	uint32 Balance_Small_Power;  //负荷平衡小功率
	uint32 Balance_Big_Time;  //负荷平衡大功率时间
	uint32 Balance_Small_Time;  //负荷平衡小功率时间

	uint32 Fire_Detect_Mode;  //火焰检测方式，0默认控制器检测，1则外置火焰检测
	uint32 Login_Control;  //用于操作三级用户管理
	uint32 Special_Secs; //高温回水时间控制
	

	uint32 BenTi_WenDu_Protect;
	uint32 Steam_WenDu_Protect;

	uint32 WenDu_Protect_Flag;
	uint32 SafetyProtect_Flag;
	

	uint32 WaterUnchangeMaxTime;
	uint32 WaterSupplyTime; 


	uint32 Inside_WenDu_ProtectValue;
	uint32 Water_UnchangeMAX_Time;

	uint32 YuRe_Enabled;
	uint32 Slave_YuRe_Enabled;

	uint32 Water_Max_Percent;
	uint32 Water_BianPin_Enabled;
	uint32 Device_Style;  //设备类型参数


	uint32 LianXu_PaiWu_Enabled;//连续排污使能
	uint32 LianXu_PaiWu_DelayTime;//连续排污间隔时间
	uint32 LianXu_PaiWu_OpenSecs;//连续排污阀每次开启的时间

	uint32 ChaYa_WaterHigh_Enabled;
	uint32 ChaYa_WaterLow_Set;
	uint32 ChaYa_WaterMid_Set;
	uint32 ChaYa_WaterHigh_Set;
	

	uint32 AutoOpen1_Hour;
	uint32 AutoOpen1_Min;
	uint32 AutoOpen1_Flag;
	uint32 AutoOpen2_Hour;
	uint32 AutoOpen2_Min;
	uint32 AutoOpen2_Flag;

	uint32 AutoClose1_Hour;
	uint32 AutoClose1_Min;
	uint32 AutoClose1_Flag;
	uint32 AutoClose2_Hour;
	uint32 AutoClose2_Min;
	uint32 AutoClose2_Flag;
	

}SYS_ADMIN;



typedef struct _sys_work_time //系统累计时间结构体
{
		uint8 sec;
		uint8 min;
	  uint32 hour;
	  uint16 days;
	  uint32 All_Minutes; //大火累计时间，以分钟进行统计
	  uint32 UnPaiwuMinutes;
	  
}SYS_WORK_TIME;


typedef struct _LCD_MESS  //LCD 内存结构体
{
	
//	uint8 Head_H;
//	uint8 Head_L;
//	uint8 Data_Length;
	uint8 Lcd_Cmd;
	uint8 Start_Address_H;
	uint8 Start_Address_L;
	
	uint8 Data_10H;
	uint8 Data_10L;  
								
	uint8 Data_11H;
	uint8 Data_11L;  
	
	uint8 Data_12H;
	uint8 Data_12L;// 
	
	uint8 Data_13H;
	uint8 Data_13L;//风机功率显示 
	
	uint8 Data_14H;
	uint8 Data_14L;//烟温精确度0.1度
	
	uint8 Data_15H;
	uint8 Data_15L;// 水位状态显示
	
	uint8 Data_16H;
	uint8 Data_16L; //  保留未使用
	
	uint8 Data_17H; 
	uint8 Data_17L;  //火焰动画显示	（两个字）
	uint8 Data_18H;
	uint8 Data_18L; 

	
	uint8 Data_19H;
	uint8 Data_19L;//补水动画显示（两个字）
	uint8 Data_1AH;
	uint8 Data_1AL; 
	
	uint8 Data_1BH;
	uint8 Data_1BL; ///风机动画显示（两个字）
	uint8 Data_1CH;
	uint8 Data_1CL; 

	
	uint8 Data_1DH;
	uint8 Data_1DL; 
	uint8 Data_1EH;
	uint8 Data_1EL;
	uint8 Data_1FH;
	uint8 Data_1FL; 
	
	uint8 Data_20H;
	uint8 Data_20L;  //额定蒸汽压力的值
	uint8 Data_21H;
	uint8 Data_21L;  //系统运行时间
	
	
	uint8 Data_22H; 
	uint8 Data_22L;  //蒸汽温度
	uint8 Data_23H;
	uint8 Data_23L; //本体温度
	
	uint8 Data_24H; 
	uint8 Data_24L;
	uint8 Data_25H;
	uint8 Data_25L;
	
	uint8 Data_26H; 
	uint8 Data_26L;   
	uint8 Data_27H;
	uint8 Data_27L;
 	
	uint8 Data_28H; 
	uint8 Data_28L;  
	uint8 Data_29H;
	uint8 Data_29L;  //未使用	
	
	
	uint8 Data_2AH; 
	uint8 Data_2AL;  //address:0x30_系统压力指针控制
	uint8 Data_2BH;
	uint8 Data_2BL; //address:0x31_系统大小火指针控制


	uint8 Data_2CH;
	uint8 Data_2CL;   
	uint8 Data_2DH;
	uint8 Data_2DL;  
//=====================================	
	uint8 Data_2EH;
	uint8 Data_2EL;  //排污时间提示图标
	uint8 Data_2FH;
	uint8 Data_2FL;   //排污计时： 小时
//=====================================	
	uint8 Data_30H;
	uint8 Data_30L;  //排污计时：分钟 
	uint8 Data_31H;
	uint8 Data_31L;   

//=====================================	
	uint8 Data_32H;
	uint8 Data_32L;  //风速实时显示
	uint8 Data_33H;
	uint8 Data_33L; 
//=====================================		
	uint8 Data_34H;
	uint8 Data_34L; // 
	
	uint8 Data_35H;
	uint8 Data_35L; // 
//=====================================
	uint8 Data_36H;
	uint8 Data_36L; // 
	uint8 Data_37H;
	uint8 Data_37L; //  
//=====================================
	uint8 Data_38H;
	uint8 Data_38L;   
	uint8 Data_39H;
	uint8 Data_39L;  
	
//=====================================
	uint8 Data_3AH;
	uint8 Data_3AL;  //用户设定压力值

	
	uint8 Data_3BH;
	uint8 Data_3BL;  // 

	uint8 Data_3CH;
	uint8 Data_3CL;// 
//===========================================================================	
	uint8 Data_3DH;
	uint8 Data_3DL; // 
	uint8 Data_3EH;
	uint8 Data_3EL;// 
	uint8 Data_3FH;
	uint8 Data_3FL;  //显示软件版本号


	
	
}LCD_MEM;



typedef struct _LOGIN_3SA//系统累计时间结构体
{
		
	  uint32 Login_KeyH[7]; //登录密码高位
	  uint32 Login_KeyL[7]; //登录密码低位
	  
	  uint32 Login_Acccess[7]; //登录授权
	  
	  
}Login_TT;


typedef struct _LCD_MASS  //LCD 内存结构体
{
	
//	uint8 Head_H;
//	uint8 Head_L;
//	uint8 Data_Length;
	uint8 Lcd_Cmd;
	uint8 Start_Address_H;
	uint8 Start_Address_L;
	
	uint8 Data_10H;
	uint8 Data_10L;  
								
	uint8 Data_11H;
	uint8 Data_11L;  
	
	uint8 Data_12H;
	uint8 Data_12L;// 
	
	uint8 Data_13H;
	uint8 Data_13L;//风机功率显示 
	
	
}LCD_MAM;






typedef union _LCD_READ
{
		
	
		struct{
		uint8 Data_60H;
		uint8 Data_60L;  
	
		uint8 Data_61H;
		uint8 Data_61L;   
//=====================================
    	uint8 Data_62H;
		uint8 Data_62L;  
	
		uint8 Data_63H;
		uint8 Data_63L;   
//=====================================
		uint8 Data_64H;
		uint8 Data_64L;  
	
		uint8 Data_65H;
		uint8 Data_65L;  
//=====================================
    	uint8 Data_66H;
		uint8 Data_66L;  
	
		uint8 Data_67H;
		uint8 Data_67L;  
//=====================================		
		uint8 Data_68H;
		uint8 Data_68L;   
	
		uint8 Data_69H;
		uint8 Data_69L;   
//=====================================		
		uint8 Data_6AH;
		uint8 Data_6AL;  
	
		uint8 Data_6BH;
		uint8 Data_6BL;  //鼓引风时间：6AH有效		
		
//=====================================	
		uint8 Data_6CH;
		uint8 Data_6CL;  
	
		uint8 Data_6DH;
		uint8 Data_6DL;  //稳定火焰时间：6CH有效	


//=====================================	
		uint8 Data_6EH;
		uint8 Data_6EL;  //转换压力设置：6EH有效
	
		uint8 Data_6FH;
		uint8 Data_6FL;  
		}address;
		
		uint8 data[32];
				



}Lcd_Read_Data;







//用于统计并转串，共16位的IO信息
typedef union _IO_DATA
{
		 
		struct{
				uint8 SER1_Data[8];
				uint8 SER2_Data[8];
		}Data;
			
		struct{
				uint8 water_protect;// 1用于极低水位保护，一旦无信号，报警停炉
				uint8 water_mid;   //2中水位信号
				uint8 water_high; //3高水位信号   //蒸汽使用
				uint8 water_shigh;//4超高水位信号
				
				uint8 Flame_Signal; //5 外置火焰检测信号
				uint8	XB_Hpress_Ykong;//6   内侧相变压控
				uint8 XB_WaterLow; // 7 ==  
				uint8	XB_WaterHigh;// 8 相变时，作为内胆压控保护 
				
				uint8  gas_low_pressure ;//9 燃气压力低状态 
				uint8  Air_Door;// 10 风压开关  
				uint8 hot_protect;//11,热保护开关
				uint8 hpressure_signal;//12机械式压力检测信号
				
		}Target;
		
	
}IO_DATA;

typedef union _FLOAT_S
{
		float value;//STM32默认为小端模式
		struct
			{
				uint8 data_LL;	//低地址
				uint8 data_LH; 
				uint8 data_HL;
				uint8 data_HH;//高地址

			}byte4;
}FLP_INT;

typedef union _UINT32_S
{
		uint32 value;//STM32默认为小端模式
		struct
			{
				uint8 data_LL;	//低地址
				uint8 data_LH; 
				uint8 data_HL;
				uint8 data_HH;//高地址

			}byte4;
}BYTE_INT32;

typedef union _4byte2word
{
		uint32 lcd_secret;
		struct
			{
				uint8 data_HH;
				uint8 data_HL;
				uint8 data_LH;
				uint8 data_LL;		
			}byte4;
}BYTE_WORD4;

typedef union _2byte1word
{
		uint16 data16;
		struct
			{
				uint8 data_L;
				uint8 data_H;		
			}byte2;
}BYTE_WORD1;

//lcd 三路温度曲线图数据结构体
typedef struct _LCD_PROFILE
{
	uint8 Head_H;//  = 0x5A;
	uint8 Head_L;//  = 0xA5;
	uint8 data1;//  = 0x08;
	uint8 data2;// = 0x84;
	uint8 data3;// = 0x07;

	uint8 out_water_H;
	uint8 out_water_L;
	uint8 back_water_H;
	uint8 back_water_L;
	uint8 smoke_tem_H;
	uint8 smoke_tem_L;
}LCD_QuXian;



typedef union _Err_tran
{
//	uint8 ERR_bit[16];
	struct{
			uint8 ERR_15H[8];
			uint8 ERR_14H[8];
		}BIT;
	struct
		{
	/*
14H.0：风门故障 
14H.1：燃气阀组泄漏
14H.2：点火失败 
14H.3：系统运行过中火焰熄 
14H.4：前四进水管热保故障
14H.5：中四进水管热保故障
14H.6：后四进水管热保故障
14H.7： 
*/

/*
15H.0：蒸汽压力超出安全范围 
15H.1：燃气压力低故障	 	
15H.2：水循环无流量信号
15H.3：内流量计通信故障
15H.4：外流量计通信故障
15H.5： 
15H.6：火焰探测器自检故障
15H.7：水位探针逻辑故障
*/
			uint8 error_1; // 蒸汽压力超出安全范围 
			uint8 error_2;//燃气压力低故障	 
			uint8 error_3; //水循环无流量信号
			uint8 error_4;// 内流量计通信故障
			uint8 error_5;//外流量计通信故障
			uint8 error_6; //水位极低保护故障
			uint8 error_7;//火焰探测器自检故障 
			uint8 error_8;// 水位探针逻辑故障

			
			uint8 error_9; //风门开关故障
			uint8 error_10;//燃气阀组泄漏故障
			uint8 error_11; //点火失败故障
			uint8 error_12;//系统运行中火焰熄灭故障
			uint8 error_13;//前四进水管热保故障
			uint8 error_14;//中四进水管热保故障
			uint8 error_15;//后四进水管热保故障
			uint8 error_16;//
			
		}ERR;

		
}ERR_LCD;


//此结构体用于LCD记录故障信息
typedef struct LCDE_R
{
	uint8 Cmd;
	uint8 AddH;
	uint8 AddL;

	uint8 MON;
	uint8 date;
	uint8 hour;
	uint8 min;
	uint8 data_H;
	uint8 data_L;
	
}LCD_E_M;

typedef struct Load_ERR_S
{
	LCD_E_M  Error1;
	LCD_E_M  Error2;
	LCD_E_M  Error3;
	LCD_E_M  Error4;
	LCD_E_M  Error5;
	LCD_E_M  Error6;
	LCD_E_M  Error7;
	LCD_E_M  Error8;
}ERROR_DATE_STRUCT;

typedef struct Load_LCD_FLASH
{
	uint16   data1;

	LCD_E_M  Error1;
	LCD_E_M  Error2;
	LCD_E_M  Error3;
	LCD_E_M  Error4;
	LCD_E_M  Error5;
	LCD_E_M  Error6;
	LCD_E_M  Error7;
	LCD_E_M  Error8;
}LCD_FLASH_STRUCT;


/***
typedef struct _UART_INF
{
	uint8 TX_Data[300];//发送数据缓存
	uint8 RX_Data[300];//接收数据缓存
	uint8 Rx_temp_length; //接收中断临时变量
	uint8 RX_Length;  //所接收的数据长度
	uint8 send_flag;
	uint8 Recive_Time;//接收时间间隔
	uint8 Recive_Flag;//接收到第一个字节标志
	uint8 Recive_Ok_Flag;//完全接收完成标志
}UART_DATA;
****/
// 在现有的UART_DATA结构体中添加DMA相关字段

typedef struct _UART_INF
{
    uint8 TX_Data[300];         // 发送数据缓冲
    uint8 RX_Data[300];         // 接收数据缓冲
    uint8 Rx_temp_length;       // 临时接收长度（DMA模式下保留兼容性）
    uint8 RX_Length;            // 接收数据长度
    uint8 send_flag;
    uint8 Recive_Time;          // 接收超时计数（保留兼容性）
    uint8 Recive_Flag;          // 接收标志（保留兼容性）
    uint8 Recive_Ok_Flag;       // 接收完成标志
    uint8 Flag_100ms;           // 100ms标志
    
    // DMA相关新增字段
    uint8 DMA_RX_Buffer[300];   // DMA接收缓冲区
    uint16 DMA_RX_Size;         // DMA设置的缓冲区大小
    uint8 DMA_Mode_Enable;      // DMA模式使能标志
}UART_DATA;



typedef struct _sys_flags
{
volatile	uint8 XB_WaterLow_Flag;
	volatile uint16 XB_WaterLow_Count;

	volatile uint16 XB_WaterLowAB_Count;
	volatile uint16 XB_WaterLowAB_RecoverTime;

	uint16 ChaYaWater_Value;
	uint16 Cong_ChaYaWater_Value;
	

	uint8 Work_1S_Flag;
	uint16 AirWork_Time ;

	uint8 Interrupt_Alive;  //用于继电器过零检测中断的判定
	uint8 Special_100msFlag;
	uint8 Address_1;
	uint8 Address_2;
	uint8 Address_3;
	uint8 Address_4;

	uint8 Union_1A_Sec;
	uint8 Union_1_Sec;

	uint8 Wifi_Lock_System;  //物联网远程锁机功能
		uint16 wifi_Lock_Year;
		uint8 wifi_Lock_Month;
		uint8 Wifi_Lock_Day;


	uint8 SPI_ReadRX_Flag;
	uint16 Protect_WenDu;
	uint16 Value_Buffer;
	uint16 Value_Buffer2;
volatile	uint8 ADC_100msFlag;
	uint16 Inputs_State;

	volatile	uint8  test_Ms;
	volatile	uint8  test_OFF;
	volatile	uint8  test_Result;
	


	uint16 Pressure_buffer;

	uint8 Water_Percent;
	uint8 Water_Error_Code;
		uint8 waterSend_Flag;
	uint8 waterSend_Count;
	
	volatile	uint8 LianXu_1sFlag;
	volatile	uint32 LianxuWorkTime;
	volatile	uint32 Lianxu_OpenTime; //连续排污阀的开启时间统计，精确到0.1s
	volatile	uint8 LianXu_100msFlag;

	volatile	uint8 WaterAJ_Flag;

	uint16 PC0_ADC;
	uint16 PC1_ADC;

	volatile	uint8 Error1_Count;	
	volatile	uint8 Error1_Flag;
	volatile	uint8 Error15_Count;	
	volatile	uint8 Error15_Flag;
	

	volatile	uint8 Error16_Count;	
	volatile	uint8 Error16_Flag;
	volatile	uint8 Error5_Count;	
	volatile	uint8 Error5_Flag;
	volatile	uint8 Force_Count;	
	volatile	uint8 Force_Flag;
	
	volatile	uint16 HighLoss_Flag;
	volatile	uint16 HighLoss_Count;

	volatile	uint8 WenDu_Send_Flag;
				uint8 Attention_Flag;
	volatile	uint16 WenDu_Send_Count;
	volatile	uint16 WaterUnsupply_Count;
	uint8 Address_Number; //0则主控标志，1，2,3，4则为从机地址标志

	uint8 UnTalk_Time; 

	uint16 Pressure_ChangeTime;
	uint8 Pressure_1sFlag;

	volatile uint8  FlameFilter;
	volatile	uint8 Safety_Send_Flag;
	volatile	uint16 Safety_Send_Count;
	volatile	uint8 Work_1sec_Flag;

	volatile	uint8 Tx_3Sec_Flag;
	
	uint16  Steam_Value ;
	uint16  BenTi_Value;
		uint8 Paiwu_Flag;
		uint8 Alarm_Out;
		uint8 Rpm_1_Sec;


		uint8 WenKongSafety;

		uint8 Login_Number; //登录人员序列
		uint8 Login_Buffer;//准备登录的人
		uint8 Login_Lock;  //用户锁屏

		uint16 Low_Count;
		uint8 FlameOut_Count;
		uint16 FlameRecover_Time;
		uint8 Paiwu_Secs;
		uint8 Lcd_Read_Flag;
		uint8 Access_Data_Flag; //用于远程获取数据标志
		uint8 Begin_Check ;  //用于开机检查分项完成标志
		uint8 Check_Finsh; //用于做开机检查完成标志
		uint8 Lock_System;  //用于管理可运行天数，系统锁定功能
		uint8 Lcd_Unconnect_Time;  //用于LCD跟主板的定时通信
		uint8 Lcd_First_Connect;
		uint8 Lost_Host_Time;
		uint8 Error_Code;
		uint8 Find_Host;
	volatile	uint8 Pressure_HFlag;
	volatile	uint8 Pressure_HCount;
	volatile	uint8 WenDu_HFlag;
	volatile	uint8 WenDu_HCount;

	volatile	uint16 Old_Error_Count;  //用于上次故障时间的记录


	volatile	uint8 High_Lose_Flag;
	volatile	uint8 High_Lose_Count;

		uint8 Wts_Gas_Index; //用于WTS自制燃气检漏
		uint8 Begin_Check_Index; //用于开机检查跳转
		
		uint8 flame_state; //用于记录火焰的状态
		uint8 Target_Page;
		uint8 Force_UnSupply_Water_Flag; //强制不补水标志
		uint8 Force_Supple_Water_Flag; //强制补水标志
		uint8 Force_JIGAO_Level;  //强制补水极高标志

		uint8 Lcd_Value_Check1S_Flag;
		
		uint8 check_time_60s_Flag;
		uint8 Pai_Wu_Idle_Index;

		uint16 Test_Buffer12;
		
		

		uint8 Ignition_Count; //单次点火的次数，最大两次
		
		
		
		uint8 Lock_Error; //用于锁定故障信息，防止重复处理
		uint8 last_blow_flag;//后吹扫执行标志，=1时表示正在执行后吹扫
		uint8 freeze_protect_check_begin;//系统复位开机，等待1到2分钟，检测
		uint8  Already_Work_On_Flag; //标志锅炉已经运行过，设标志

		uint8 Manual_open_water_suplly_flag; //手动补水标志
		 
		uint8 tx_hurry_flag;//用于紧急发送数据给服务器	
		uint8 flag_200ms; //200ms flag
		 
		uint8 flag_300ms; //300ms flag

	volatile	uint8 Power_1_Sec; //风机功率一秒标志
	volatile	uint8 Power_2_Sec;//风机功率2秒标志
	volatile	uint8 Power_5_Sec;//风机功率5秒标志

	volatile	uint16 LianXu_Work_Time;

		volatile	uint8 Relays_3Secs_Flag;//继电器脉冲信号

	volatile 	uint8 time_60s_count;
	 	uint8  Sys_1_Sec;
		uint8 two_sec_flag;//两秒标志，用于缓慢增加风速
		uint8 sec_5_flag;//五秒标志
		
		uint8 sec_120_flag;
		 

		 
		uint8 sec_20_flag;//20秒标志
	volatile uint8 Water_Relay_2_Sec_Flag; //用于补水继电器频繁开启，强制大于2s
		

		uint8 ModBus_Mode; //控制ModBus通信模式
		uint8 slave_flag[4];//控制从机的个数
		uint8 response_flag[4];
		
		
		uint8 slave_address;
		
	
		uint8 before_ignition_index; //用于点火前跳转变量
		uint8 get_60_percent_flag; //点火达到过60
	
		
	volatile uint16 Fan_count; //用于风机每半个周期占用的时间检测

		uint16  Fan_Rpm ; //用于风机转速
		
		 
		uint16 Pump_Close_Time;  //补水泵超时监测
		uint16 Pump_Open_Time;

		uint16 WaterOut_Time;
		uint16 WaterClose_Time;  //用于控制水泵和补水电磁阀开启的先后

		 
	volatile	uint8   sys_delay_up;  //系统延迟变量
	volatile	uint8    sys_delay_start;
	volatile uint16 sys_delay_time;
		

		uint32 Flash_Address;




		
}sys_flags;



typedef struct Devie_1 //系统累计时间结构体
{
		uint16 bus_status;//总线状态
		
		
	 
	//	uint16 read_set_value; //设定值

		uint16 err_code;   //故障代码

		uint8  time_up; //发送时间的间隔

		uint8  Send_success_flag;//请求数据成功

	//	uint16 go_set_value; //程序需要设备的开度值
		uint32 Unresponse_Time;//记录设备响应时间，超过5秒，报警
 	 
 	  
}FLOW_CONTROL_DEVICE;

typedef struct _water_logic_f // 
{
	   uint8  Zstate_Flag;	//0 为关闭状态，1位开启状态 
	   uint8  Cstate_Flag;

	   uint8  ZSignal;
	   uint8  CSignal;	 //0 为关闭请求信号，1为开启请求信号

	   uint8  ZCommand; //0 为关闭指令， 1位开启指令
	   uint8  CCommand;

	   uint8  Pump_Signal;	//0 为关闭信号，1位开启信号 

		uint16 ZC_Open_Time; //电磁阀开启的时间
		uint16 PUMP_Close_Time;//水泵开启的时间
	  
}Logic_Water;



extern uint32	sys_control_time  ;  //定时闹铃时间
extern uint8	   sys_time_up	    ;   //闹铃标志
extern uint8	   sys_time_start  ;	//启动控制时间标志 0 = not ,1 = yes


extern sys_flags sys_flag;//系统需要使用的标志量集合

extern UART_DATA U1_Inf; //定义了串口通信所用到的变量
extern UART_DATA U2_Inf;
extern UART_DATA U3_Inf;
extern UART_DATA U4_Inf;

extern UART_DATA U5_Inf;


extern uint8 target_percent ; //用于设定目标风机风速
extern uint8 now_percent ; //用于设定现在的实时风速

extern uint8 T_PERCENT;


extern  uint32_t BJ_TimeVar;//定义时间戳变量

extern struct rtc_time systmtime;  //时间结构体变量




extern BYTE_WORD4 Secret_uint;
extern BYTE_WORD1 Data_wordtobyte;
extern FLP_INT  Float_Int;//用于单精度浮点型数据的转化
extern BYTE_INT32 Byte_To_Duint32;  //用于4个字节到uint32的数据转换



extern IO_DATA IO_Status;
extern SYS_INF sys_data;
extern SYS_COPY copy_sys;

extern LCD_MEM lcd_data;

extern Logic_Water Water_State;




extern Lcd_Read_Data read_lcd_data;//用于记录人工设置的系统参数
extern SYS_WORK_TIME sys_time_inf;//系统累计运行时间变量
extern SYS_WORK_TIME Start_End_Time; //用于记录本次启停的运行时间间隔，必须得烧起来

extern SYS_WORK_TIME big_time_inf;//锅炉小风量运行时间
extern SYS_WORK_TIME small_time_inf;//锅炉大风量运行时间

extern SYS_CONFIG sys_config_data;//用于系统设置大小火延时等参数

extern SYS_ADMIN  Sys_Admin; //用于设置管理员参数

extern LCD_QuXian lcd_quxian_data;//用于刷新数据统计的曲线
extern ERR_LCD  Err_Lcd_Code;//用于刷新lcd报警代码
extern LCD_E_M  Err_Lcd_Memory[8];//用于记录8个故障信息（时间和故障原因）
extern LCD_FLASH_STRUCT  Lcd_FlashD; //用于将故障信息保存在DUGS屏上


extern ERROR_DATE_STRUCT SPI_Error_Data;


extern AB_EVENTS  Abnormal_Events;//用于系统运行时的异常记录
extern Login_TT Login_D;




extern JUMP_TYPE  Jump_Step;//用于除垢跳转变量




extern	uint32  sys_control_time;
extern	uint8		sys_time_up;
extern	uint8		sys_time_start;


extern  uint8  IDLE_INDEX ;
extern uint16 Err_Value;






extern struct ERROR_INF ERR_DATA1;
extern uint16 Man_Set_Pressure;




extern uint8 Self_Index ;
extern uint8 Sys_Staus ;
extern uint8	Sys_Launch_Index ;
extern uint8 Ignition_Index ;
extern uint8	Pressure_Index ;
extern uint8 IDLE_INDEX ;



 
void Get_IO_Inf(void); 
void	Self_Check_Function(void);//系统自检程序
void  System_All_Control(void);//系统状态切换程序
void  Abnormal_Events_Response(void);//异常状态处理程序
void Abnormal_Check_Fun(void);//异常状态中的IO检测
uint8 Idel_Check_Fun(void);

void  Err_Response(void);
void	Sys_Launch_Function(void);//系统运行程序


void  Ignition_Check_Fun(void);//点火过程中，保持监测水位及其它状态
uint8  Sys_Ignition_Fun(void);  //系统点火程序
void  Auto_Check_Fun(void);  //用于系统运行时，检查系统状态
void 	System_Idel_Function(void);//用于处理系统待机程序
//void printf_Sys_Status(void); //5秒打印一次系统信息

uint8   sys_work_time_function(void);//累计系统运行时间

void copy_to_lcd(void);//同步LCD设置参数与系统参数
void sys_control_config_function(void);//系统默认参数配置并送往LCD
void IDLE_Err_Response(void);
uint8 byte_to_bit(void);

//刷新LCD故障信息记录表格
void Lcd_Err_Refresh(void);
void Lcd_Err_Read(void);

void clear_struct_memory(void);  //开机时所有结构体清零
void Load_LCD_Data(void);//加载LCD发给MCU的数据




void One_Sec_Check(void);




uint8 sys_start_cmd(void);//系统开启命令处理
void sys_close_cmd(void);// 系统关闭命令处理

void Last_Blow_Start_Fun(void);//后吹扫开始和结束程序
void Last_Blow_End_Fun(void);



/*点火前的准备工作*/
 uint8 Before_Ignition_Prepare(void);

/*对蒸汽添加自动水平衡程序*/
 uint8  Water_Balance_Function(void);


 uint8 System_Pressure_Balance_Function(void);



//用于异常模式下，循环泵工作的处理
uint8 Abnormal_InsideLoop_Fun (void);


uint8 Manual_Realys_Function(void);

//用于检查锅炉运行关键参数，前后吹扫，点火功率等
void Check_Config_Data_Function(void);

void Fan_Speed_Check_Function(void);

uint8 Admin_Work_Time_Function(void);



void HardWare_Protect_Relays_Function(void);

uint8 Power_ON_Begin_Check_Function(void);

uint8 IDLE_Auto_Pai_Wu_Function(void);

uint8 YunXingZhong_TimeAdjustable_PaiWu_Function(void);

uint8 PaiWu_Warnning_Function(void);

uint8 Special_Water_Supply_Function(void);

uint8 SafetyProtect_Tx_Function(void);

uint8 WaterLevel_Unchange_Check(void);

void JTAG_Diable(void);

uint8  Water_BianPin_Function(void);

uint8 LianXu_Paiwu_Control_Function(void);

uint8 Auto_StartOrClose_Process_Function(void);

uint8 Speed_Pressure_Function(void);

uint8 Auto_Pai_Wu_Function(void);

uint8 Wifi_Lock_Time_Function(void);

uint8 XiangBian_Steam_AddFunction(void);
uint8 GetOut_Mannual_Function(void);

uint8  ShuangPin_Water_Balance_Function(void);

uint8 Relays_NoInterrupt_ON_OFF(void);


uint8 Double_WaterPump_LogicFunction(void);
uint8  Double_Water_BianPin_Function(void);



#endif


