#include "stc15.h"
#include "ds1302.h"
#include "onewire.h"
#include "PCF8591.h"
#include "main.h"

code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};

unsigned char DS1302_Read[7] ={0x81,0x83,0x85,0x87,0x89,0x8B,0x8D};
unsigned char DS1302_Write[7] ={0x80,0x82,0x84,0x86,0x88,0x8A,0x8C};
unsigned char time[7] = {0x50,0x59,0x16,0x03,0x01,0x01,0x25};

unsigned long DS18B20_Temp = 332000;
unsigned long V;

unsigned char hour = 17;
unsigned char temper = 25;
unsigned char led_config = 4;
unsigned char LED = 0xff;
unsigned char LED_Temp = 0xff;
unsigned int t = 0;
bit t_state;
bit L3_state = 0;
bit L1 = 1;
bit L2 = 1;
bit L3 = 1;
bit L4 = 1;
bit L5 = 1;
bit L6 = 1;
bit L7 = 1;
bit L8 = 1;

sbit row1 = P3^0;
sbit row2 = P3^1;
sbit row3 = P3^2;
sbit row4 = P3^3;

sbit col1 = P4^4;
sbit col2 = P4^2;
sbit col3 = P3^5;
sbit col4 = P3^4;
bit state = 0;


void InitTimer0()
{
	TMOD = 0x01;
	TH0 = (65535 - 50000) / 256;
	TL0 = (65535 - 50000) % 256;
	ET0 = 1;
	TR0 = 1;
	EA = 1;
}

void Timer0() interrupt 1
{
	TH0 = (65535 - 50000) / 256;
	TL0 = (65535 - 50000) % 256;
	
	t ++;
	if(t>60)
	{
		L3 = 0;
	}
	if(t>100)
	{
		t = 80;
	}
	if(t_state != L3_state)
	{
		t_state = L3_state;
		L3 = 1;
		t = 0;
	}		
}


unsigned char state_s = 0;
void DS18B20_GetTemp(void)
{
	unsigned char LSB,MSB;
	unsigned long t;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	Delay(700);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	t = MSB;
	t = (t << 8)|LSB;
	t = t * 625;
	DS18B20_Temp = t;
}

void DS1302_Config(void)
{
	char i;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i = 0;i < 7;i ++)
	{
		Write_Ds1302_Byte(DS1302_Write[i],time[i]);
	}
	Write_Ds1302_Byte(0x8e,0x80);
}

void Read_DS1302(void)
{
	char i;
	for(i = 0;i < 7;i ++)
	{
		time[i] = Read_Ds1302_Byte(DS1302_Read[i]);
	}
}

void Delay(unsigned int n)
{
	while(n--);
}

void SelectHC573(unsigned char n)
{
	switch(n)
	{
		case 4:
			P2 = (P2 & 0X1F)|0X80;
		  P2 = P2 & 0X1F;
		break;
		case 5:
			P2 = (P2 & 0X1F)|0XA0;
		  P2 = P2 & 0X1F;
		break;
		case 6:
			P2 = (P2 & 0X1F)|0XC0;
		  P2 = P2 & 0X1F;
		break;
		case 7:
			P2 = (P2 & 0X1F)|0XE0;
		  P2 = P2 & 0X1F;
		break;
	}
}

void smgDisplay(unsigned char m ,unsigned char n)
{
	P0 = 0x01 << m;
	SelectHC573(6);
	P0 = n;
	SelectHC573(7);
}

void Display_Time(void)
{
	smgDisplay(0,Seg_Table[time[2]/16]);
	Delay(500);
	smgDisplay(1,Seg_Table[time[2]%16]);
	Delay(500);
	smgDisplay(2,Seg_Table[0]);
	Delay(500);
	smgDisplay(3,Seg_Table[time[1]/16]);
	Delay(500);
	smgDisplay(4,Seg_Table[time[1]%16]);
	Delay(500);
	smgDisplay(5,Seg_Table[0]);
	Delay(500);
	smgDisplay(6,Seg_Table[time[0]/16]);
	Delay(500);
	smgDisplay(7,Seg_Table[time[0]%16]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_Time_Config(void)
{
	smgDisplay(0,0x89);//0100 1101
	Delay(500);
	smgDisplay(1,Seg_Table[4]);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,Seg_Table[hour/10]);
	Delay(500);
	smgDisplay(7,Seg_Table[hour%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_Temp(void)
{
	smgDisplay(0,Seg_Table[15]);
	Delay(500);
	smgDisplay(1,0xff);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,Seg_Table[DS18B20_Temp/100000%10]);
	Delay(500);
	smgDisplay(6,Seg_Table_Dot[DS18B20_Temp/10000%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[DS18B20_Temp/1000%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_Temp_Config(void)
{
	smgDisplay(0,0x89);//0100 1101
	Delay(500);
	smgDisplay(1,Seg_Table[5]);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,Seg_Table[temper/10]);
	Delay(500);
	smgDisplay(7,Seg_Table[temper%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_V(void)
{
	smgDisplay(0,0x89);//1000 1001
	Delay(500);
	smgDisplay(1,0xff);
	Delay(500);
	smgDisplay(2,Seg_Table_Dot[V/100%10]);
	Delay(500);
	smgDisplay(3,Seg_Table[V/10%10]);
	Delay(500);
	smgDisplay(4,Seg_Table[V%10]);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,0xff);
	Delay(500);
	if(V<180)
	{
		smgDisplay(7,Seg_Table[0]);
	}
	else
	{
		smgDisplay(7,Seg_Table[1]);
	}
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_V_Config(void)
{
	smgDisplay(0,0x89);//0100 1101
	Delay(500);
	smgDisplay(1,Seg_Table[6]);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,0xff);
	Delay(500);
	smgDisplay(7,Seg_Table[led_config%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void InitSystem(void)
{
	P0 = 0xff;
	SelectHC573(4);
	P0 = 0x00;
	SelectHC573(5);
}

void Key()
{
	col1 = 0;
	col2 = 1;
	
	if(row3 == 0)//s5
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0);
			state_s++;
			if(state_s >= 3)
			{
				state_s = 0;
			}
		}
	}
	if(row4 == 0)//s4
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0);
			if(state == 0)
			{
				state = 1;
				state_s = 0;
			}
			else
			{
				state = 0;
				state_s = 0;
			}
		}
	}
	
	col1 = 1;
	col2 = 0;
	
	if(row3 == 0)//s9
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0);
			if(state_s == 0)
			{
				if(hour < 23)
				{
					hour++;
				}
			}
			else if(state_s == 1)
			{
				if(temper < 99)
				{
					temper ++;
				}
			}
			else if(state_s == 2)
			{
				if(led_config < 8)
				{
					led_config ++;
				} 
			}
		}
	}
	if(row4 == 0)//s8
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0);
			if(state_s == 0)
			{
				if(hour > 0)
				{
					hour--;
				}
			}
			else if(state_s == 1)
			{
				if(temper > 0)
				{
					temper --;
				}
			}
			else if(state_s == 2)
			{
				if(led_config > 4)
				{
					led_config --;
				} 
			}
		}
	}
}

void LED_Fun()
{
	if(time[2]/16*10+time[2]%16 == hour)
	{
		L1 = 0;
	}
	if(L1 == 0)
	{
		if(time[2]/16*10+time[2]%16 == 8)
		{
			L1 = 1;
		}
	}
	if(DS18B20_Temp/10000 < temper)
	{
		L2 = 0;
	}
	else
	{
		L2 = 1;
	}
	if(V < 180)
	{
		if(led_config == 4)
		{
			L4 = 0;
			L5 = 1;
			L6 = 1;
			L7 = 1;
			L8 = 1;
		}
		else if(led_config == 5)
		{
			L4 = 1;
			L5 = 0;
			L6 = 1;
			L7 = 1;
			L8 = 1;
		}
		else if(led_config == 6)
		{
			L4 = 1;
			L5 = 1;
			L6 = 0;
			L7 = 1;
			L8 = 1;
		}
		else if(led_config == 7)
		{
			L4 = 1;
			L5 = 1;
			L6 = 1;
			L7 = 0;
			L8 = 1;
		}
		else if(led_config == 8)
		{
			L4 = 1;
			L5 = 1;
			L6 = 1;
			L7 = 1;
			L8 = 0;
		}
	}
	else
	{
		L4 = 1;
		L5 = 1;
		L6 = 1;
		L7 = 1;
		L8 = 1;
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
	
	if(L4 == 0)
	{
		LED_Temp = LED_Temp & 0xf7;
	}
	else
	{
		LED_Temp = LED_Temp | 0x08;
	}
	
	if(L5 == 0)
	{
		LED_Temp = LED_Temp & 0xef;
	}
	else
	{
		LED_Temp = LED_Temp | 0x10;
	}
	
	if(L6 == 0)
	{
		LED_Temp = LED_Temp & 0xdf;
	}
	else
	{
		LED_Temp = LED_Temp | 0x20;
	}
	
	if(L7 == 0)
	{
		LED_Temp = LED_Temp & 0xbf;
	}
	else
	{
		LED_Temp = LED_Temp | 0x40;
	}
	
	if(L8 == 0)
	{
		LED_Temp = LED_Temp & 0x7f;
	}
	else
	{
		LED_Temp = LED_Temp | 0x80;
	}
	
	if(LED_Temp != LED)
	{
		LED = LED_Temp;
		P0 = LED;
		SelectHC573(4);
	}
}

void main()
{
	InitSystem();
	DS1302_Config();
	InitTimer0();
	while(1)
	{
		DS18B20_GetTemp();
		Read_DS1302();
		V = (unsigned long)500*PCF8591_Get(1)/255;
		if(state == 0)
		{
			if(state_s == 0)
			{
				Display_Time();
			}
			else if(state_s == 1)
			{
				Display_Temp();
			}
			else if(state_s == 2)
			{
				Display_V();
			}
		}
		else if(state == 1)
		{
			if(state_s == 0)
			{
				Display_Time_Config();
			}
			else if(state_s == 1)
			{
				Display_Temp_Config();
			}
			else if(state_s == 2)
			{
				Display_V_Config();
			}
		}
		Key();
		LED_Fun();
		if(V < 180)
		{
			L3_state = 0;
		}
		else
		{
			L3_state = 1;
		}
	}
}