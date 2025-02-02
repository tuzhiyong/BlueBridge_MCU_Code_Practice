#include "stc15.h"
#include "iic.h"

unsigned char PCF8591_GetData()
{
	unsigned char m;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	m = I2CReceiveByte();
	I2CWaitAck();
	I2CSendAck(1);
	I2CStop();
	return m;
}

void PCF8591_PutData(unsigned char m,unsigned char n)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40|m);
	I2CWaitAck();
	I2CSendByte(n);
	I2CWaitAck();
	I2CStop();
}