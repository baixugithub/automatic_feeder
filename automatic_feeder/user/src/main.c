#include <reg51.h>
#include <intrins.h>	  		//" _nop_(); "Ҫ�õ���ͷ�ļ�
#include "LCD.h"

#define uint unsigned int
#define uchar unsigned char

sbit servo = P3^7;		  		//������ƶ˿�
sbit time_up = P3^0;	  		//�������ƶ�ʱʱ����
sbit time_down = P3^1;	  		//�������ƶ�ʱʱ���
sbit hand_control = P3^3;		//�������ƶ��ת��

uchar code time_show[]="TIMER_TIME=";		  //1602��ʾ����

uchar  period,rate;			 //	period��������������,rate:�������Ƕ�λ��
uint t0,jishu;				 //t0����ʱ�� T1 ��ʱ������jishu:���ƶ��ת���Ƕȱ���
uint time=1;				 //��ʱʱ��
bit runservo_flag=0;		 //�ö����ʼת�ı�־λ
uint max_rate=15;			 //�ı������λ�ñ���
/*
max_rate=5����0��;
max_rate=10����45��;
max_rate=15����90��;
max_rate=20����135��;
max_rate=22����180�ȣ�
*/
////////////////////////////
void delayms(uint z);	   	//��ʱz����
void handcontrol_servo();	 	//�������ƶ��ת��
void set_time();
void timer_init();				//��ʱ����ʼ��

void main()
{	
	uint i,shi,ge;	
	
	timer_init();
	LcdInit();
	rate = max_rate;       		 //���ö����ʼλ��
	delayms(600);

	while(1)
	{
		 handcontrol_servo();	 //�ֶ�����ι��
		 set_time();			 //��ʱʱ������	

//--------------------------------�������ת-----------------------------//	
					
		if(jishu==1)			 //һ����jishu��һ�Σ����ı�������Ƕ�λ�ã�ʵ�ֶ������ת
		{
			max_rate=5;			 //�������Ƕ�λ�ã�  0  ��
		}
		if(jishu==2)			  
		{
			max_rate=25;		 //�������Ƕ�λ�ã�	180 ��
		}	
		TR0=1;				 	//����ʱ��T0
		rate=max_rate;  		//ת�����
		delayms(600);			//��ʱ������ת��������

//---------------------------1602��ʾ��ʱʱ��--------------------------//			  	   
		
		shi = time/10;
		ge = time%10;

		LcdWriteCom(0x80);			 //1602��ʾ	"TIMER_TIME=xx min"
		for(i=0;i<11;i++)
			LcdWriteData(time_show[i]);
		LcdWriteCom(0x8b);
		LcdWriteData(0x30+shi);
		LcdWriteCom(0x8c);
		LcdWriteData(0x30+ge);

		LcdWriteCom(0x8d);
		LcdWriteData('m');
		LcdWriteCom(0x8e);
		LcdWriteData('i');
		LcdWriteCom(0x8f);
		LcdWriteData('n');


		TR0=0;						  	//�ض�ʱ�� T0����� "while(!runservo_flag){}��" ʹ���ֹͣת��
		servo=0;					  	//���ݿ�����

		while(!runservo_flag)		  	//��runservo��־Ϊ0 ��ʹ�����ת��
		{
			handcontrol_servo();	 	//�ֶ�����ι��		  
			set_time();				 	//��ʱʱ������		 //��������дһ����ʹ�����תʱ��Ҳ�ܼ�ⰴ���Ƿ��»�1602��ʾ
																		
			shi = time/10;
			ge = time%10;
	
			LcdWriteCom(0x80);			 //1602��ʾ	"TIMER_TIME=xx min"
			for(i=0;i<11;i++)
				LcdWriteData(time_show[i]);
			LcdWriteCom(0x8b);
			LcdWriteData(0x30+shi);
			LcdWriteCom(0x8c);
			LcdWriteData(0x30+ge);
	
			LcdWriteCom(0x8d);
			LcdWriteData('m');
			LcdWriteCom(0x8e);
			LcdWriteData('i');
			LcdWriteCom(0x8f);
			LcdWriteData('n');

		}
		runservo_flag=0;	   			//Ϊ�´ζ��ת����׼��
	}//while(1)	
}
//
void delayms(uint z)	   	//��ʱz����
{	 uint x,y;
   for(x=1;x<118;x++)
   for(y=1;y<z;y++);
}

void handcontrol_servo()	 	//�������ƶ��ת��
{
	if(hand_control==0)		 	//�ж��Ƿ���
	{		
		delayms(30);			//��ʱ10ms,ȥ����������
		if(hand_control==0)		//ȷ���Ѿ�����
		{
			 runservo_flag = 1;				//servo��־��1 �������ʼת��
		}
		while (!hand_control);	   	//�ȴ�����	
	}
}

void set_time()
{

	if(time_up==0)		 		//�ж��Ƿ���
	{		
		delayms(10);			//��ʱ10ms,ȥ����������
		if(time_up==0)			//ȷ���Ѿ�����
			{
				time++;			//��ʱʱ����
			}
		while(!time_up);	  	//�ȴ�����	
	}

	if(time_down==0)		 	//�ж��Ƿ���
	{		
		delayms(10);			//��ʱ10ms,ȥ����������
		if(time_down==0)		//ȷ���Ѿ�����
			{
				time--;			//��ʱʱ���
			}
		while(!time_down);	 	//�ȴ�����
	}

	if(time<0) time=0;
}
void timer_init()				//��ʱ����ʼ��
{
	TMOD = 0x11;				//��ʱ�� T0 ��T1 ����Ϊ������ʽ 1

	TH0=(65535-100)/256;        //��ʱ�� T0 ��T1 װ��ֵ   
	TL0=(65535-100)%256;		//100��ʾ100us

	TH1=(65536-50000)/256;   	//50000��ʾ50ms
	TL1=(65536-50000)%256;

	EA = 1;				   //�����ж�
	ET0 = 1;			   //����ʱ��T0�ж�
	ET1 = 1;			   //����ʱ��T1�ж�
	TR1 = 1;			   //����ʱ��T1
}


void maikuan() interrupt 1	   			//�ö�ʱ�� T0 ������������ƶ��
{ 
	TH0=(65535-100)/256;      			//��װ��ֵ             
	TL0=(65535-100)%256; 	                                                               
	period++; 
	if(period>200)  period=0;	 		//������������Ϊ20MS
	if(period<rate)  servo=1; 	 		//���һ��ʱ���ռ�ձ�����
	else  servo=0;
}

void timer() interrupt 3				//�ö�ʱ�� T1 ��ʱ		 
{  
	uint j,sec;
	TH1=(65536-50000)/256;   
	TL1=(65536-50000)%256;	
	t0++;	
	if(t0==20)						   		//1s ��һ���ж�
		{
			t0 = 0;									
			jishu++;
			j++;			
			if(j>=60*time)					//��60*time��s �� time ���Ӷ�ʱʱ�䵽
			{							 	
				runservo_flag = 1;			//runservo��־��1 ��ʹ�����ʼת��
				sec++;						//���ʱ�����ƶ��ת��ʱת������
				if(sec > 10)				//ת��10s
				{
					runservo_flag = 0;		//��־����
					sec = 0;				//���ʱ��������
					j = 0;					//��ʱ��������
				}						
			}
			if(jishu==3) jishu=0;			//ѡ��������Ƕ�λ�� 						
		} 
}
