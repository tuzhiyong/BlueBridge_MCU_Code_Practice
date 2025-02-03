#include "stc15.h"
#include "iic.h"

void PCF8591_PutVoltage(unsigned char ain,unsigned char v)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40|ain);
	I2CWaitAck();
	I2CSendByte(v);
	I2CWaitAck();
	I2CStop();
}
