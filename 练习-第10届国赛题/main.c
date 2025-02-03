#include "stc15.h"
#include "onewire.h"
#include "PCF8591.h"
#include "AT24C02.h"
#include <intrins.h>
#include <string.h>
code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};
code unsigned char ST[] = "ST\r\n";
code unsigned char PARA[] = "PARA\r\n";
unsigned char count1 = 0;
unsigned char count2 = 0;
sbit Tx = P1^0;
sbit Rx = P1^1;
sbit row1 = P3^0;
sbit row2 = P3^1;
sbit row3 = P3^2;
sbit row4 = P3^3;
sbit col1 = P4^4;
sbit col2 = P4^2;
sbit col3 = P3^5;
sbit col4 = P3^4;
unsigned int temper = 2632;
unsigned char distance = 50;
unsigned int temperConfig = 30;
unsigned int distanceConfig = 35;
unsigned int temperConfig_temp = 30;
unsigned int distanceConfig_temp = 35;
unsigned int editTime = 0;
unsigned char s12 = 0;
unsigned char t0_count = 0;
unsigned char LED = 0xff,LED_Temp = 0xff;
unsigned int time = 0;
unsigned char urdat;
bit L1 = 1;
bit L2 = 1;
bit L3 = 1;
bit s13 = 0;
bit s12_press = 0;
bit s13_press = 0;
bit s13_v = 0;

void SendString(unsigned char *str);
void SendByte(unsigned char dat);



void Delay(unsigned int n)
{
	while(n --);
}

void InitUart()
{
	TMOD = 0x21;
	TH1 = 0xFA;
	TL1 = 0xFA;
	TR1 = 1;
	
	SCON = 0x50;
	AUXR = 0x00;
	ES = 1;
	EA = 1;
}

void Uart() interrupt 4
{
	unsigned char buffer1[12] = {0};
	unsigned char buffer2[11] = {0};
	if(RI == 1)
	{
		RI = 0;
		urdat = SBUF;
		if(urdat == ST[count1])
		{
			count1++;
		}
		else
		{
			count1 = 0;
		}
		if(count1 == 4)
		{
			count1 = 0;
			SendByte('$');
			SendByte(distance/10+'0');
			SendByte(distance%10+'0');
			SendByte(',');
			SendByte(temper/100/10+'0');
			SendByte(temper/100%10+'0');
			SendByte('.');
			SendByte(temper%100/10+'0');
			SendByte(temper%100%10+'0');
			SendByte('\r');
			SendByte('\n');
		}
		if(urdat == ST[count2])
		{
			count2++;
		}
		else
		{
			count2 = 0;
		}
		if(count2 == 6)
		{
			count2 = 0;
			SendByte('$');
			SendByte(distanceConfig/10+'0');
			SendByte(distanceConfig%10+'0');
			SendByte(',');
			SendByte(temperConfig/10+'0');
			SendByte(temperConfig%10+'0');
			SendByte('\r');
			SendByte('\n');
		}
	}
}

void SendByte(unsigned char dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}

void SendString(unsigned char *str)
{
	while(*str != '\0')
	{
		SendByte(*str++);
	}
}

void SelectHC573(unsigned char n)
{
	switch(n)
	{
		case 4:
			P2 = (P2 & 0x1f)|0x80;
			P2 = (P2 & 0x1f);
		break;
		case 5:
			P2 = (P2 & 0x1f)|0xa0;
			P2 = (P2 & 0x1f);
		break;
		case 6:
			P2 = (P2 & 0x1f)|0xc0;
			P2 = (P2 & 0x1f);
		break;
		case 7:
			P2 = (P2 & 0x1f)|0xe0;
			P2 = (P2 & 0x1f);
		break;
	}
}

void InitTimer0()
{
	TMOD = 0x21;
	TH0 = (65535 - 50000) / 256;
	TL0 = (65535 - 50000) % 256;
	ET0 = 0;
	TR0 = 1;
	EA = 1;
}

void Timer0() interrupt 1
{
	TH0 = (65535 - 50000) / 256;
	TL0 = (65535 - 50000) % 256;
	t0_count ++;
	if(t0_count >= 20)
	{
		t0_count = 0;
		ET0 = 0;
		if(s12_press == 1)
		{
			editTime = 0;
			Write_AT24C02(0,editTime >> 8);
			Delay(1000);
			Write_AT24C02(1,editTime & 0x00ff);
			Delay(1000);
		}
		else if(s13_press == 1)
		{
			if(s13_v == 0)
			{
				s13_v = 1;
			}
			else
			{
				s13_v = 0;
			}
		}
	}
}

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void InitWave()
{
	unsigned char i;
	for(i = 0;i < 8;i ++)
	{
		Tx = 1;
		Delay12us();
		Tx = 0;
		Delay12us();
	}
}

void Wave()
{
	unsigned int temp;
	TMOD &= 0x0f;
	TH1 = TL1 = 0;
	InitWave();
	TR1 = 1;
	while((TF1 == 0)&&(Rx == 1));
	TR1 = 0;
	if(TF1 == 0)
	{
		temp = TH1;
		temp = (temp << 8)|TL1;
		distance = temp * 0.017;
	}
	else
	{
		distance = 0;
		TF1 = 0;
	}
}

void DS18B20_Read()
{
	unsigned char LSB,MSB;
	unsigned int t;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	init_ds18b20();
	t = MSB;
	t = (t<<8)|LSB;
	temper = t * 6.25;
}

void smgDisplay(unsigned char m,unsigned char n)
{
	P0 = 0x01 << m;
	SelectHC573(6);
	P0 = n;
	SelectHC573(7);
}

void Display_Temper()
{
	smgDisplay(0,Seg_Table[15]);
	Delay(500);
	smgDisplay(1,0xff);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,Seg_Table[temper/1000%10]);
	Delay(500);
	smgDisplay(5,Seg_Table_Dot[temper/100%10]);
	Delay(500);
	smgDisplay(6,Seg_Table[temper/10%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[temper%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_Distance()
{
	smgDisplay(0,0xc7);
	Delay(500);
	smgDisplay(1,0xff);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,Seg_Table[distance/10%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[distance%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_EditTime()
{
	smgDisplay(0,0x40);//0100 0000
	Delay(500);
	smgDisplay(1,0xff);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	if(editTime > 9999)
		smgDisplay(3,Seg_Table[editTime/10000%10]);
	else
		smgDisplay(3,0xff);
	Delay(500);
	if(editTime > 999)
		smgDisplay(4,Seg_Table[editTime/1000%10]);
	else
		smgDisplay(4,0xff);
	Delay(500);
	if(editTime > 99)
		smgDisplay(5,Seg_Table[editTime/100%10]);
	else
		smgDisplay(5,0xff);
	Delay(500);
	if(editTime > 9)
		smgDisplay(6,Seg_Table[editTime/10%10]);
	else
		smgDisplay(6,0xff);
	Delay(500);
	smgDisplay(7,Seg_Table[editTime%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_TemperConfig()
{
	smgDisplay(0,Seg_Table[5]);
	Delay(500);
	smgDisplay(1,0xff);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,Seg_Table[4]);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,Seg_Table[temperConfig_temp/10%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[temperConfig_temp%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_DistanceConfig()
{
	smgDisplay(0,Seg_Table[5]);
	Delay(500);
	smgDisplay(1,0xff);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,Seg_Table[4]);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,Seg_Table[distanceConfig_temp/10%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[distanceConfig_temp%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Key()
{
	col1 = 0;
	col2 = 1;
	col3 = 1;
	col4 = 1;
	if(row3 == 0)//s13
	{
		s13_press = 1;
		ET0 = 1;
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0)
			{
				if(s13 == 0)
				{
					if(s12 == 0)
					{
						Display_Temper();
					}
					else if(s12 == 1)
					{
						Display_Distance();
					}
					else if(s12 == 2)
					{
						Display_EditTime();
					}
				}
				else
				{
					if(s12 == 0)
					{
						Display_TemperConfig();
					}
					else if(s12 == 1)
					{
						Display_DistanceConfig();
					}
				}
			}
			if(s13 == 0  && ET0 == 1)
			{
				s13 = 1;
				s12 = 0;
			}
			else if(ET0 == 1)
			{
				s13 = 0;
				s12 = 0;
			}
		}
	}
	s13_press = 0;
	ET0 = 0;
	t0_count = 0;
	if(row4 == 0)//s12
	{
		s12_press = 1;
		ET0 = 1;
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0)
			{
				if(s13 == 0)
				{
					if(s12 == 0)
					{
						Display_Temper();
					}
					else if(s12 == 1)
					{
						Display_Distance();
					}
					else if(s12 == 2)
					{
						Display_EditTime();
					}
				}
				else
				{
					if(s12 == 0)
					{
						Display_TemperConfig();
					}
					else if(s12 == 1)
					{
						Display_DistanceConfig();
					}
				}
			}
			if(s13 == 0 && ET0 == 1)
			{
				s12 ++;
				if(s12 >= 3)
				{
					s12 = 0;
				}
			}
			else if(ET0 == 1)
			{
				s12 ++;
				if(s12 >= 2)
				{
					s12 = 0;
				}
				if(temperConfig_temp != temperConfig)
				{
					temperConfig= temperConfig_temp;
					editTime ++;
					Write_AT24C02(0,editTime >> 8);
					Delay(1000);
					Write_AT24C02(1,editTime & 0x00ff);
					Delay(1000);
				}
				if(distanceConfig_temp != distanceConfig)
				{
					distanceConfig= distanceConfig_temp;
					editTime ++;
					Write_AT24C02(0,editTime >> 8);
					Delay(1000);
					Write_AT24C02(1,editTime & 0x00ff);
					Delay(1000);
				}
			}
		}
	}
	s12_press = 0;
	ET0 = 0;
	t0_count = 0;
	col1 = 1;
	col2 = 0;
	col3 = 1;
	col4 = 1;
	if(row3 == 0 && s13 == 1)//s17
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0)
			{
				if(s13 == 0)
				{
					if(s12 == 0)
					{
						Display_Temper();
					}
					else if(s12 == 1)
					{
						Display_Distance();
					}
					else if(s12 == 2)
					{
						Display_EditTime();
					}
				}
				else
				{
					if(s12 == 0)
					{
						Display_TemperConfig();
					}
					else if(s12 == 1)
					{
						Display_DistanceConfig();
					}
				}
			}
			if(s12 == 0 && temperConfig_temp < 98)
			{
				temperConfig_temp += 2;
			}
			else if(s12 == 1 && distanceConfig_temp < 95)
			{
				distanceConfig_temp += 5;
			}
		}
	}
	if(row4 == 0  && s13 == 1)//s16
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0)
			{
				if(s13 == 0)
				{
					if(s12 == 0)
					{
						Display_Temper();
					}
					else if(s12 == 1)
					{
						Display_Distance();
					}
					else if(s12 == 2)
					{
						Display_EditTime();
					}
				}
				else
				{
					if(s12 == 0)
					{
						Display_TemperConfig();
					}
					else if(s12 == 1)
					{
						Display_DistanceConfig();
					}
				}
			}
			if(s12 == 0 && temperConfig_temp > 0)
			{
				temperConfig_temp -= 2;
			}
			else if(s12 == 1 && distanceConfig_temp > 0)
			{
				distanceConfig_temp -= 5;
			}
		}
	}
}

void LED_Fun()
{
	if(temper > temperConfig * 100 )
	{
		L1 = 0;
	}
	else
	{
		L1 = 1;
	}
	if(distance > distanceConfig * 100 )
	{
		L2 = 0;
	}
	else
	{
		L2 = 1;
	}
	if(s13_v == 0)
	{
		L3 = 0;
	}
	else
	{
		L3 = 1;
	}
	if(L1 == 0)
	{
		LED_Temp = LED_Temp & 0xfe;
	}
	else
	{
		LED_Temp = LED_Temp | 0x01;
	}
	if(L2 == 0)
	{
		LED_Temp = LED_Temp & 0xfd;
	}
	else
	{
		LED_Temp = LED_Temp | 0x02;
	}
	if(L3 == 0)
	{
		LED_Temp = LED_Temp & 0xfb;
	}
	else
	{
		LED_Temp = LED_Temp | 0x04;
	}
	if(LED_Temp != LED)
	{
		LED = LED_Temp;
		P0 = LED;
		SelectHC573(4);
	}
}

void InitSystem()
{
	P0 = 0xff;
	SelectHC573(4);
	P0 = 0x00;
	SelectHC573(5);
}

void main()
{
	InitSystem();
	InitTimer0();
	InitUart();
	while(1)
	{
		if(s13 == 0)
		{
			if(s12 == 0)
			{
				Display_Temper();
			}
			else if(s12 == 1)
			{
				Display_Distance();
			}
			else if(s12 == 2)
			{
				Display_EditTime();
			}
		}
		else
		{
			if(s12 == 0)
			{
				Display_TemperConfig();
			}
			else if(s12 == 1)
			{
				Display_DistanceConfig();
			}
		}
		DS18B20_Read();
		Key();
		if(distance <= distanceConfig && s13_v == 0)
		{
			PCF8591_PutVoltage(0,102);
		}
		else if(distance > distanceConfig && s13_v == 0)
		{
			PCF8591_PutVoltage(0,204);
		}
		LED_Fun();
		time ++;
		if(time >= 50)
		{
			time = 0;
			//Wave();
			SendByte(0x5a);
			SendByte(0xa5);
		}
	}
}