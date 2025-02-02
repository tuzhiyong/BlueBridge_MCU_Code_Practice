#include "stc15.h"
#include "PCF8591.h"

code unsigned char Seg_Table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};

sbit S7 = P3^0;
sbit S6 = P3^1;
sbit S5 = P3^2;
sbit S4 = P3^3;

//Timers
unsigned long time0 = 0;
unsigned char time1 = 0;

//HZ
unsigned long hz = 0;

//U:v
unsigned long v = 341;

//Button
unsigned char v_hz = 0;
unsigned char v_2v = 0;
unsigned char ledSwitch = 0;
unsigned char smg = 0;

//LED
unsigned char led = 0xff;
unsigned char ledTemp = 0xff;

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

void Delay(unsigned int n)
{
	while(n--);
}

void InitTimers()
{
	TH0 = 0xff;
	TL0 = 0xff;
	
	TH1 = (65535 - 50000)/256;
	TL1 = (65535 - 50000)%256;
	
	TMOD = 0x16;
	
	ET0 = 1;
	ET1 = 1;
	EA = 1;
	
	TR0 = 1;
	TR1 = 1;
}

void timer0() interrupt 1
{
	time0 ++;
}

void timer1() interrupt 3
{
	time1 ++;
	if(time1 >= 20)
	{
		time1 = 0;
		hz = time0;
		time0 = 0;
	}
}

void InitSystem()
{
	P2 = 0x00;
	SelectHC573(4);
	P0 = 0xff;
	SelectHC573(5);
	P0 = 0x00;
	P2 = 0x00;
}

void smgDisplay(unsigned char m,unsigned char n)
{
	SelectHC573(6);
	P0 = 0x01 << m;
	SelectHC573(7);
	P0 = n;
}

void Display_HZ()
{
	P0 = 0xff;
	smgDisplay(0,Seg_Table[15]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(1,0xff);
	Delay(500);
	
	P0 = 0xff;
	if(hz >= 100000)
		smgDisplay(2,Seg_Table[hz/100000%10]);
	else
		smgDisplay(2,0xff);
	Delay(500);
	
	P0 = 0xff;
	if(hz >= 10000)
		smgDisplay(3,Seg_Table[hz/10000%10]);
	else
		smgDisplay(3,0xff);
	Delay(500);
	
	P0 = 0xff;
	if(hz >=1000)
		smgDisplay(4,Seg_Table[hz/1000%10]);
	else
		smgDisplay(4,0xff);
	Delay(500);
	
	P0 = 0xff;
	if(hz >=100)
		smgDisplay(5,Seg_Table[hz/100%10]);
	else
		smgDisplay(5,0xff);
	Delay(500);
	
	P0 = 0xff;
	if(hz >=10)
		smgDisplay(6,Seg_Table[hz/10%10]);
	else
		smgDisplay(6,0xff);
	Delay(500);
	
	P0 = 0xff;
	if(hz >=0)
		smgDisplay(7,Seg_Table[hz%10]);
	else
		smgDisplay(7,0xff);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
	Delay(500);
}

void Display_V()
{
	P0 = 0xff;
	smgDisplay(0,0xc1);
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
	smgDisplay(5,Seg_Table_Dot[v/100%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(6,Seg_Table[v/10%10]);
	Delay(500);
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[v%10]);
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
	if(S7 == 0)
	{
		Delay(500);
		if(S7 == 0)
		{
			while(S7 == 0);
			if(smg == 0)
			{
				smg = 1;
			}
			else
			{
				smg = 0;
			}
		}
	}
	if(S6 == 0)
	{
		Delay(500);
		if(S6 == 0)
		{
			while(S6 == 0);
			if(ledSwitch == 0)
			{
				ledSwitch = 1;
			}
			else
			{
				ledSwitch = 0;
			}
		}
	}
	if(S5 == 0)
	{
		Delay(500);
		if(S5 == 0)
		{
			while(S5 == 0);
			if(v_2v == 0)
			{
				v_2v = 1;
			}
			else
			{
				v_2v = 0;
			}
		}
	}
	if(S4 == 0)
	{
		Delay(500);
		if(S4 == 0)
		{
			while(S4 == 0);
			if(v_hz == 0)
			{
				v_hz = 1;
			}
			else
			{
				v_hz = 0;
			}
		}
	}
}

void Funled()
{
	unsigned char pp2 = 0x00;
	if(v < 150)
	{
		ledTemp = ledTemp | 0x04;//0000 0100
	}
	else if(v >=150 && v < 250)
	{
		ledTemp = ledTemp & 0xfb;//1111 1011
	}
	else if(v >=250 && v < 350)
	{
		ledTemp = ledTemp | 0x04;//0000 0100
	}
	else if(v >= 350 )
	{
		ledTemp = ledTemp & 0xfb;//1111 1011
	}
	
	if(hz < 1000)
	{
		ledTemp = ledTemp | 0x08;//0000 1000
	}
	else if(hz >=1000 && hz < 5000)
	{
		ledTemp = ledTemp & 0xf7;//1111 0111
	}
	else if(hz >=5000 && hz < 10000)
	{
		ledTemp = ledTemp | 0x08;//0000 1000
	}
	else if(hz >= 10000 )
	{
		ledTemp = ledTemp & 0xf7;//1111 0111
	}
	
	if(v_2v == 1)
	{
		ledTemp = ledTemp | 0x10;//0001 0000
	}
	else
	{
		ledTemp = ledTemp & 0xef;//1110 1111
	}
	
	if(ledSwitch == 0)
	{
		if(ledTemp != led)
		{
			led = ledTemp;
			pp2 = P2;
			SelectHC573(4);
			P0 = led;
			P2 = pp2;
		}
	}
	else if(ledSwitch == 1)
	{
		led = 0xff;
		pp2 = P2;
		SelectHC573(4);
		P0 = led;
		P2 = pp2;
	}
	
}

void main()
{
	unsigned long v_v;
	InitSystem();
	InitTimers();
	while(1)
	{
		if(v_hz == 0)
		{
			if(v_2v == 1)
			{
				v_v = PCF8591_GetAD(3);
			}
			else
			{
				v_v = 102;
			}
			PCF8591_DAOut(v_v);
			v = 500*v_v/255;
			if(smg == 0)
			{
				Display_V();
			}
		}
		else
		{
			if(smg == 0)
			{
				Display_HZ();
			}
		}
		Key();
		Funled();
	}
}