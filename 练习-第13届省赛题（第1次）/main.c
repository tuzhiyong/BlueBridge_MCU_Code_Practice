#include "stc15.h"
#include "main.h"
#include "onewire.h"
#include "ds1302.h"

code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};


code unsigned char Read_DS1302_addr[7] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
code unsigned char Write_DS1302_addr[7] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
unsigned char DS1302_Time[7] = {0x55,0x59,0x20,0x05,0x10,0x01,0x25};

sbit row1 = P3^0;
sbit row2 = P3^1;
sbit row3 = P3^2;
sbit row4 = P3^3;

sbit col1 = P4^2;
sbit col2 = P4^4;
sbit col3 = P3^4;
sbit col4 = P3^5;

bit _S13 = 0;
bit t0_flag = 0;
unsigned long DStemp = 235;
unsigned long tempConfig = 23;
unsigned long tempConfig_temp = 23;
unsigned char _S12 = 0;

unsigned char LED = 0xff;
unsigned char LED_Temp = 0xff;
bit L1 = 1;
bit L2 = 1;
bit L3 = 1;
bit L10 = 1;//继电器标志
void DS1302_Config()
{
	char i = 0;
	for(i = 0;i<7;i++)
	{
		Write_Ds1302_Byte(Write_DS1302_addr[i], DS1302_Time[i]);
	}
}

void Read_Ds1302()
{
	char i = 0;
	for(i = 0;i<7;i++)
	{
		DS1302_Time[i] = Read_Ds1302_Byte(Read_DS1302_addr[i]);
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
			P2 = (P2 & 0x1f)|0x80;
			P2 = P2 & 0x1f;
		break;
		case 5:
			P2 = (P2 & 0x1f)|0xa0;
			P2 = P2 & 0x1f;
		break;
		case 6:
			P2 = (P2 & 0x1f)|0xc0;
			P2 = P2 & 0x1f;
		break;
		case 7:
			P2 = (P2 & 0x1f)|0xe0;
			P2 = P2 & 0x1f;
		break;
	}
}

void InitTimer0()
{
	TMOD = 0x01;
	TH0 = (65535 - 100000)/256;
	TL0 = (65535 - 100000)%256;
	ET0 = 0;
	TR0 = 0;
	EA = 1;
}

void Timer0() interrupt 1
{
	unsigned char P2_temp;
	TH0 = (65535 - 100000)/256;
	TL0 = (65535 - 100000)%256;
	P2_temp = P2;
	if(t0_flag == 0)
	{
		LED_Temp = LED;
		LED_Temp = LED_Temp & 0xfb;
		LED = LED_Temp;
		P0 = LED;
		SelectHC573(4);
		t0_flag = 1;
	}
	else
	{
		LED_Temp = LED;
		LED_Temp = LED_Temp | 0x04;
		LED = LED_Temp;
		P0 = LED;
		SelectHC573(4);
		t0_flag = 0;
	}
	P2 = P2_temp;
}

void InitSystem()
{
	P0 = 0xff;
	SelectHC573(4);
	P0 = 0x00;
	SelectHC573(5);
}

void smgDisplay(unsigned char m,unsigned char n)
{
	P0 = 0x01 << m;
	SelectHC573(6);
	P0 = n;
	SelectHC573(7);
}

void Display_U1()
{
	smgDisplay(0,0xc1); //1000 0011  1100 0001
	Delay(500);
	
	smgDisplay(1,Seg_Table[1]);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,0xff);
	Delay(500);
	
	smgDisplay(4,0xff);
	Delay(500);
	
	smgDisplay(5,Seg_Table[DStemp/100%10]);
	Delay(500);
	
	smgDisplay(6,Seg_Table_Dot[DStemp/10%10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[DStemp%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
}

void Display_U2()
{
	smgDisplay(0,0xc1); //1000 0011
	Delay(500);
	
	smgDisplay(1,Seg_Table[2]);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,Seg_Table[DS1302_Time[2]/16]);
	Delay(500);
	
	smgDisplay(4,Seg_Table[DS1302_Time[2]%16]);
	Delay(500);
	
	smgDisplay(5,0xbf);//1011 1111
	Delay(500);
	
	smgDisplay(6,Seg_Table[DS1302_Time[1]/16]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[DS1302_Time[1]%16]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
}

void Display_U2S17()
{
	smgDisplay(0,0xc1); //1000 0011
	Delay(500);
	
	smgDisplay(1,Seg_Table[2]);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,Seg_Table[DS1302_Time[1]/16]);
	Delay(500);
	
	smgDisplay(4,Seg_Table[DS1302_Time[1]%16]);
	Delay(500);
	
	smgDisplay(5,0xbf);
	Delay(500);
	
	smgDisplay(6,Seg_Table[DS1302_Time[0]/16]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[DS1302_Time[0]%16]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
}

void Display_U3()
{
	smgDisplay(0,0xc1); //1000 0011
	Delay(500);
	
	smgDisplay(1,Seg_Table[3]);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,0xff);
	Delay(500);
	
	smgDisplay(4,0xff);
	Delay(500);
	
	smgDisplay(5,0xff);
	Delay(500);
	
	smgDisplay(6,Seg_Table[tempConfig_temp/10%10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[tempConfig_temp%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
}

void Key()
{
	col1 = 1;
	col2 = 1;
	col3 = 0;
	col4 = 1;
	if(row1 == 0)//s15代替s17
	{
		Delay(500);
		if(row1 == 0)
		{
			while(row1 == 0)
			{
				if(_S13 == 1)
				{
					Display_U2S17();
				}
				else
				{
					if(_S12 == 0)
					{
						Display_U1();
					}
					else if(_S12 == 1)
					{
						Display_U2();
					}
					else if(_S12 == 2)
					{
						Display_U3();
					}
				}
				DS18B20_GETTemp();
				Read_Ds1302();
				LED_Fun();
				L10_Fun();
			}
			if(_S13 == 0 && tempConfig_temp > 10)
			{
				tempConfig_temp --;
			}
		}
	}
	if(row2 == 0)//s14代替s16
	{
		Delay(500);
		if(row2 == 0)
		{
			while(row2 == 0)
			{
				DS18B20_GETTemp();
				Read_Ds1302();
				LED_Fun();
				L10_Fun();
				if(_S12 == 0)
				{
					Display_U1();
				}
				else if(_S12 == 1)
				{
					Display_U2();
				}
				else if(_S12 == 2)
				{
					Display_U3();
				}
			}
			if(tempConfig_temp < 99)
			{
				tempConfig_temp ++;
			}
		}
	}
	if(row3 == 0)//s13
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0)
			{
				DS18B20_GETTemp();
				Read_Ds1302();
				LED_Fun();
				L10_Fun();
				if(_S12 == 0)
				{
					Display_U1();
				}
				else if(_S12 == 1)
				{
					Display_U2();
				}
				else if(_S12 == 2)
				{
					Display_U3();
				}
			}
			if(_S13 == 0)
			{
				_S13 = 1;
			}
			else
			{
				_S13 = 0;
			}
		}
	}
	if(row4 == 0)//s12
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0)
			{
				DS18B20_GETTemp();
				Read_Ds1302();
				LED_Fun();
				L10_Fun();
				if(_S12 == 0)
				{
					Display_U1();
				}
				else if(_S12 == 1)
				{
					Display_U2();
				}
				else if(_S12 == 2)
				{
					Display_U3();
				}
			}
			_S12++;
			if(_S12 >= 3)
			{
				_S12 = 0;
			}
			if(_S12 == 0)
			{
				tempConfig = tempConfig_temp;
			}
		}
	}
}

void LED_Fun()
{
	if(DS1302_Time[0] <= 0x05 && DS1302_Time[1] == 0x00)
	{
		L1 = 0;
	}
	else if(DS1302_Time[0] >= 0x05 && DS1302_Time[1] == 0x00)
	{
		L1 = 1;
	}
	
	if(_S13 == 0)
	{
		L2 = 0;
	}
	else
	{
		L2 = 1;
	}
	
	if(L10 == 0)
	{
		L3 = 0;
		
		ET0 = 1;
		TR0 = 1;
	}
	else
	{
		L3 = 1;
		
		ET0 = 0;
		TR0 = 0;
		LED_Temp = LED;
		LED_Temp = LED_Temp | 0x04;
		LED = LED_Temp;
		P0 = LED;
		SelectHC573(4);
	}
	
	if(L1 == 0)
	{
		LED_Temp =LED_Temp & 0xfe;
	}
	else
	{
		LED_Temp =LED_Temp | 0x01;
	}
	
	if(L2 == 0)
	{
		LED_Temp =LED_Temp & 0xfd;
	}
	else
	{
		LED_Temp =LED_Temp | 0x02;
	}
	
	if((LED_Temp&0xc0)!=(LED&0xc0))
	{
		LED = LED_Temp;
		P0 = LED;
		SelectHC573(4);
	}
}

void DS18B20_GETTemp()
{
	unsigned char LSB,MSB;
	unsigned long temp;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	Delay(7000);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	init_ds18b20();
	temp = MSB;
	temp = (temp << 8)|LSB;
	DStemp = temp * 625 /1000;
}

void L10_Fun()
{
	unsigned char p2_temp;
	if(_S13 == 0)
	{
		if(DStemp > tempConfig*10)
		{
			p2_temp = P2;
			L10 = 0;
			P0 = 0x10;
			SelectHC573(5);
			P2 = p2_temp;
		}
		else
		{
			p2_temp = P2;
			L10 = 1;
			P0 = 0x00;
			SelectHC573(5);
			P2 = p2_temp;
		}
	}
	else if(_S13 == 1)
	{
		if(DS1302_Time[0] <= 0x05 && DS1302_Time[1] == 0x00)
		{
			p2_temp = P2;
			L10 = 0;
			P0 = 0x10;
			SelectHC573(5);
			P2 = p2_temp;
		}
		else if(DS1302_Time[0] >= 0x05 && DS1302_Time[1] == 0x00)
		{
			p2_temp = P2;
			L10 = 1;
			P0 = 0x00;
			SelectHC573(5);
			P2 = p2_temp;
		}
	}
}

void main()
{
	InitSystem();
	InitTimer0();
	DS1302_Config();
	while(1)
	{
		if(_S12 == 0)
		{
			Display_U1();
		}
		else if(_S12 == 1)
		{
			Display_U2();
		}
		else if(_S12 == 2)
		{
			Display_U3();
		}
		DS18B20_GETTemp();
		Read_Ds1302();
		LED_Fun();
		L10_Fun();
		Key();
	}
}