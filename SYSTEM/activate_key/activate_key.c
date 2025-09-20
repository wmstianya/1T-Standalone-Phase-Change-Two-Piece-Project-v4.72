

#include"main.h"


 Key_Struct login_key;//用于储存用户输入的激活码
	
//Data_Product Product_Inf ;//用于生产时，标明本机序列号使用的信息结构体



volatile  uint32 CPUIDEncrypt = 0xFFFFFFFF;//ID加密



//产品型号

const unsigned char product_name[15] ={"PM-S8-1.0ZQ"};

//系统版本号
const unsigned char system_version[10] ={"PMS8_3.3.1"};  /*2021年8月31日10:40:27*/








//测试码：1234,4321,5678,8765，仅用于生产调试使用，不具备WIFI，不具备调温，只能运行到稳火，稳火结束，熄火
const unsigned char Test_key[8] = {12, 34, 43, 21, 56, 78, 87, 65,};










void send_product_information(void)
{
	
	
	SysTick_Delay_ms(1);
	 send_product_name();
	 SysTick_Delay_ms(1);
	 send_system_version();
	 SysTick_Delay_ms(1);
	
		
}

//发送产品型号信息到LCD上，在帮助界面显示
void send_product_name(void)
{
	uint8 temp = 0;
	
		send_byte(0x5A);
		send_byte(0XA5);
		send_byte(3+2*sizeof(product_name));//字符长度
		send_byte(0X82);//写命令码
		send_byte(0X03);
		send_byte(0X90);//起始地址

		for(temp = 0; temp < sizeof(product_name);temp++ )
			{
				send_byte(0X00);
				send_byte(product_name[temp]);
			}
		
}

void send_system_version(void)
{
	uint8 temp = 0;
		
		send_byte(0x5A);
		send_byte(0XA5);
		send_byte(3+2*sizeof(system_version));//字符长度
		send_byte(0X82);//写命令码
		send_byte(0X03);
		send_byte(0X60);//起始地址

		for(temp = 0; temp < sizeof(system_version);temp++ )
			{
				send_byte(0X00);
				send_byte(system_version[temp]);
			}

		
}






void  Write_First_Flash(void)
{
	FLASH_Unlock();  //内部FLASH解锁
	FLASH_ErasePage(LOGIN_KEY_HEAD_ADDRESS);
	FLASH_ProgramWord(LOGIN_OK_ADDRESS,ACTIVATE_DATA);//防止第二次写入
	FLASH_Lock();
}


void  Write_Test_Key_Flash(uint8 data)
{
	FLASH_Unlock();  //内部FLASH解锁
	FLASH_ErasePage(TEST_KEY_COUNT_ADDRESS);
	FLASH_ProgramWord(TEST_KEY_COUNT_ADDRESS,data);//防止第二次写入
	FLASH_Lock();
}




void Write_CPUID_Flash(uint32 data)
{
	FLASH_Unlock();  //内部FLASH解锁
	FLASH_ErasePage(CPUID_ENCRYPT_ADDRESS);
	FLASH_ProgramWord(CPUID_ENCRYPT_ADDRESS,data);//防止第二次写入
	FLASH_Lock();
}


void Write_CPU_ID_Encrypt(void)
{
	 
	uint32 CPUID[3];
	uint32 EncryptCode = 0;
	
	CPUIDEncrypt  = *(uint32 *)(CPUID_ENCRYPT_ADDRESS);
	if(CPUIDEncrypt ==0xFFFFFFFF)
		{
			CPUID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高地址
    		CPUID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
    		CPUID[2] = *(__IO u32 *)(0X1FFFF7E8); // 低地址
    		EncryptCode = (CPUID[0]>>3)+(CPUID[1]>>1)+(CPUID[2]>>2);
			Write_CPUID_Flash(EncryptCode);

			if(*(uint32 *)(CPUID_ENCRYPT_ADDRESS) == EncryptCode)
				printf("\r\nCPU ID 加密成功！！！");
		}
}


uint8 Judge_ID_Encrypt(void)
{
	uint32 CPUID[4];

	CPUID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高地址
    CPUID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
    CPUID[2] = *(__IO u32 *)(0X1FFFF7E8); // 低地址

	CPUID[3]= (CPUID[0]>>3)+(CPUID[1]>>1)+(CPUID[2]>>2);
	CPUIDEncrypt  = *(uint32 *)(CPUID_ENCRYPT_ADDRESS);

	return(CPUIDEncrypt == CPUID[3]);
	
}



void Flash_Read_Protect(void)
{
	 if(FLASH_GetReadOutProtectionStatus()!=SET)
    { 
        FLASH_Unlock(); //不解锁FALSH也可设置读保护 
        FLASH_ReadOutProtection(ENABLE);
        FLASH_Lock();//上锁
    } 
}


void Flash_Read_Disable(void)
{
	if(FLASH_GetReadOutProtectionStatus()!=RESET)
    { 
     FLASH_Unlock(); 
     FLASH_ReadOutProtection(DISABLE);
     FLASH_Lock(); 
    }
}





