#ifndef __ACTIVATE_KEY_H
#define	__ACTIVATE_KEY_H

#include "main.h"






#define PRODUCT_ZERO_NUMBER  (100)  //该产品系列0--100为测试ID，从第101开始生产

#define PRODUCT_BEGIN_NUMBER  (190110001)  //201，301表示此次产品生产开始序列

#define PRODUCT_END_NUMBER  (190111000)  //300，400表示此次产品生产结束序列
//限定生产批次，每次50台，上限65500台

#define PRODUCT_DEFINE  (10003)



#define  ACTIVATE_DATA  0xA6A6

#define  TEST_KEY_COUNT_ADDRESS  (0x08000000+114*1024)  //测试码输入次数保存地址


#define  CPUID_ENCRYPT_ADDRESS  (0x08000000+116*1024)

#define  LOGIN_KEY_HEAD_ADDRESS  (0x08000000+126*1024) //将数据保存在第121K开始的地方

//#define  KEY_ONE_ADDRESS   (LOGIN_KEY_HEAD_ADDRESS)  //烧机密码前 2byte
//#define  KEY_TWO_ADDRESS   (LOGIN_KEY_HEAD_ADDRESS+4)
//#define  KEY_THREE_ADDRESS   (LOGIN_KEY_HEAD_ADDRESS+8)
//#define  KEY_FOUR_ADDRESS   (LOGIN_KEY_HEAD_ADDRESS+12)//烧机密码最后 2byte

//#define  CHECK_KEY_ADDRESS (LOGIN_KEY_HEAD_ADDRESS+16)   //用于存储是否得到出厂码的信息

#define  LOGIN_OK_ADDRESS (LOGIN_KEY_HEAD_ADDRESS+20)  //当激活成功后，向该区域写入一个值



typedef struct p_data{
	uint16	product_zero_number ;
	uint16  product_begin_number;
	uint16  product_end_number ;
	}Data_Product;



typedef struct _KEY_STD
{
		uint16  key1_buff;
		uint16  key2_buff;
		uint16  key3_buff;
		uint16  key4_buff;

}Key_Struct;//用于储存用户输入的激活码


extern volatile  uint32 CPUIDEncrypt;


extern Data_Product Product_Inf ;//用于生产时，标明本机序列号使用的信息结构体



extern Key_Struct login_key; 





void  Write_First_Flash(void);

void send_product_information(void);
void send_product_name(void);
void send_system_version(void);




void Write_CPU_ID_Encrypt(void);//CPU ID加密程序


uint8 Judge_ID_Encrypt(void);//判断ID加密是否正确

void Flash_Read_Protect(void);//内部Flash读保护开启

void Flash_Read_Disable(void);//内部flash读保护解除
void  Write_Test_Key_Flash(uint8 data);//用于记录测试码已经使用的次数


#endif



