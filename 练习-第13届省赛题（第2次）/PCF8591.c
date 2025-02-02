#include "stc15.h"
#include "iic.h"

unsigned char PCF8591_GETAD(unsigned char AD_Value)
{
	unsigned char v;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40|AD_Value);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	v = I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return v;
}

void PCF8591_PUT(unsigned char DA_Value)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(DA_Value);
	I2CWaitAck();
	I2CStop();
}