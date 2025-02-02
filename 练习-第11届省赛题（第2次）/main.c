#include "stc15.h"
#include "onewire.h"
#include "PCF8591.h"

code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};


unsigned char tmax = 30,tmin = 20;
unsigned long ds18b20_temp = 24;
unsigned char tmax_t = 30,tmin_t = 20;
sbit S7 = P3^0;
sbit S6 = P3^1;
sbit S5 = P3^2;
sbit S4 = P3^3;

bit jm = 0;
bit cs = 0;


//LED
unsigned char LED = 0xff;
unsigned char LED_Temp = 0xff;

bit L1 = 1;
bit L2 = 1;
bit L3 = 1;
bit L4 = 1;

void Delay(unsigned int n)
{
	while(n --);
}

void SelectHC573(unsigned char n)
{
	switch(n)
	{
		case 4:
			P2 = (P2 & 0x1f)|0x80;
		break;
		case 5:
			P2 = (P2 & 0x1f)|0xa0;
		break;
		case 6:
			P2 = (P2 & 0x1f)|0xc0;
		break;
		case 7:
			P2 = (P2 & 0x1f)|0xe0;
		break;
	}
}

void smgDisplay(unsigned char m,unsigned char n)
{
	SelectHC573(6);
	P0 = 0x01 << m;
	SelectHC573(7);
	P0 = n;
}

void Display_P()
{
	P0 = 0xff;
	smgDisplay(0,0x8c);//1000 1100
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(1,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(2,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(3,Seg_Table[tmax_t/10%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(4,Seg_Table[tmax_t%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(5,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(6,Seg_Table[tmin_t/10%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[tmin_t%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
	Delay(500);
}

void Display_C()
{
	P0 = 0xff;
	smgDisplay(0,Seg_Table[12]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(1,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(2,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(3,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(4,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(5,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(6,Seg_Table[ds18b20_temp/10%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[ds18b20_temp%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
	Delay(500);
}

void InitSystem()
{
	SelectHC573(4);
	P0 = 0xff;
	SelectHC573(5);
	P0 = 0x00;
	P2 = 0x00;
}

unsigned long DS18B20_Read()
{
	unsigned char LSB,MSB;
	unsigned long t = 0,temp;
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	Delay(700);
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	init_ds18b20();
	t = MSB;
	t = (t << 8)|LSB;
	t *= 0.0625;
	temp = t;
	return temp;
}

void Key()
{
	if(S7 == 0 &&jm == 1)
	{
		Delay(500);
		if(S7 == 0)
		{
			while(S7 == 0);
			if(cs == 0 && jm == 1)
			{
				if(tmax_t > 0)
				{
					tmax_t --;
				}
			}
			else
			{
				if(tmin_t > 0)
				{
					tmin_t --;
				}
			}
		}
	}
	
	if(S6 == 0 &&jm == 1)
	{
		Delay(500);
		if(S6 == 0)
		{
			while(S6 == 0);
			if(cs == 0 && jm == 1)
			{
				if(tmax_t < 99)
				{
					tmax_t ++;
				}
			}
			else
			{
				if(tmin_t < 99)
				{
					tmin_t ++;
				}
			}
		}
	}
	
	if(S5 == 0 &&jm == 1)
	{
		Delay(500);
		if(S5 == 0)
		{
			while(S5 == 0);
			if(cs == 0&&jm == 1)
			{
				cs = 1;
			}
			else
			{
				cs = 0;
			}
		}
	}
	
	if(S4 == 0)
	{
		Delay(500);
		if(S4 == 0)
		{
			while(S4 == 0);
			if(jm == 0)
			{
				jm = 1;
				tmax_t = tmax;
				tmin_t = tmin;
			}
			else
			{
				jm = 0;
				if(tmax_t >=tmin_t)
				{
					tmax = tmax_t;
					tmin = tmin_t;
					L4 = 1;
				}
				else
				{
					L4 = 0;
				}
			}
		}
	}
}

void LED_Fun()
{
	unsigned char m;
	if(L1 == 0)
	{
		LED = LED & 0xfe;
	}
	else
	{
		LED = LED | 0x01;
	}
	
	if(L2 == 0)
	{
		LED = LED & 0xfd;
	}
	else
	{
		LED = LED | 0x02;
	}
	
	if(L3 == 0)
	{
		LED = LED & 0xfb;
	}
	else
	{
		LED = LED | 0x04;
	}
	
	if(L4 == 0)
	{
		LED = LED & 0xf7;
	}
	else
	{
		LED = LED | 0x08;
	}
	
	if(LED!=LED_Temp)
	{
		LED_Temp = LED;
		m = P2;
		SelectHC573(4);
		P0 = LED_Temp;
		P2 = m;
	}
}

void main()
{
	InitSystem();
	while(1)
	{
		if(jm == 0)
		{
			Display_C();
		}
		else
		{
			Display_P();
		}
		Key();
		ds18b20_temp = DS18B20_Read();
		if(ds18b20_temp >tmax)
		{
			PCF8591_DAOut(204);
			L1 = 0;
			L2 = 1;
			L3 = 1;
		}
		else if(ds18b20_temp <= tmax && ds18b20_temp >= tmin)
		{
			PCF8591_DAOut(153);
			L1 = 1;
			L2 = 0;
			L3 = 1;
		}
		else if(ds18b20_temp < tmin)
		{
			PCF8591_DAOut(102);
			L1 = 1;
			L2 = 1;
			L3 = 0;
		}
		LED_Fun();
	}
}