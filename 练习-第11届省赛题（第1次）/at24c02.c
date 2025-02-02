#include "stc15.h"
#include "iic.h"

void AT24C02_Wirte(unsigned char m,unsigned char n)
{
	I2CStart();
	I2CSendByte(0xA0); //1010 000 0
	I2CWaitAck();
	I2CSendByte(m);
	I2CWaitAck();
	I2CSendByte(n);
	I2CWaitAck();
	I2CStop();
}

unsigned char AT24C02_Read(unsigned char m)
{
	unsigned char n;
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(m);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	n = I2CReceiveByte();
	I2CWaitAck();
	I2CSendAck(1);
	I2CStop();
	return n;
}
