#include "stc15.h"
#include "PCF8591.h"
code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};

bit td = 0;                   //状态:通道1或3 (0为通道1，1为通道3)
unsigned long F_Dis = 0;      //频率 Display，同时起到存储作用        
unsigned int V_Dis = 325;			//电压 Display
unsigned int V_Dis_1 = 325;		//电压 Display
unsigned int V_Dis_3 = 325;		//电压 Display
unsigned long N_Dis = 50;     //周期 Display
unsigned long f_dis_cc = 0;   //F_Dis变量
unsigned long keyDelayTime = 0;
//key定义：
sbit row1 = P3^0;
sbit row2 = P3^1;
sbit row3 = P3^2;
sbit row4 = P3^3;

unsigned char s4_jm = 0;

//key状态：
bit f_bc = 0;									//n保存
bit u_bc = 0;									//u保存
bit ButtonKeep = 0;						//按钮保持

//LED灯：
unsigned char LED = 0xff;
unsigned char LED_Temp = 0xff;
bit LedSwitch = 0;						//0:OPEN,1:CLOSE
bit L1 = 1;
bit L2 = 1;
bit L3 = 1;
bit L4 = 1;
bit L5 = 1;

//Timer
unsigned long t0_count = 0;
unsigned long t1_count = 0;
unsigned long us = 1000;
unsigned long V_Dis_3_hc = 0;
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


void InitTimers()
{
	TMOD = 0x16;
	TH0 = 0xff;
	TL0 = 0xff;
	TH1 = (65535 - 50000)/256;
	TL1 = (65535 - 50000)%256;
	ET0 = 1;
	ET1 = 1;
	TR0 = 1;
	TR1 = 1;
	EA = 1;
}

void Timer0() interrupt 1
{
	t0_count++;
}

void Timer1() interrupt 3
{
	TH1 = (65535 - 50000)/256;
	TL1 = (65535 - 50000)%256;
	if(ButtonKeep == 1)
	{
		keyDelayTime ++;
		if(keyDelayTime >= 20)
		{
			if(LedSwitch == 0)
			{
				LedSwitch = 1;
			}
			else
			{
				LedSwitch = 0;
			}
			keyDelayTime = 0;
		}
	}
	else
	{
		keyDelayTime = 0;
	}
	t1_count ++;
	if(t1_count >= 20)
	{
		if(f_bc == 0)
		{
			F_Dis = t0_count;
			f_dis_cc = t0_count;
		}
		else
		{
			f_dis_cc = t0_count;
		}
		N_Dis = 1000000/f_dis_cc;
		t0_count = 0;
		t1_count = 0;
	}
}

void InitSystem(void)
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

void Display_F(void)
{
	smgDisplay(0,Seg_Table[15]);
	Delay(500);
	
	if(F_Dis < 1000000)
		smgDisplay(2,0xff);
	else
		smgDisplay(2,Seg_Table[F_Dis/1000000%10]);
	Delay(500);
	
	if(F_Dis < 100000)
		smgDisplay(2,0xff);
	else
		smgDisplay(2,Seg_Table[F_Dis/100000%10]);
	Delay(500);
	
	if(F_Dis < 10000)
		smgDisplay(3,0xff);
	else
		smgDisplay(3,Seg_Table[F_Dis/10000%10]);
	Delay(500);
	
	if(F_Dis < 1000)
		smgDisplay(4,0xff);
	else
		smgDisplay(4,Seg_Table[F_Dis/1000%10]);
	Delay(500);
	
	if(F_Dis < 100)
		smgDisplay(5,0xff);
	else
		smgDisplay(5,Seg_Table[F_Dis/100%10]);
	Delay(500);
	
	if(F_Dis < 10)
		smgDisplay(6,0xff);
	else
		smgDisplay(6,Seg_Table[F_Dis/10%10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[F_Dis%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_N(void)
{
	smgDisplay(0,0xc8);//1100 1000 N
	Delay(500);
	
	if(N_Dis < 1000000)
		smgDisplay(1,0xff);
	else
		smgDisplay(2,Seg_Table[N_Dis/1000000%10]);
	Delay(500);
	
	if(N_Dis < 100000)
		smgDisplay(2,0xff);
	else
		smgDisplay(2,Seg_Table[N_Dis/100000%10]);
	Delay(500);
	
	if(N_Dis < 10000)
		smgDisplay(3,0xff);
	else
		smgDisplay(3,Seg_Table[N_Dis/10000%10]);
	Delay(500);
	
	if(N_Dis < 1000)
		smgDisplay(4,0xff);
	else
		smgDisplay(4,Seg_Table[N_Dis/1000%10]);
	Delay(500);
	
	if(N_Dis < 100)
		smgDisplay(5,0xff);
	else
		smgDisplay(5,Seg_Table[N_Dis/100%10]);
	Delay(500);
	
	if(N_Dis < 10)
		smgDisplay(6,0xff);
	else
		smgDisplay(6,Seg_Table[N_Dis/10%10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[N_Dis%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_U(void)
{
	smgDisplay(0,0xc1);//U 1100 0001
	Delay(500);
	
	smgDisplay(1,0xbf);//- 1011 1111
	Delay(500);
	
	if(td == 1)
	{
		smgDisplay(2,Seg_Table[3]);
	}
	else
	{
		smgDisplay(2,Seg_Table[1]);
	}
	Delay(500);
	
	smgDisplay(3,0xff);
	Delay(500);
	
	smgDisplay(4,0xff);
	Delay(500);
	
	smgDisplay(5,Seg_Table_Dot[V_Dis/100%10]);
	Delay(500);
	
	smgDisplay(6,Seg_Table[V_Dis/10%10]);
	Delay(500);
	
	smgDisplay(7,Seg_Table[V_Dis%10]);
	Delay(500);
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Key()
{
	if(row1 == 0)//S7
	{
		Delay(500);
		if(row1 == 0)
		{
			while(row1 == 0)
			{
				if(s4_jm == 0)
				{
					Display_F();
				}
				else if(s4_jm == 1)
				{
					Display_N();
				}
				else if(s4_jm == 2)
				{
					Display_U();
				}
				V_Dis_1 = (unsigned long)500*PCF8591_GETData(3)/255;
				V_Dis_3 = (unsigned long)500*PCF8591_GETData(1)/255;
				ButtonKeep = 1;
			}
			ButtonKeep = 0;
			if(f_bc == 0)
			{
				f_bc = 1;
			}
			else
			{
				f_bc = 0;
			}
		}
	}
	
	if(row2 == 0)//S6
	{
		Delay(500);
		if(row2 == 0)
		{
			while(row2 == 0)
			{
				if(s4_jm == 0)
				{
					Display_F();
				}
				else if(s4_jm == 1)
				{
					Display_N();
				}
				else if(s4_jm == 2)
				{
					Display_U();
				}
				V_Dis_1 = (unsigned long)500*PCF8591_GETData(3)/255;
				V_Dis_3 = (unsigned long)500*PCF8591_GETData(1)/255;
			}
			if(u_bc == 0)
			{
				u_bc = 1;
			}
			else
			{
				u_bc = 0;
			}
		}
	}
	
	if(row3 == 0)//S5
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0)
			{
				if(s4_jm == 0)
				{
					Display_F();
				}
				else if(s4_jm == 1)
				{
					Display_N();
				}
				else if(s4_jm == 2)
				{
					Display_U();
				}
				V_Dis_1 = (unsigned long)500*PCF8591_GETData(3)/255;
				V_Dis_3 = (unsigned long)500*PCF8591_GETData(1)/255;
			}
			if(td == 0)
			{
				td = 1;
			}
			else
			{
				td = 0;
			}
		}
	}
	if(row4 == 0)//S4
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0)
			{
				if(s4_jm == 0)
				{
					Display_F();
				}
				else if(s4_jm == 1)
				{
					Display_N();
				}
				else if(s4_jm == 2)
				{
					Display_U();
				}
				V_Dis_1 = (unsigned long)500*PCF8591_GETData(3)/255;
				V_Dis_3 = (unsigned long)500*PCF8591_GETData(1)/255;
			}
			if(s4_jm == 0)
			{
				s4_jm = 1;
			}
			else if(s4_jm == 1)
			{
				s4_jm = 2;
			}
			else if(s4_jm == 2)
			{
				s4_jm = 0;
			}
		}
	}
}

void Led_Fun()
{
	if(V_Dis_3_hc < V_Dis_3)
	{
		L1 = 0;
	}
	else
	{
		L1 = 1;
	}
	if(f_dis_cc >F_Dis)
	{
		L2 = 0;
	}
	else
	{
		L2 = 1;
	}
	if(s4_jm == 0)
	{
		L3 = 0;
	}
	else 
	{
		L3 = 1;
	}
	if(s4_jm == 1)
	{
		L4 = 0;
	}
	else
	{
		L4 = 1;
	}
	if(s4_jm == 2)
	{
		L5 = 0;
	}
	else
	{
		L5 = 1;
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
	
	if(LED_Temp != LED)
	{
		LED = LED_Temp;
		P0 = LED;
		SelectHC573(4);
	}
}

void main(void)
{
	InitSystem();
	InitTimers();
	while(1)
	{
		if(s4_jm == 0)
		{
			Display_F();
		}
		else if(s4_jm == 1)
		{
			Display_N();
		}
		else if(s4_jm == 2)
		{
			Display_U();
		}
		if(td == 0)
		{
			V_Dis = V_Dis_1;
		}
		else if(u_bc == 0)
		{
			V_Dis = V_Dis_3;
			V_Dis_3_hc = V_Dis_3;
		}
		V_Dis_1 = (unsigned long)500*PCF8591_GETData(3)/255;
		V_Dis_3 = (unsigned long)500*PCF8591_GETData(1)/255;
		Key();
		if(LedSwitch == 0)
		{
			Led_Fun();
			LED = LED_Temp;
			P0 = LED;
			SelectHC573(4);
		}
		else
		{
			P0 = 0xff;
			SelectHC573(4);
		}
	}
}