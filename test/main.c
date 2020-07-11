#include "reg52.h"
#include "stdio.h"

unsigned char recdat =0 ,FLAG=0 , Flag_fre=0;
//unsigned char disp[8] = "";
unsigned int disp[8];
int T0count=0,T1count=0;
unsigned int Value =0;


sbit RS=P3^3;
sbit RW=P1^2;
sbit E=P3^2;

void delay(unsigned int t)
{
	unsigned int i =0 , j = 0;
	for(i=0;i<t;i++)
	{
		for(j=0;j<120;j++){
		}
	}
}

void initscon()
{
	SCON=0x50;
	TMOD|=0x20;
	TH1 = 256-1;
//	TH1=256-1;
//	TL1=256-1;
//	ES=1;
//	ET1=1;
//	EA=1;
	TR1=1;
}


void senddat(unsigned char dat)
{
	SBUF = dat;
	//SBUF = (dat+'0');
	while(!TI);
	TI = 0;
}






//void sendstrdat()
//{
//	int num = 0;
//	for(num=0;num<8;num++){
//		recdat = disp[num]+'0';
//		senddat();
//		num++;
//	}
//	
//}

void writedat(unsigned char dat)
{
	RS = 1;
	RW = 0;
	E = 0;
	P2=dat;
	delay(5);
	E=1;
	E=0;
}

void writecom(unsigned int com)
{
	RS = 0;
	RW = 0;
	E = 0;
	P2=com;
	delay(5);
	E=1;
	E=0;
}


void initlcd()
{
	writecom(0x38);
	writecom(0x0c);
	writecom(0x06);
	writecom(0x01);
}

void display()
{
	int num=0;
	writecom(0x80+0x40+2);
//	writecom(0x80);
	delay(5);
	
//	while (disp[num]!='\r')
//	{
//		writedat(disp[num]);
//		delay(5);
//		num++;
//	}
	for(num=0;num<8;num++){
	writedat(disp[num]+'0');
	delay(5);
	}
	
//	senddat(5);
//	senddat(6);
//	senddat(7);
	
//	for(num=0;num<8;num++){
//	senddat((unsigned char)disp[num]);
//	delay(5);
//	}
	

}

void freinit()
{
	TMOD|=0x05;//TIME0计数器，TIME2定时器
	T0count=0;
	T1count=0;
	Flag_fre=0;
	TH0=0;
	TL0=0;
	TH2=(65536-5*221184/12)/256;	//50ms
	TL2=(65536-5*221184/12)%256;	
	T2CON=0;
//	IE=0x50;//10100000
	ET2=1;	//开始中断
	ET0=1;	//开始中断
	TR2=1;	//启动定时器
	TR0=1;
	EA=1;	//开启总中断
	
}

//void sendstrdat(unsigned char *s)
//{
//	int num1 = 0;

//	while(*s!='\0')// 
//	{
//		senddat(*s);
//		s++;
//	}
//}

void calc()
{
	unsigned char i;
	int num = 0;
	long frequency;
//	frequency = T0count*3*500;
	frequency = ((T0count*256+TH0)*256+TL0)/5;

	for(i=7;i>0;i--)
	{
		disp[i]=(int)(frequency%10);
		frequency=frequency/10;
	}
	disp[0]=(int)frequency;
	
	for(num=0;num<=7;num++){
		recdat = (disp[num]+'0');
		senddat(recdat);
		delay(5);
	}
	
	TH0=0;
	TL0=0;

}



void main()
{
	int i=0;
	for(i=0;i<=7;i++)
		disp[i]=0;
	initlcd();
	freinit();
	initscon();
	
//	sendstrdat(disp);
	
	while(1)
	{
		delay(100);
		if(Flag_fre==1){

			calc();
				
			display();
			writedat('H');
			delay(5);
			writedat('Z');
			delay(5);
		
			Flag_fre=0;
		}
	}
}
	

void time0_isr() interrupt 1		//定时器中断0
{
	T0count++;
}

void time1_isr() interrupt 5		//定时器中断2
{
	TF2=0;

//	TH2=(65536-5*50000)/256;		
//	TL2=(65536-5*50000)%256;
	TH2=(65536-5*221184/12)/256;	//50ms
	TL2=(65536-5*221184/12)%256;	
	if(T1count==380)//5s
	{
		Flag_fre=1;
		
		T1count = 0;
		T0count = 0;
		
		
//		freinit();
//		initscon();
	}
	else T1count++;

}

