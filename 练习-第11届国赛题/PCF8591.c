#include "stc15.h"
#include "iic.h"

unsigned char PCF8591_Get(unsigned char add)
{
	unsigned char v;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40|add);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	v = I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return v;
}

void PCF8591_Put(unsigned char add,unsigned char n)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40|add);
	I2CWaitAck();
	I2CSendByte(n);
	I2CWaitAck();
	I2CStop();
}