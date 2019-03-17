#include <reg51.h>
#include <intrins.h>	  		//" _nop_(); "要用到此头文件
#include "LCD.h"

#define uint unsigned int
#define uchar unsigned char

sbit servo = P3^7;		  		//舵机控制端口
sbit time_up = P3^0;	  		//按键控制定时时间增
sbit time_down = P3^1;	  		//按键控制定时时间减
sbit hand_control = P3^3;		//按键控制舵机转动

uchar code time_show[]="TIMER_TIME=";		  //1602显示数据

uchar  period,rate;			 //	period：输出脉宽的周期,rate:舵机输出角度位置
uint t0,jishu;				 //t0：定时器 T1 计时变量，jishu:控制舵机转动角度变量
uint time=1;				 //定时时间
bit runservo_flag=0;		 //让舵机开始转的标志位
uint max_rate=15;			 //改变舵机输出位置变量
/*
max_rate=5——0度;
max_rate=10——45度;
max_rate=15——90度;
max_rate=20——135度;
max_rate=22——180度；
*/
////////////////////////////
void delayms(uint z);	   	//延时z毫秒
void handcontrol_servo();	 	//按键控制舵机转动
void set_time();
void timer_init();				//定时器初始化

void main()
{	
	uint i,shi,ge;	
	
	timer_init();
	LcdInit();
	rate = max_rate;       		 //设置舵机初始位置
	delayms(600);

	while(1)
	{
		 handcontrol_servo();	 //手动控制喂鱼
		 set_time();			 //定时时间设置	

//--------------------------------舵机正反转-----------------------------//	
					
		if(jishu==1)			 //一秒钟jishu增一次，来改变舵机输出角度位置，实现舵机正反转
		{
			max_rate=5;			 //舵机输出角度位置：  0  度
		}
		if(jishu==2)			  
		{
			max_rate=25;		 //舵机输出角度位置：	180 度
		}	
		TR0=1;				 	//开定时器T0
		rate=max_rate;  		//转动舵机
		delayms(600);			//延时（控制转动快慢）

//---------------------------1602显示定时时间--------------------------//			  	   
		
		shi = time/10;
		ge = time%10;

		LcdWriteCom(0x80);			 //1602显示	"TIMER_TIME=xx min"
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


		TR0=0;						  	//关定时器 T0，结合 "while(!runservo_flag){}；" 使舵机停止转动
		servo=0;					  	//数据口拉低

		while(!runservo_flag)		  	//若runservo标志为0 ，使舵机不转动
		{
			handcontrol_servo();	 	//手动控制喂鱼		  
			set_time();				 	//定时时间设置		 //这两句再写一遍是使舵机不转时，也能检测按键是否按下或1602显示
																		
			shi = time/10;
			ge = time%10;
	
			LcdWriteCom(0x80);			 //1602显示	"TIMER_TIME=xx min"
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
		runservo_flag=0;	   			//为下次舵机转动做准备
	}//while(1)	
}
//
void delayms(uint z)	   	//延时z毫秒
{	 uint x,y;
   for(x=1;x<118;x++)
   for(y=1;y<z;y++);
}

void handcontrol_servo()	 	//按键控制舵机转动
{
	if(hand_control==0)		 	//判断是否按下
	{		
		delayms(30);			//延时10ms,去掉抖动部分
		if(hand_control==0)		//确认已经按下
		{
			 runservo_flag = 1;				//servo标志置1 ，舵机开始转动
		}
		while (!hand_control);	   	//等待松手	
	}
}

void set_time()
{

	if(time_up==0)		 		//判断是否按下
	{		
		delayms(10);			//延时10ms,去掉抖动部分
		if(time_up==0)			//确认已经按下
			{
				time++;			//定时时间增
			}
		while(!time_up);	  	//等待松手	
	}

	if(time_down==0)		 	//判断是否按下
	{		
		delayms(10);			//延时10ms,去掉抖动部分
		if(time_down==0)		//确认已经按下
			{
				time--;			//定时时间减
			}
		while(!time_down);	 	//等待松手
	}

	if(time<0) time=0;
}
void timer_init()				//定时器初始化
{
	TMOD = 0x11;				//定时器 T0 ，T1 都设为工作方式 1

	TH0=(65535-100)/256;        //定时器 T0 ，T1 装初值   
	TL0=(65535-100)%256;		//100表示100us

	TH1=(65536-50000)/256;   	//50000表示50ms
	TL1=(65536-50000)%256;

	EA = 1;				   //开总中断
	ET0 = 1;			   //开定时器T0中断
	ET1 = 1;			   //开定时器T1中断
	TR1 = 1;			   //开定时器T1
}


void maikuan() interrupt 1	   			//用定时器 T0 输出脉宽来控制舵机
{ 
	TH0=(65535-100)/256;      			//重装初值             
	TL0=(65535-100)%256; 	                                                               
	period++; 
	if(period>200)  period=0;	 		//设置脉宽周期为20MS
	if(period<rate)  servo=1; 	 		//输出一定时间的占空比脉宽
	else  servo=0;
}

void timer() interrupt 3				//用定时器 T1 计时		 
{  
	uint j,sec;
	TH1=(65536-50000)/256;   
	TL1=(65536-50000)%256;	
	t0++;	
	if(t0==20)						   		//1s 进一次中断
		{
			t0 = 0;									
			jishu++;
			j++;			
			if(j>=60*time)					//（60*time）s 既 time 分钟定时时间到
			{							 	
				runservo_flag = 1;			//runservo标志置1 ，使舵机开始转动
				sec++;						//秒计时来控制舵机转动时转动次数
				if(sec > 10)				//转动10s
				{
					runservo_flag = 0;		//标志清零
					sec = 0;				//秒计时变量清零
					j = 0;					//计时变量清零
				}						
			}
			if(jishu==3) jishu=0;			//选择舵机输出角度位置 						
		} 
}
