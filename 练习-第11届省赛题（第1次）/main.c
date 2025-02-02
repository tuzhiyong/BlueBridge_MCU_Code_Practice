#include "stc15.h"
#include "at24c02.h"
#include "PCF8591.h"

code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};

unsigned int U_ain3 = 324;
unsigned int P_p = 300;
unsigned long N_count = 0;

//GET:U(V)
unsigned int U_temp = 0;

sbit row1 = P3^0;
sbit row2 = P3^1;
sbit row3 = P3^2;
sbit row4 = P3^3;

sbit col1 = P4^4;
sbit col2 = P4^2;
sbit col3 = P3^5;
sbit col4 = P3^4;

//Button
unsigned char s12_dis = 0;
unsigned char Button_count = 0;

//LED:L1,L2,L3
unsigned char led = 0xff;
unsigned char led_temp = 0xff;
unsigned char L1 = 1;
unsigned char L2 = 1;
unsigned char L3 = 1;

//Timer
unsigned char t0_count = 0;

//states
unsigned char H = 3;
unsigned char H_locker = 0;

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

void InitSystem()
{
	SelectHC573(4);
	P0 = 0xff;
	SelectHC573(5);
	P0 = 0x00;
	P2 = 0x00;
}

void InitTimer0()
{
	TMOD = 0x01;
	TH0 = (65535 - 50000)/256;
	TL0 = (65535 - 50000)%256;
	ET0 = 0;
	TR0 = 0;
	EA = 1;
}

void Timer0() interrupt 1
{
	TH0 = (65535 - 50000)/256;
	TL0 = (65535 - 50000)%256;
	if(U_ain3 < P_p)
	{
		t0_count ++;
	}
	else
	{
		t0_count = 0;
	}
	if(t0_count >= 100)
	{
		t0_count = 0;
		L1 = 0;
		ET0 = 0;
		TR0 = 0;
	}
}

void smgDisplay(unsigned char m,unsigned char n)
{
	SelectHC573(6);
	P0 = 0x01 << m;
	SelectHC573(7);
	P0 = n;
}

void Display_U()
{
	P0 = 0xff;
	smgDisplay(0,0xc1);//1100 0001  U
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
	smgDisplay(5,Seg_Table_Dot[U_ain3/100]);
	Delay(500); 
	
	P0 = 0xff;
	smgDisplay(6,Seg_Table[U_ain3/10%10]);
	Delay(500); 
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[U_ain3%10]);
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
	smgDisplay(0,0x8c);//1000 1100  P
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
	smgDisplay(5,Seg_Table_Dot[P_p/100]);
	Delay(500); 
	
	P0 = 0xff;
	smgDisplay(6,Seg_Table[P_p/10%10]);
	Delay(500); 
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[P_p%10]);
	Delay(500); 
	
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	P0 = 0xff;
	Delay(500);
}

void Display_N()
{
	P0 = 0xff;
	smgDisplay(0,0xc8);//1100 1000  N
	Delay(500); 
	
	P0 = 0xff;
	if(N_count >= 1000000)
		smgDisplay(1,Seg_Table[N_count/100000%10]);
	else
		smgDisplay(1,0xff);
	Delay(500); 
	
	P0 = 0xff;
	if(N_count >= 100000)
		smgDisplay(2,Seg_Table[N_count/100000%10]);
	else
		smgDisplay(2,0xff);
	Delay(500); 
	
	P0 = 0xff;
	if(N_count >= 10000)
		smgDisplay(3,Seg_Table[N_count/10000%10]);
	else
		smgDisplay(3,0xff);
	Delay(500); 
	
	P0 = 0xff;
	if(N_count >= 1000)
		smgDisplay(4,Seg_Table[N_count/1000%10]);
	else
		smgDisplay(4,0xff);
	Delay(500); 
	
	P0 = 0xff;
	if(N_count >=100)
		smgDisplay(5,Seg_Table[N_count/100%10]);
	else
		smgDisplay(6,0xff);
	Delay(500); 
	
	P0 = 0xff;
	if(N_count>=10)
		smgDisplay(6,Seg_Table[N_count/10%10]);
	else
		smgDisplay(6,0xff);
	Delay(500); 
	
	P0 = 0xff;
	smgDisplay(7,Seg_Table[N_count%10]);
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
	col1 = 1;
	col2 = 1;
	col3 = 0;
	col4 = 1;
	if(s12_dis == 2)
	{
		if(row3 == 0)//S13
		{
			Delay(500);
			if(row3 == 0)
			{
				while(row3 == 0);
				if(N_count != 0)
				{
					N_count = 0;
					Button_count = 0;
				}
				else
				{
					if(Button_count < 3)
						Button_count++;
				}
			}
		}
	}
	if(row4 == 0)//S12
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0);
			Button_count = 0;
			s12_dis++;
			if(s12_dis == 2)
			{
				//...
				AT24C02_Wirte(0x00,P_p/10);
				Delay(10000);
			}
			if(s12_dis >= 3)
			{
				s12_dis = 0;
			}
		}
	}
	col1 = 1;
	col2 = 0;
	col3 = 1;
	col4 = 1;
	if(s12_dis == 1)
	{
		if(row3 == 0)//S17
		{
			Delay(500);
			if(row3 == 0)
			{
				while(row3 == 0);
				if(P_p>0)
				{
					P_p -= 50;
					Button_count = 0;
				}
				else
				{
					if(Button_count < 3)
						Button_count++;
				}
			}
		}
		if(row4 == 0)//S16
		{
			Delay(500);
			if(row4 == 0)
			{
				while(row4 == 0);
				if(P_p<500)
				{
					P_p +=50;
					Button_count = 0;
				}
				else
				{
					if(Button_count < 3)
						Button_count ++;
				}
			}
		}
	}
}

void Led_fun()
{
	unsigned char m;
	if(Button_count < 3)
	{
		L3 = 1;
	}
	else
	{
		L3 = 0;
	}
	
	if(U_ain3 >= P_p)
	{
		//²»ÁÁ
		ET0 = 0;
		TR0 = 0;
		L1 = 1;
	}
	else
	{
		//Æô¶¯£¬½«ÁÁ
		ET0 = 1;
		TR0 = 1;
	}
	
	if(N_count %2 == 1)
	{
		L2 = 0;
	}
	else
	{
		L2 = 1;
	}
	
	if(L1 == 0)
	{
		led_temp = led_temp & 0xfe;
	}
	else if(L1 == 1)
	{
		led_temp = led_temp | 0x01;
	}
	if(L2 == 0)
	{
		led_temp = led_temp & 0xfd;//1111 1101
	}
	else if(L2 == 1)
	{
		led_temp = led_temp | 0x02;
	}
	
	if(L3 == 0)
	{
		led_temp = led_temp & 0xfb;//1111 1011
	}
	else if(L3 == 1)
	{
		led_temp = led_temp | 0x04;
	}
	
	if(led_temp != led)
	{
		led = led_temp;
		m = P2;
		SelectHC573(4);
		P0 = led;
		P2 = m;
	}
}

void main()
{
	InitSystem();
	InitTimer0();
	P_p = (unsigned int)10*AT24C02_Read(0x00);
	while(1)
	{
		if(s12_dis == 0)
		{
			Display_U();
		}
		else if(s12_dis == 1)
		{
			Display_P();
		}
		else if(s12_dis == 2)
		{
			Display_N();
		}
		U_temp = PCF8591_GetData(3);
		U_ain3 = U_temp*((unsigned long)500)/((unsigned long)255);
		Key();
		Led_fun();
		if(U_ain3 > P_p)
		{
			if(H ==0)
			{
				H = 1;
				H_locker = 0;
			}
		}
		if(U_ain3 <= P_p)
		{
			H = 0;
		}
		if(H == 1)
		{
			if(H_locker == 0)
			{
				N_count ++;
				H_locker = 1;
			}
		}
	}
}