#include <STC12C5A60S2.H>
#include "oled.h"
//#include "bmp.h"

#define key1 P35
#define key2 P36
#define key3 P37
#define buzzer P00//改这个就要一起把推挽输出口改了

unsigned int time[3]={23,59,55};//存储显示时间
unsigned int count[2]={0};//存储正计时时间
unsigned int counter_type = 0;//正计时器是否在工作
unsigned int work_type = 0;//定时器1是工作在正计时(0)还是倒计时(1)
unsigned char IOtime[7]={'\0'};//存储串口更新时间
unsigned char buf = '\0';
unsigned int io_i = 0;
double buzzer_delay = 0;

#include "Lazurit_eeC_rebulid1.h"


/*****定时器0中断服务程序(普通时间++)*****/
void Timer0_Routine() interrupt 1
{
	static unsigned int timer0 = 0;
	timer0++;
	
	//num of c means delay c ms
	if(timer0>=3610)//溢出3614次是999ms903.45us(应该没算错吧.....)
	{
		timer0 = 0;//归0
		//时间增加
		time[2]++;
		if(time[2]==60)
		{
			time[1]++;
		}
		if(time[1]==60)
		{
			time[0]++;
		}
		time[0]%=24;//避免时分秒超出范围
		time[1]%=60;
		time[2]%=60;
		//刷新显示
		oled_flash(1);//普通刷新
	}
}

/*****定时器1中断服务程序(计时时间++)*****/
void Timer1_Routine() interrupt 3
{
	static unsigned int timer1 = 0;
	timer1++;

	//num of d means delay d ms
	if(timer1>=3610)//溢出3614次是999ms903.45us
	{
		timer1 = 0;//归0
		if(work_type == 0)//计时器
		{
			oled_flash(2);
			count[1]++;
			if(count[1]==60&&count[0]<60)//这里要避免分钟超过60
			{
				count[0]++;//时间进位
			}
			count[1]%=60;//每60就归0
		}
		else if(work_type == 1)//倒计时
		{
			//检测有没有到时间
			if(count[0]==0&&count[1]==0)
			{
				buzzer = 1;//设置蜂鸣器工作
				buzzer_delay = time[0]*3600+time[1]*60+time[2];//蜂鸣器开始工作时间单位秒
				TR1 = 0;//关掉这个硬件定时器1
			}
			else if(count[1]==0)//减少倒计时时间数组(这个必须要分钟位不为0才能这样，所以用elseif)
			{
				count[1]=59;
				count[0]--;
			}
			else
			{
				count[1]--;
			}
			//刷新显示
			oled_flash(3);
		}
	}
}

/********串行中断服务函数***********/
void serial() interrupt 4
{
	ES=0;		//暂时关闭串口中断
	RI=0;
	buf=SBUF;	//把收到的信息从SBUF放到buf中。
/*************对数据进行自定义操作***************/
	IOtime[io_i++]=buf;
	if(io_i == 6)
	{
		io_i = 0;
		time[0] = (IOtime[0]-'0')*10 + (IOtime[1]-'0');
		time[1] = (IOtime[2]-'0')*10 + (IOtime[3]-'0');
		time[2] = (IOtime[4]-'0')*10 + (IOtime[5]-'0');
	}
/****************************/
	ES=1;		//重新开启串口中断
}

int main(void)
{	
	//初始化OLED  
	OLED_Init();
	OLED_Clear();
	
	OLED_ShowString(0,0,"Waiting 8266Init",16);
	OLED_ShowString(0,2,"by Lazurit",16);
	OLED_ShowCHinese(100,3,4);//图标
	OLED_ShowString(0,4,"2021.12.3",16);
	while(!P17);//等8266把p17拉高
	UartInit();//串口初始化
	while(P17);//等8266把p17拉低

	Timer0Init();//对定时器0初始化
	Timer1Init();//对定时器1初始化
	
	//AUXR |= 0x02;//允许使用片上拓展ram,不知道12c5a有没有
	EA = 1;//总中断开
	
	buzzer = 0;//消除上电响一声
	P0M0 = 0xff;//蜂鸣器口推挽输出
	
	oled_flash(0);
	while(1) 
	{		
		//定时器
		if (key1 == 0)
		{
			delay_ms(10);
			if (key1 == 0)
			{
				if(counter_type==0)//没有工作，现在要开始
				{
					work_type = 0;//告诉中断服务函数现在运行计时还是定时
					count[0]=0;count[1]=0;//初始化 count数组
					TR1 = 1;//定时器1启动
					counter_type = !counter_type;//刷新work_type的状态
				}
				else if(counter_type==1)//工作了，现在要停止
				{
					count[0]=0;count[1]=0;//初始化 count数组
					TR1 = 0;//定时器1关闭
					counter_type = !counter_type;//刷新work_type的状态
				}
				while(!key1);
			}
		}
		
		//倒计时器
		if (key2 == 0)
		{
			delay_ms(10);
			if (key2 == 0)
			{
				while(!key2);
				work_type = 1;
				count[0]=0;count[1]=0;//初始化 count数组
				countdown_reset();//进入设置过程
				TR1 = 1;//定时器1启动
			}
		}
		
		if((time[0]*3600+time[1]*60+time[2]) - buzzer_delay >= 6)//蜂鸣器工作6秒就停止
		{
			buzzer = 0;//关闭蜂鸣器
			buzzer_delay = 0;
		}
		
		if (key3 == 0)
		{
			delay_ms(10);
			if (key3 == 0)
			{
				while(!key3);
				TR0 = 0;//关闭定时器0
				TR1 = 0;//关闭定时器1
				time_set();//用新的oled刷新办法，让用户设置时间
				TR0 = 1;//启用定时器0
			}
		}
		
	}
}