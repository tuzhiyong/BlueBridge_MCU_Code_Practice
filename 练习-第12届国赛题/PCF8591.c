#include "stc15.h"
#include "iic.h"

void PCF8591_PUTVoltage(unsigned char m,unsigned char voltage)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40|m);
	I2CWaitAck();
	I2CSendByte(voltage);
	I2CWaitAck();
	I2CStop();
}

unsigned char PCF8591_GETVoltage(unsigned char m)
{
	unsigned char v;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40|m);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	v = I2CReceiveByte();
	I2CWaitAck();
	I2CSendAck(1);
	I2CStop();
	return v;
}