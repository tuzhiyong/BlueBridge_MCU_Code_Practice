#include "stc15.h"
#include "iic.h"

void PCF8591_DAOut(unsigned char n)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(n);
	I2CWaitAck();
	I2CStop();
}
