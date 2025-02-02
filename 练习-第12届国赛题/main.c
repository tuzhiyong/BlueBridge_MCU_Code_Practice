#include "stc15.h"
#include "ds1302.h"
#include "PCF8591.h"
#include <intrins.h>

sbit Tx = P1^0;
sbit Rx = P1^1;
code unsigned char Seg_Table[] ={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
code unsigned char Seg_Table_Dot[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x0e};
code unsigned char Write_DS1302_Code[7] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
code unsigned char Read_DS1302_Code[7] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
unsigned char Display_Distance_Mode = 12;
unsigned char DS1302_Time[7] = {0x00,0x01,0x01,0x03,0x01,0x01,0x25};
unsigned char distance = 15;
unsigned char distanceMax = 30;
unsigned char distanceMin  = 5;
unsigned int distanceAvg = 226;
unsigned char gatherTime = 2;
unsigned char gatherDistance = 20;
bit firstTest = 0;
sbit row3 = P3^2;
sbit row4 = P3^3;
sbit col1 = P4^4;
sbit col2 = P4^2;
bit s4Key = 0;
unsigned char s5Key_0 = 0;
unsigned char s5Key_1 = 0;
bit s8Key_0 = 0;
unsigned char s8Key_1 = 0;
unsigned char ldr = 0;
bit ldr_flag = 0;
bit ldr_flag1 = 0;
bit L1 = 1;
bit L2 = 1;
bit L3 = 1;
bit L4 = 1;
bit L5 = 1;
bit L6 = 1;
bit timeFlag = 0;
unsigned char LED = 0xff,LED_Temp = 0xff;
unsigned long count = 0,sum = 0;
void DS1302_Init(void)
{
	unsigned char i;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i = 0;i < 7;i ++)
	{
		Write_Ds1302_Byte(Write_DS1302_Code[i],DS1302_Time[i]);
	}
	Write_Ds1302_Byte(0x8e,0x8e);
}

void DS1302_Read(void)
{
	unsigned char i;
	for(i = 0;i < 7;i ++)
	{
		DS1302_Time[i] = Read_Ds1302_Byte(Read_DS1302_Code[i]);
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
			P2 = (P2 & 0x1f)|0xA0;
			P2 = P2 & 0x1f;
		break;
		case 6:
			P2 = (P2 & 0x1f)|0xC0;
			P2 = P2 & 0x1f;
		break;
		case 7:
			P2 = (P2 & 0x1f)|0xE0;
			P2 = P2 & 0x1f;
		break;
	}
}

void smgDisplay(unsigned char m,unsigned char n)
{
	P0 = 0x01 << m;
	SelectHC573(6);
	P0 = n;
	SelectHC573(7);
}

void Display_Time()
{
	smgDisplay(0,Seg_Table[DS1302_Time[2]/16]);
	Delay(500);
	smgDisplay(1,Seg_Table[DS1302_Time[2]%16]);
	Delay(500);
	smgDisplay(2,0xbf);//1011 1111
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
	Delay(500);
}

void Display_Distance()
{
	smgDisplay(0,0xc7);//L 1100 0111
	Delay(500);
	smgDisplay(1,Seg_Table[Display_Distance_Mode]);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,Seg_Table[distance / 100 % 10]);
	Delay(500);
	smgDisplay(6,Seg_Table[distance / 10 % 10]);
	Delay(500);
	smgDisplay(7,Seg_Table[distance % 10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_DistanceMax()
{
	smgDisplay(0,0x89);//H 1000 1001
	Delay(500);
	smgDisplay(1,0xfe);//1111 1110
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,Seg_Table[distanceMax/100%10]);
	Delay(500);
	smgDisplay(6,Seg_Table[distanceMax/10%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[distanceMax%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_DistanceMin()
{
	smgDisplay(0,0x89);//H 1000 1001
	Delay(500);
	smgDisplay(1,0xf7);//1111 0111
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,Seg_Table[distanceMin/100%10]);
	Delay(500);
	smgDisplay(6,Seg_Table[distanceMin/10%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[distanceMin%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_DistanceAvg()
{
	smgDisplay(0,0x89);////H 1000 1001
	Delay(500);
	smgDisplay(1,0xbf);//1011 1111
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,Seg_Table[distanceAvg/10%10]);
	Delay(500);
	smgDisplay(5,Seg_Table[distanceAvg/10%10]);
	Delay(500);
	smgDisplay(6,Seg_Table_Dot[distanceAvg/10%10]);
	Delay(500);
	smgDisplay(7,Seg_Table[distanceAvg%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_Config1()
{
	smgDisplay(0,0x8c);////P 1000 1100
	Delay(500);
	smgDisplay(1,Seg_Table[1]);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,Seg_Table[0]);
	Delay(500);
	smgDisplay(7,Seg_Table[gatherTime%10]);
	Delay(500);
	P0 = 0xff;
	SelectHC573(6);
	P0 = 0xff;
	SelectHC573(7);
	Delay(500);
}

void Display_Config2()
{
	smgDisplay(0,0x8c);////P 1000 1100
	Delay(500);
	smgDisplay(1,Seg_Table[2]);
	Delay(500);
	smgDisplay(2,0xff);
	Delay(500);
	smgDisplay(3,0xff);
	Delay(500);
	smgDisplay(4,0xff);
	Delay(500);
	smgDisplay(5,0xff);
	Delay(500);
	smgDisplay(6,Seg_Table[gatherDistance/10]);
	Delay(500);
	smgDisplay(7,Seg_Table[gatherDistance%10]);
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
	if(row3 == 0)//s5
	{
		Delay(500);
		if(row3 == 0)
		{
			while(row3 == 0);
			if(s4Key == 0)
			{
				if(s5Key_0 == 0)
				{
					s5Key_0 = 1;
				}
				else if(s5Key_0 == 1)
				{
					s5Key_0 = 2;
					s8Key_1 = 0;
				}
				else if(s5Key_0 == 2)
				{
					s5Key_0 = 0;
				}
			}
			else
			{
				if(s5Key_1 == 0)
				{
					s5Key_1 = 1;
				}
				else if(s5Key_1 == 1)
				{
					s5Key_1 = 0;
				}
			}
		}
	}
	
	if(row4 == 0)//s4
	{
		Delay(500);
		if(row4 == 0)
		{
			while(row4 == 0);
			if(s4Key == 0)
			{
				s4Key = 1;
				s5Key_1 = 0;
			}
			else
			{
				s4Key = 0;
				s5Key_0 = 0;
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
			if(s5Key_1 == 0)
			{
				if(gatherTime == 2)
				{
					gatherTime = 3;
				}
				else if(gatherTime == 3)
				{
					gatherTime = 5;
				}
				else if(gatherTime == 5)
				{
					gatherTime = 7;
				}
				else if(gatherTime == 7)
				{
					gatherTime = 9;
				}
				else if(gatherTime == 9)
				{
					gatherTime = 2;
				}
			}
			else if(s5Key_1 == 1)
			{
				gatherDistance += 10;
				if(gatherDistance > 90)
				{
					gatherDistance = 10;
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
			if(s5Key_0 == 1)
			{
				if(s8Key_0 == 0)
				{
					s8Key_0 = 1;
				}
				else
				{
					s8Key_0 = 0;
				}
			}
			else if(s5Key_0 == 2)
			{
				if(s8Key_1 == 0)
				{
					s8Key_1 = 1;
				}
				else if(s8Key_1 == 1)
				{
					s8Key_1 = 2;
				}
				else 
				{
					s8Key_1 = 0;
				}
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



void Ut_Wave_Init()
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

unsigned int Ut_Wave_Data() //超声波距离读取函数
{
    unsigned int time;//时间储存变量
    TMOD &=  0x0F;//配置PCA工作模式
    TH1 = TL1 = 0;//复位计数值 等待超声波信号发出
    Ut_Wave_Init();//发送超声波信号
    TR1 = 1;//开始计时
    while((Rx == 1) && (TF1 == 0));//等待接受返回信号或者定时器溢出
    TR1 = 0;//停止计时
    if(TF1 == 0) //定时器没有溢出
    {
        time = TH1 << 8 | TL1;//读取当前时间
			if(firstTest == 0)
			{
				firstTest = 1;
				distance = time * 0x017;
				distanceMax = distance;
				distanceMin = distance;
			}
        return (time * 0.017);//返回距离值
    }
    else
    {
        TF1 = 0;//清除溢出标志位
        return 0;
    }
}

void LED_Fun()
{
	if(s5Key_0 == 0&&s4Key == 0)
	{
		L1 = 0;
		L2 = 1;
		L3 = 1;
	}
	else if(s5Key_0 == 1&&s4Key == 0)
	{
		L1 = 1;
		L2 = 0;
		L3 = 1;
	}
	else if(s5Key_0 == 2&&s4Key == 0)
	{
		L1 = 1;
		L2 = 1;
		L3 = 0;
	}
	else
	{
		L1 = 1;
		L2 = 1;
		L3 = 1;
	}
	if(s8Key_0 == 0)
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
	DS1302_Init();
	while(1)
	{
		if(s4Key == 0)
		{
			if(s5Key_0 == 0)
			{
				 Display_Time();
			}
			else if(s5Key_0 == 1)
			{
				Display_Distance();
			}
			else if(s5Key_0 == 2)
			{
				if(s8Key_1 == 0)
				{
					Display_DistanceMax();
				}
				else if(s8Key_1 == 1)
				{
					Display_DistanceMin();
				}
				else if(s8Key_1 == 2)
				{
					Display_DistanceAvg();
				}
			}
		}
		else if(s4Key == 1)
		{
			if(s5Key_1 == 0)
			{
				Display_Config1();
			}
			else if(s5Key_1 == 1)
			{
				Display_Config2();
			}
		}
		DS1302_Read();
		if(distance > 80)
		{
			PCF8591_PUTVoltage(0,255);
		}
		else if(distance <10)
		{
			PCF8591_PUTVoltage(0,51);
		}
		else
		{
			PCF8591_PUTVoltage(0,102*distance/35+153/7);
		}
		ldr = PCF8591_GETVoltage(1);
		if(ldr < 128)
		{
			L6 = 1;
			if(ldr_flag1 == 0)
			{
				ldr_flag = 1;
				ldr_flag1 = 1;
			}
		}
		else
		{
			L6 = 0;
			if(ldr_flag1 == 1)
			{
				ldr_flag = 1;
				ldr_flag1 = 0;
			}
		}
		if(ldr_flag == 1&&s8Key_0 == 0)
		{
			//触发一次
			distance = Ut_Wave_Data();
			count ++;
			sum += distance;
			distanceAvg = (sum*10)/count;
			ldr_flag = 0;
		}
		if((DS1302_Time[0]/16+DS1302_Time[0]%16)%gatherTime == 0&&s8Key_0 == 1)
		{
			if(timeFlag == 0)
			{
				timeFlag = 1;
				//触发一次
				distance = Ut_Wave_Data();
				count ++;
				sum += distance;
				distanceAvg = (sum*10)/count;
			}
		}
		else
		{
			timeFlag = 0;
		}
		Key();
		LED_Fun();
		if(distance > distanceMax)
		{
			distanceMax = distance;
		}
		if(distance < distanceMin)
		{
			distanceMin = distance;
		}
	}
}