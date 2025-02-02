#include "stc15.h"
#include "onewire.h"

void Delay_DS(unsigned int n)
{
	while(n--);
}

//Temp >=0
unsigned long DS18B20_GetTemp()
{
	unsigned long m;
	unsigned char LSB,MSB;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	Delay_DS(700);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	init_ds18b20();
	m = MSB;
	m = (m << 8)|LSB;
	m = m*625;
	return m;
}