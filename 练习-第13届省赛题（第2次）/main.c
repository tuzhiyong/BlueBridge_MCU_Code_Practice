#include "stc15.h"
#include <intrins.h>
#include "main.h"
#include "PCF8591.h"

code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};

unsigned int v = 245;
unsigned int v_max = 450;
unsigned int v_min = 50;
unsigned int v_max_temp = 450;
unsigned int v_min_temp = 50;
unsigned int distance = 45;
unsigned char t0 = 0;
unsigned int t0_1 = 0;
sbit s4 = P3^3;
sbit s5 = P3^2;
sbit s6 = P3^1;
sbit s7 = P3^0;
sbit Tx = P1^0;
sbit Rx = P1^1;

unsigned char s4_k = 0;
bit s5_k = 0;
bit wave_bz = 0;
bit L1 = 1;
bit L2 = 1;
bit L3 = 1;
bit L8 = 1;
bit sonar = 0;
unsigned char LED = 0xff,LED_Temp = 0xff;

void Delay_12us(void)	//@11.0592MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	_nop_();
	i = 30;
	while (--i);
}

void Delay(unsigned char n)
{
	while(n--);
}

void Ut_Wave_Init()
{
	unsigned char i;
	for(i = 0;i < 8;i ++)
	{
		Tx = 0;
		Delay_12us();
		Tx = 1;
		Delay_12us();
	}
}

unsigned char Ut_Wave_Data()
{
	unsigned int time;
	TMOD &= 0x0f;
	TH1 = TL1 = 0;
	ET1 = 1;EA = 1;
	Ut_Wave_Init();//发送超声波
	while((Rx == 1) && (TF1 == 0));//接收到超声波或者定时器1溢出
	TR1 = 0;//把定时器1关掉
	if(TF1 == 0)
	{
		time = TH1 << 8|TL1;
		return (time * 0.017);
	}
	else
	{
		TF1 = 0;
		return 0;
	}
}

void SelectHC573(unsigned char n)
{
	switch(n)
	{
		case 4:
			P2 = (P2 & 0x1f) | 0x80;
			P2 = P2 & 0x1f;
		break;
		case 5:
			P2 = (P2 & 0x1f) | 0xA0;
			P2 = P2 & 0x1f;
		break;
		case 6:
			P2 = (P2 & 0x1f) | 0xC0;
			P2 = P2 & 0x1f;
		break;
		case 7:
			P2 = (P2 & 0x1f) | 0xE0;
			P2 = P2 & 0x1f;
		break;
	}
}

void InitTimer0()
{
	TMOD = 0x01;
	TH0 = (65535 - 50000)/256;
	TL0 = (65535 - 50000)%256;
	ET0 = 1;
	TR0 = 0;
	EA = 1;
}

void Timer0() interrupt 1
{
	TH0 = (65535 - 50000)/256;
	TL0 = (65535 - 50000)%256;
	t0++;
	t0_1++;
	if(t0 >=2)
	{
		t0 = 0;
		L8 ^= 1;
	}
	if(t0_1 >= 10)
	{
		t0_1 = 0;
		sonar = 1;
	}
}

void smgDisplay(unsigned char m,unsigned char n)
{
	P0 = 0x01 << m;
	SelectHC573(6);
	P0 = n;
	SelectHC573(7);
}

void Display_v()
{
	smgDisplay(0,0xc1);//1100 0001 U
	Delay(500);
	
	smgDisplay(1,0xff);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,0xff);
	Delay(500);
	
	smgDisplay(4,0xff);
	Delay(500);
	
	smgDisplay(5,Seg_Table_Dot[v/100%10]);
	Delay(500);
	
	smgDisplay(6,Seg_Table[v/10%10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[v%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_p()
{
	smgDisplay(0,0x8c);//1000 1100 P
	Delay(500);
	
	smgDisplay(1,0xff);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,Seg_Table_Dot[v_max_temp/100%10]);
	Delay(500);
	
	smgDisplay(4,Seg_Table[v_max_temp/10%10]);
	Delay(500);
	
	smgDisplay(5,0xff);
	Delay(500);
	
	smgDisplay(6,Seg_Table_Dot[v_min_temp/100%10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[v_min_temp/10%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_L0()
{
	smgDisplay(0,0xc3);//1100 0011  L
	Delay(500);
	
	smgDisplay(1,0xff);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,0xff);
	Delay(500);
	
	smgDisplay(4,0xff);
	Delay(500);
	
	if(distance >= 100)
		smgDisplay(5,Seg_Table[distance/100%10]);
	else
		smgDisplay(5,0xff);
	Delay(500);
	
	if(distance >= 10)
		smgDisplay(6,Seg_Table[distance/10%10]);
	else
		smgDisplay(6,0xff);
	Delay(500);
	
	smgDisplay(7,Seg_Table[distance%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_L1()
{
	smgDisplay(0,0xc3);//1100 0011  L
	Delay(500);
	
	smgDisplay(1,0xff);
	Delay(500);
	
	smgDisplay(2,0xff);
	Delay(500);
	
	smgDisplay(3,0xff);
	Delay(500);
	
	smgDisplay(4,0xff);
	Delay(500);
	
	smgDisplay(5,Seg_Table[10]);
	Delay(500);
	
	smgDisplay(6,Seg_Table[10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Key()
{
	if(s4 == 0)
	{
		Delay(500);
		if(s4 == 0)
		{
			while(s4 == 0);
			s4_k ++;
			if(s4_k > 2)
			{
				s4_k = 0;
				v_max = v_max_temp;
				v_min = v_min_temp;
			}
		}
	}
	if(s5 == 0)
	{
		Delay(500);
		if(s5 == 0)
		{
			while(s5 == 0);
			if(s5_k == 0)
			{
				s5_k = 1;
			}
			else
			{
				s5_k = 0;
			}
		}
	}
	
	if(s6 == 0 && s4_k == 2)
	{
		Delay(500);
		if(s6 == 0)
		{
			while(s6 == 0);
			if(s5_k == 0)
			{
				if(v_max_temp != 500)
				{
					v_max_temp += 50;
				}
			}
			else
			{
				if(v_min_temp != 500)
				{
					v_min_temp += 50;
				}
			}
		}
	}
	
	if(s7 == 0 && s4_k == 2)
	{
		Delay(500);
		if(s7 == 0)
		{
			while(s7 == 0);
			if(s5_k == 0)
			{
				if(v_max_temp != 50)
				{
					v_max_temp -= 50;
				}
			}
			else
			{
				if(v_min_temp != 50)
				{
					v_min_temp -= 50;
				}
			}
		}
	}
}

void InitSystem()
{
	P0 = 0xff;
	SelectHC573(4);
	P0 = 0x00;
	SelectHC573(5);
}

void LED_Fun()
{
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
	unsigned int t = 0;
	InitSystem();
	InitTimer0();
	while(1)
	{
		if(s4_k == 0)
		{
			Display_v();
			L1 = 0;
			L2 = 1;
			L3 = 1;
		}
		else if(s4_k == 1)
		{
			if(v<=v_max && v>= v_min)
			{
				Display_L0();
			}
			else
			{
				Display_L1();
			}
			L2 = 0;
			L1 = 1;
			L3 = 1;
		}
		else if(s4_k == 2)
		{
			Display_p();
			L1 = 1;
			L2 = 1;
			L3 = 0;
		}
		
		
		if(v<=v_max && v>= v_min)
		{
			TR0 = 1;
			if(distance>=80)
			{
				PCF8591_PUT(255);
			}
			else if(distance<=20)
			{
				PCF8591_PUT(51);
			}
			else
			{
				PCF8591_PUT((distance-5)*51/15);
			}
		}
		else
		{
			TR0 = 0;
			L8 = 1;
			PCF8591_PUT(0);
		}
		v = ((unsigned long)500 * PCF8591_GETAD(3))/(unsigned long)255;
		LED_Fun();
		Key();
		
		
		//
		if(sonar == 1)
		{
			sonar = 0;
			distance = Ut_Wave_Data();
		}
	}
}