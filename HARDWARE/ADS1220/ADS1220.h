#ifndef ADS1220_H_
#define ADS1220_H_

#include "stm32f10x.h"


#define ADS1220_ENABLE    1
#define ADS1220_DISABLE   0

#define CPOL    0
#define CPHA    1

typedef enum
{
	ADS1220_CS,
	ADS1220_SCLK,
	ADS1220_DIN,
	ADS1220_DOUT,
	ADS1220_DRDY
}ADS1220_GPIO;

typedef enum
{
	GAIN_1 = 0X00,
	GAIN_2 = 0X02,
	GAIN_4 = 0X04,
	GAIN_8 = 0X06,
	GAIN_16 = 0X08,
	GAIN_32 = 0X0A,
  GAIN_64 = 0X0C,
	GAIN_128 =0X0E
	
}ADS1220_Gain;

typedef struct 
{
   GPIO_TypeDef *port[5];
   uint16_t pin[5];	
}ADS1220_InitType;

extern ADS1220_InitType ADS1220_Init;

/* Error Return Values */
#define ADS1220_NO_ERROR           0
#define ADS1220_ERROR				-1
/* Command Definitions */
#define ADS1220_CMD_RDATA    	0x10
#define ADS1220_CMD_RREG     	0x20
#define ADS1220_CMD_WREG     	0x40
#define ADS1220_CMD_SYNC    	0x08
#define ADS1220_CMD_SHUTDOWN    0x02
#define ADS1220_CMD_RESET    	0x06
/* ADS1220 Register Definitions */
#define ADS1220_0_REGISTER   	0x00
#define ADS1220_1_REGISTER     	0x01
#define ADS1220_2_REGISTER     	0x02
#define ADS1220_3_REGISTER    	0x03
/* ADS1220 Register 0 Definition */
/*   Bit 7   |   Bit 6   |   Bit 5   |   Bit 4   |   Bit 3   |   Bit 2   |   Bit 1   |   Bit 0 
//--------------------------------------------------------------------------------------------
//                     MUX [3:0]                 |             GAIN[2:0]             | PGA_BYPASS
*/
/* Define MUX */
#define ADS1220_MUX_0_1   	0x00
#define ADS1220_MUX_0_2   	0x10
#define ADS1220_MUX_0_3   	0x20
#define ADS1220_MUX_1_2   	0x30
#define ADS1220_MUX_1_3   	0x40
#define ADS1220_MUX_2_3   	0x50
#define ADS1220_MUX_1_0   	0x60
#define ADS1220_MUX_3_2   	0x70
#define ADS1220_MUX_0_G		0x80
#define ADS1220_MUX_1_G   	0x90
#define ADS1220_MUX_2_G   	0xa0
#define ADS1220_MUX_3_G   	0xb0
#define ADS1220_MUX_EX_VREF 0xc0
#define ADS1220_MUX_AVDD   	0xd0
#define ADS1220_MUX_DIV2   	0xe0
/* Define GAIN */
#define ADS1220_GAIN_1      0x00
#define ADS1220_GAIN_2      0x02
#define ADS1220_GAIN_4      0x04
#define ADS1220_GAIN_8      0x06
#define ADS1220_GAIN_16     0x08
#define ADS1220_GAIN_32     0x0a
#define ADS1220_GAIN_64     0x0c
#define ADS1220_GAIN_128    0x0e
/* Define PGA_BYPASS */
#define ADS1220_PGA_BYPASS 	0x01
/* ADS1220 Register 1 Definition */
/*   Bit 7   |   Bit 6   |   Bit 5   |   Bit 4   |   Bit 3   |   Bit 2   |   Bit 1   |   Bit 0 
//--------------------------------------------------------------------------------------------
//                DR[2:0]            |      MODE[1:0]        |     CM    |     TS    |    BCS
*/
/* Define DR (data rate) */
#define ADS1220_DR_20		0x00
#define ADS1220_DR_45		0x20
#define ADS1220_DR_90		0x40
#define ADS1220_DR_175		0x60
#define ADS1220_DR_330		0x80
#define ADS1220_DR_600		0xa0
#define ADS1220_DR_1000		0xc0
/* Define MODE of Operation */
#define ADS1220_MODE_NORMAL 0x00
#define ADS1220_MODE_DUTY	0x08
#define ADS1220_MODE_TURBO 	0x10
#define ADS1220_MODE_DCT	0x18
/* Define CM (conversion mode) */
#define ADS1220_CC			0x04
/* Define TS (temperature sensor) */
#define ADS1220_TEMP_SENSOR	0x02
/* Define BCS (burnout current source) */
#define ADS1220_BCS			0x01
/* ADS1220 Register 2 Definition */
/*   Bit 7   |   Bit 6   |   Bit 5   |   Bit 4   |   Bit 3   |   Bit 2   |   Bit 1   |   Bit 0 
//--------------------------------------------------------------------------------------------
//         VREF[1:0]     |        50/60[1:0]     |    PSW    |             IDAC[2:0]
*/
/* Define VREF */
#define ADS1220_VREF_INT	0x00
#define ADS1220_VREF_EX_DED	0x40
#define ADS1220_VREF_EX_AIN	0x80
#define ADS1220_VREF_SUPPLY	0xc0
/* Define 50/60 (filter response) */
#define ADS1220_REJECT_OFF	0x00
#define ADS1220_REJECT_BOTH	0x10
#define ADS1220_REJECT_50	0x20
#define ADS1220_REJECT_60	0x30

/* Define PSW (low side power switch) */
#define ADS1220_PSW_SW		0x08
/* Define IDAC (IDAC current) */
#define ADS1220_IDAC_OFF	0x00
#define ADS1220_IDAC_10		0x01
#define ADS1220_IDAC_50		0x02
#define ADS1220_IDAC_100	0x03
#define ADS1220_IDAC_250	0x04
#define ADS1220_IDAC_500	0x05
#define ADS1220_IDAC_1000	0x06
#define ADS1220_IDAC_2000	0x07
/* ADS1220 Register 3 Definition */
/*   Bit 7   |   Bit 6   |   Bit 5   |   Bit 4   |   Bit 3   |   Bit 2   |   Bit 1   |   Bit 0 
//--------------------------------------------------------------------------------------------
//               I1MUX[2:0]          |               I2MUX[2:0]          |   DRDYM   | RESERVED
*/
/* Define I1MUX (current routing) */
#define ADS1220_IDAC1_OFF	0x00
#define ADS1220_IDAC1_AIN0	0x20
#define ADS1220_IDAC1_AIN1	0x40
#define ADS1220_IDAC1_AIN2	0x60
#define ADS1220_IDAC1_AIN3	0x80
#define ADS1220_IDAC1_REFP0	0xa0
#define ADS1220_IDAC1_REFN0	0xc0
/* Define I2MUX (current routing) */
#define ADS1220_IDAC2_OFF	0x00
#define ADS1220_IDAC2_AIN0	0x04
#define ADS1220_IDAC2_AIN1	0x08
#define ADS1220_IDAC2_AIN2	0x0c
#define ADS1220_IDAC2_AIN3	0x10
#define ADS1220_IDAC2_REFP0	0x14
#define ADS1220_IDAC2_REFN0	0x18
/* define DRDYM (DOUT/DRDY behaviour) */
#define ADS1220_DRDY_MODE	0x02
/* Low Level ADS1220 Device Functions */

void ADS1220Init(void);							/* Device initialization */
int ADS1220WaitForDataReady(int Timeout);		/* DRDY polling */
void ADS1220CsStatus(uint8_t state);				/* Assert/deassert CS */
void ADS1220SendByte(unsigned char cData );		/* Send byte to the ADS1220 */
unsigned char ADS1220ReceiveByte(void);			/* Receive byte from the ADS1220 */
/* ADS1220 Higher Level Functions */
long ADS1220ReadData(void);						/* Read the data results */
void ADS1220ReadRegister(int StartAddress, int NumRegs, unsigned * pData);	/* Read the register(s) */
void ADS1220WriteRegister(int StartAddress, int NumRegs, unsigned * pData); /* Write the register(s) */
void ADS1220SendResetCommand(void);				/* Send a device Reset Command */
void ADS1220SendStartCommand(void);				/* Send a Start/SYNC command */
void ADS1220SendShutdownCommand(void);			/* Place the device in powerdown mode */
/* Register Set Value Commands */
void ADS1220Config(void);
int ADS1220SetChannel(int Mux);
void ADS1220SetGain(ADS1220_Gain Gain);
int ADS1220SetPGABypass(int Bypass);
int ADS1220SetDataRate(int DataRate);
int ADS1220SetClockMode(int ClockMode);
int ADS1220SetPowerDown(int PowerDown);
int ADS1220SetTemperatureMode(int TempMode);
int ADS1220SetBurnOutSource(int BurnOut);
int ADS1220SetVoltageReference(int VoltageRef);
int ADS1220Set50_60Rejection(int Rejection);
int ADS1220SetLowSidePowerSwitch(int PowerSwitch);
int ADS1220SetCurrentDACOutput(int CurrentOutput);
int ADS1220SetIDACRouting(int IDACRoute);
int ADS1220SetDRDYMode(int DRDYMode);
/* Register Get Value Commands */
int ADS1220GetChannel(void);
int ADS1220GetGain(void);
int ADS1220GetPGABypass(void);
int ADS1220GetDataRate(void);
int ADS1220GetClockMode(void);
int ADS1220GetPowerDown(void);
int ADS1220GetTemperatureMode(void);
int ADS1220GetBurnOutSource(void);
int ADS1220GetVoltageReference(void);
int ADS1220Get50_60Rejection(void);
int ADS1220GetLowSidePowerSwitch(void);
int ADS1220GetCurrentDACOutput(void);
int ADS1220GetIDACRouting(int WhichOne);
int ADS1220GetDRDYMode(void);

long ADS1220ReadData(void);


#endif /*ADS1220_H_*/
