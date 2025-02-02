#include "stc15.h"
#include "ds18b20.h"
#include "PCF8591.h"

//keyboard:keyboardDefine
sbit row1 = P3^0;
sbit row2 = P3^1;
sbit row3 = P3^2;
sbit row4 = P3^3;

sbit col1 = P4^4;
sbit col2 = P4^2;
sbit col3 = P3^5;
sbit col4 = P3^4;

code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};

void Display_C();
void Display_P();
void Display_A();
void LED_Fun();

void Delay(unsigned int n);

unsigned long Temp;

//Keyboard:keyboardValue
unsigned char s4_keyValue = 0;
unsigned char s5_mode = 0;
//Temp
unsigned int Temp_config;
unsigned int Temp_config_temp = 25;

unsigned char t0_count = 39;

unsigned char L1 = 1;
unsigned char L2 = 1;
unsigned char L3 = 1;
unsigned char L4 = 1;
unsigned char LED = 0xff;
unsigned char LED_Temp = 0xff;
unsigned int DAC = 0;
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
			//P2 = (P2 & 0x1f);
		break;
		case 5:
			P2 = (P2 & 0x1f)|0xa0;
			//P2 = (P2 & 0x1f);
		break;
		case 6:
			P2 = (P2 & 0x1f)|0xc0;
			//P2 = (P2 & 0x1f);
		break;
		case 7:
			P2 = (P2 & 0x1f)|0xe0;
			//P2 = (P2 & 0x1f);
		break;
	}
}

void InitTimer()
{
	TMOD = 0x01;
	TH0 = (65535 - 20000)/256;
	TL0 = (65535 - 20000)%256;
	ET0 = 1;
	TR0 = 1;
	EA = 1;
}

void Timer0() interrupt 1
{
	TH0 = (65535 - 20000)/256;
	TL0 = (65535 - 20000)%256;
	t0_count++;
	if(t0_count>=40)
	{
		t0_count = 0;
		Temp = ((unsigned long)DS18B20_GetTemp()/100);//»ñÈ¡ÎÂ¶È
	}
	
	
	if(s5_mode == 0)
	{
		if(Temp<(Temp_config*100))
		{
			PCF8591_PutData(0,0);
			DAC = 0;
		}
		else
		{
			PCF8591_PutData(0,255);
			DAC = 500;
		}
	}
	else if(s5_mode == 1)
	{
		if(Temp<=2000)
		{
			PCF8591_PutData(0,51);
			DAC = 100;
		}
		else if(Temp>=4000)
		{
			PCF8591_PutData(0,204);
			DAC = 400;
		}
		else
		{
			PCF8591_PutData(0,Temp*3/20-200);
			DAC = Temp*3/20-200;
		}
	}
	Delay(500);
	
}

void smgDisplay(unsigned char m,unsigned char n)
{
	
	P0 = 0x01 << m;
	SelectHC573(6);
	P2 = P2 & 0x1f;
	P0 = n;
	SelectHC573(7);
	P2 = P2 & 0x1f;
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
	smgDisplay(4,Seg_Table[Temp/1000%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(5,Seg_Table_Dot[Temp/100%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(6,Seg_Table[Temp/10%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[Temp%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
	Delay(500);
}

void Display_P()
{
	P0 = 0xff;
	smgDisplay(0,0x8c);
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
	if(Temp_config_temp/10%10!=0)
		smgDisplay(6,Seg_Table[Temp_config_temp/10%10]);
	else
		smgDisplay(6,0xff);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[Temp_config_temp%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
	Delay(500);
}

void Display_A()
{
	P0 = 0xff;
	smgDisplay(0,Seg_Table[10]);
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
	smgDisplay(5,Seg_Table_Dot[DAC/100%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(6,Seg_Table[DAC/10%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[DAC%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
	Delay(500);
}

void Key()
{
	col1 = 0;
	col2 = 1;
	col3 = 1;
	col4 = 1;
	if(row3 == 0)//s5
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0)
			{
				if(s4_keyValue == 0)
				{
					Display_C();
				}
				else if(s4_keyValue == 1)
				{
					Display_P();
				}
				else if(s4_keyValue == 2)
				{
					Display_A();
				}
			}
			s5_mode ++;
			if(s5_mode >= 2)
			{
				s5_mode = 0;
			}
		}
	}
	
	if(row4 == 0)//s4
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0)
			{
				if(s4_keyValue == 0)
				{
					Display_C();
				}
				else if(s4_keyValue == 1)
				{
					Display_P();
				}
				else if(s4_keyValue == 2)
				{
					Display_A();
				}
			}
			s4_keyValue ++;
			if(s4_keyValue >=3)
			{
				s4_keyValue = 0;
			}
			if(s4_keyValue == 2)
			{
				Temp_config = Temp_config_temp;
			}
		}
	}
	
	col1 = 1;
	col2 = 0;
	col3 = 1;
	col4 = 1;
	if(row3 == 0)//s9
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0)
			{
				if(s4_keyValue == 0)
				{
					Display_C();
				}
				else if(s4_keyValue == 1)
				{
					Display_P();
				}
				else if(s4_keyValue == 2)
				{
					Display_A();
				}
			}
			if(Temp_config_temp <99&&s4_keyValue == 1)
			{
				Temp_config_temp ++;
			}
		}
	}
	
	if(row4 == 0)//s8
	{
		Delay(500);
		if(row4 == 0)
		{
			if(s4_keyValue == 1)
			{
				while(row4 == 0)
				{
					if(s4_keyValue == 0)
					{
						Display_C();
					}
					else if(s4_keyValue == 1)
					{
						Display_P();
					}
					else if(s4_keyValue == 2)
					{
						Display_A();
					}
				}
				if(Temp_config_temp >0)
				{
					Temp_config_temp --;
				}
			}
		}
	}
}

void LED_Fun()
{
	unsigned char P2_Temp;
	if(s5_mode == 0)
	{
		L1 = 0;
	}
	else
	{
		L1 = 1;
	}
	if(s4_keyValue == 0)
	{
		L2 = 0;
	}
	else
	{
		L2 = 1;
	}
	if(s4_keyValue == 1)
	{
		L3 = 0;
	}
	else
	{
		L3 = 1;
	}
	if(s4_keyValue == 2)
	{
		L4 = 0;
	}
	else
	{
		L4 = 1;
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
	if(LED_Temp != LED)
	{
		LED = LED_Temp;
		//
		P2_Temp = P2;
		P0 = LED;
		SelectHC573(4);
		P2 = P2 & 0X1f;
		P2 = P2_Temp;
	}
}

void InitSystem()
{
	P0 = 0xff;
	SelectHC573(4);
	P2 = P2 & 0X1f;
	
	P0 = 0x00;
	SelectHC573(5);
	P2 = P2 & 0x1f;
	
	Temp_config = Temp_config_temp;
}

void main()
{
	InitSystem();
	InitTimer();
	while(1)
	{
		Key();
		LED_Fun();
		if(s4_keyValue == 0)
		{
			Display_C();
		}
		else if(s4_keyValue == 1)
		{
			Display_P();
		}
		else if(s4_keyValue == 2)
		{
			Display_A();
		}
	}
}