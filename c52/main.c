#include "reg52.h"

unsigned char recdat =0 ,FLAG=0 ;
int recdatanum = 0 , sendflag=0;
int  disp[8];
sbit RS=P3^3;
sbit RW=P1^0;
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
	TMOD=0x20;
	TH1=256-1;
	TL1=256-1;
	ES=1;
	EA=1;
	TR1=1;
}

void senddat()
{
	disp[recdatanum]=(int)recdat;
	recdatanum++;
	if(recdatanum == 8)
	{
		recdatanum=0;
		sendflag=1;
	}
	
	SBUF=recdat;
	while(!TI);
	TI=0;
}

void writedat(unsigned int dat)
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
	writecom(0x80);
	delay(5);
	for(num=0;num<8;num++){
	writedat(disp[num]);
	delay(5);
	}
	writedat('H');
	delay(5);
	writedat('Z');
	delay(5);
	writedat('2');
	delay(5);
}

void main()
{
	initlcd();
	initscon();
	display();
	while(1)
	{
		if(sendflag==1){
			display();
			sendflag=0;
		}
		if(FLAG==1){
			senddat();
			FLAG=0;
		}
	}
}


void scon_isr() interrupt 4
{
	recdat=SBUF;
	RI=0;
	FLAG=1;
}
