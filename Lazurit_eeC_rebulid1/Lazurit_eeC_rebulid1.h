void UartInit(void)//串口初始化9600bps@11.0592MHz
{
	PCON &= 0x7F;//波特率不倍速
	SCON = 0x50;//8位数据可变波特率
	AUXR |= 0x04;//独立波特率发射器时钟位Fosc，即1T
	BRT = 0xDC;//设定独立波特率发生器重装值
	AUXR |= 0x01;//串口1选择独立波特率发生器作为波特率发生器
	AUXR |= 0x10;//启动独立波特率发生器
	
	EA = 1;
	ES = 1;
}

void Timer0Init(void)//对定时器0初始化
{
	TMOD = 0x02;//设置定时器0为八位自动重装
	TH0 = 0x00;//高八位寄存器初值(决定延迟时常)
	TL0 = 0x00;//低八位寄存器装填
	TF0 = 0;//中断位置零
	ET0 = 1;//允许定时器0使用中断
	TR0 = 1;//允许其开始计时
}

void Timer1Init(void)//对定时器1初始化
{
	TMOD |= 0x20;//设置定时器1为八位自动重装
	TH1 = 0x00;//高八位寄存器初值(决定延迟时常)
	TL1 = 0x00;//低八位寄存器装填
	TF1 = 0;//中断位置零
	ET1 = 1;//允许定时器1使用中断
	TR1 = 0;//暂时不允许其开始计时
}

void oled_flash(unsigned char work)//刷新oled
{
	switch(work)
	{
		case 0 ://全刷
		{
			
			OLED_Clear();
			OLED_ShowNum(0,0,time[0],2,16);//显示时间
			OLED_ShowChar(20,0,':',16);
			OLED_ShowNum(25,0,time[1],2,16);
			OLED_ShowChar(41,0,':',16);
			OLED_ShowNum(56,0,time[2],2,16);
		
			OLED_ShowCHinese(0,2,1);//计
			OLED_ShowCHinese(16,2,2);//时
			OLED_ShowCHinese(32,2,3);//器
			OLED_ShowCHinese(0,5,0);//定
			OLED_ShowCHinese(16,5,2);//时
			OLED_ShowCHinese(32,5,3);//器
			
			OLED_ShowCHinese(105,0,4);//图标
			break;
		}
		case 1://刷新时间
		{
			OLED_ShowNum(0,0,time[0],2,16);//显示时间
			OLED_ShowChar(20,0,':',16);
			OLED_ShowNum(25,0,time[1],2,16);
			OLED_ShowChar(41,0,':',16);
			OLED_ShowNum(56,0,time[2],2,16);
			break;
		}
		case 2://只刷新正计时
		{
			OLED_ShowNum(50,2,count[0],2,16);//计时器时间刷新
			OLED_ShowChar(66,2,':',16);
			OLED_ShowNum(75,2,count[1],2,16);
		
			OLED_ShowString(55,5,"     ",16);
			break;
		}
		case 3://只刷新倒计时
		{
			OLED_ShowString(55,2,"     ",16);
		
			OLED_ShowNum(50,5,count[0],2,16);//定时器时间刷新
			OLED_ShowChar(66,5,':',16);
			OLED_ShowNum(75,5,count[1],2,16);
			break;
		}
	}
}

void oled_set_flash(void)//set状态下刷新oled
{
	unsigned char i=0;
	OLED_Clear();
	OLED_ShowString(20,4,"Set Time?",16);
	//本来这里可以用for实现的，但是展开来能省运行时间，精确
	OLED_ShowNum(0,0,time[0],2,16);//显示时间
	OLED_ShowChar(20,0,':',16);
	
	OLED_ShowNum(25,0,time[1],2,16);
	OLED_ShowChar(41,0,':',16);
	
	OLED_ShowNum(56,0,time[2],2,16);
}

void countdown_reset(void)
{
	oled_flash(3);//定时器状态刷新
	while(1)//等待按键操作
	{
		if(key1==0)//控制延迟加
		{
			delay_ms(10);
			if(key1==0)
			{
				while(key1==0);
				count[1]++;
				if(count[1]==60&&count[0]<60)//这里要避免分钟超过60
				{
					count[0]++;//时间进位
				}
				count[1]%=60;//每60就归0
				//刷新oled
				oled_flash(3);//定时器状态刷新
			}
		}
		if(key2==0)//控制延迟减
		{
			delay_ms(10);
			if(key2==0)
			{
				while(key2==0);
				if(count[1]>0)
				{
					count[1]--;//避免小于0
				}
				count[1]%=60;
				//刷新oled
				oled_flash(3);//定时器状态刷新
			}
		}
		if(key3==0)//跳出这个设置
		{
			delay_ms(10);
			if(key3==0)
			{
				while(key3==0);
				break;
			}
		}
	}
}

void time_set(void)
{
	oled_set_flash();
	while(1)//等待按键操作结束
	{
		if(key1==0)//控制时间加
		{
			delay_ms(10);
			if(key1==0)
			{
				while(key1==0);
				time[2]++;
				if(time[2]==60)//秒位满了
				{
					time[2]=0;
					time[1]++;
				}
				if(time[1]==60)//分位满了
				{
					time[1]=0;
					time[0]++;
				}
				if(time[0]==24)//小时位满了
				{
					time[0]=0;
					time[1]=0;
					time[2]=0;//全部归零
				}
				oled_set_flash();//刷新显示
			}
		}
		if(key2==0)//控制时间减
		{
			delay_ms(10);
			if(key2==0)
			{
				while(key2==0);
				if(time[0]==0 && time[1]==0 && time[2]==0)//小时位0了
				{
					time[0]=23;
					time[1]=59;
					time[2]=60;//全部回满
				}
				else if(time[1]==0 && time[2]==0)//分位0了
				{
					time[2]=60;
					time[1]=59;
					time[0]--;
				}
				else if(time[2]==0)//秒位0了
				{
					time[2]=60;
					time[1]--;
				}
				time[2]--;//这里得先检测是否是0，因为没有24点60分
				oled_set_flash();//刷新显示
			}
		}
		if(key3==0)//跳出这个设置
		{
			delay_ms(10);
			if(key3==0)
			{
				while(key3==0);
				oled_flash(0);//刷新显示
				break;
			}
		}
	}
}