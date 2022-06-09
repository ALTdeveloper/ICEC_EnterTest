void UartInit(void)//���ڳ�ʼ��9600bps@11.0592MHz
{
	PCON &= 0x7F;//�����ʲ�����
	SCON = 0x50;//8λ���ݿɱ䲨����
	AUXR |= 0x04;//���������ʷ�����ʱ��λFosc����1T
	BRT = 0xDC;//�趨���������ʷ�������װֵ
	AUXR |= 0x01;//����1ѡ����������ʷ�������Ϊ�����ʷ�����
	AUXR |= 0x10;//�������������ʷ�����
	
	EA = 1;
	ES = 1;
}

void Timer0Init(void)//�Զ�ʱ��0��ʼ��
{
	TMOD = 0x02;//���ö�ʱ��0Ϊ��λ�Զ���װ
	TH0 = 0x00;//�߰�λ�Ĵ�����ֵ(�����ӳ�ʱ��)
	TL0 = 0x00;//�Ͱ�λ�Ĵ���װ��
	TF0 = 0;//�ж�λ����
	ET0 = 1;//����ʱ��0ʹ���ж�
	TR0 = 1;//�����俪ʼ��ʱ
}

void Timer1Init(void)//�Զ�ʱ��1��ʼ��
{
	TMOD |= 0x20;//���ö�ʱ��1Ϊ��λ�Զ���װ
	TH1 = 0x00;//�߰�λ�Ĵ�����ֵ(�����ӳ�ʱ��)
	TL1 = 0x00;//�Ͱ�λ�Ĵ���װ��
	TF1 = 0;//�ж�λ����
	ET1 = 1;//����ʱ��1ʹ���ж�
	TR1 = 0;//��ʱ�������俪ʼ��ʱ
}

void oled_flash(unsigned char work)//ˢ��oled
{
	switch(work)
	{
		case 0 ://ȫˢ
		{
			
			OLED_Clear();
			OLED_ShowNum(0,0,time[0],2,16);//��ʾʱ��
			OLED_ShowChar(20,0,':',16);
			OLED_ShowNum(25,0,time[1],2,16);
			OLED_ShowChar(41,0,':',16);
			OLED_ShowNum(56,0,time[2],2,16);
		
			OLED_ShowCHinese(0,2,1);//��
			OLED_ShowCHinese(16,2,2);//ʱ
			OLED_ShowCHinese(32,2,3);//��
			OLED_ShowCHinese(0,5,0);//��
			OLED_ShowCHinese(16,5,2);//ʱ
			OLED_ShowCHinese(32,5,3);//��
			
			OLED_ShowCHinese(105,0,4);//ͼ��
			break;
		}
		case 1://ˢ��ʱ��
		{
			OLED_ShowNum(0,0,time[0],2,16);//��ʾʱ��
			OLED_ShowChar(20,0,':',16);
			OLED_ShowNum(25,0,time[1],2,16);
			OLED_ShowChar(41,0,':',16);
			OLED_ShowNum(56,0,time[2],2,16);
			break;
		}
		case 2://ֻˢ������ʱ
		{
			OLED_ShowNum(50,2,count[0],2,16);//��ʱ��ʱ��ˢ��
			OLED_ShowChar(66,2,':',16);
			OLED_ShowNum(75,2,count[1],2,16);
		
			OLED_ShowString(55,5,"     ",16);
			break;
		}
		case 3://ֻˢ�µ���ʱ
		{
			OLED_ShowString(55,2,"     ",16);
		
			OLED_ShowNum(50,5,count[0],2,16);//��ʱ��ʱ��ˢ��
			OLED_ShowChar(66,5,':',16);
			OLED_ShowNum(75,5,count[1],2,16);
			break;
		}
	}
}

void oled_set_flash(void)//set״̬��ˢ��oled
{
	unsigned char i=0;
	OLED_Clear();
	OLED_ShowString(20,4,"Set Time?",16);
	//�������������forʵ�ֵģ�����չ������ʡ����ʱ�䣬��ȷ
	OLED_ShowNum(0,0,time[0],2,16);//��ʾʱ��
	OLED_ShowChar(20,0,':',16);
	
	OLED_ShowNum(25,0,time[1],2,16);
	OLED_ShowChar(41,0,':',16);
	
	OLED_ShowNum(56,0,time[2],2,16);
}

void countdown_reset(void)
{
	oled_flash(3);//��ʱ��״̬ˢ��
	while(1)//�ȴ���������
	{
		if(key1==0)//�����ӳټ�
		{
			delay_ms(10);
			if(key1==0)
			{
				while(key1==0);
				count[1]++;
				if(count[1]==60&&count[0]<60)//����Ҫ������ӳ���60
				{
					count[0]++;//ʱ���λ
				}
				count[1]%=60;//ÿ60�͹�0
				//ˢ��oled
				oled_flash(3);//��ʱ��״̬ˢ��
			}
		}
		if(key2==0)//�����ӳټ�
		{
			delay_ms(10);
			if(key2==0)
			{
				while(key2==0);
				if(count[1]>0)
				{
					count[1]--;//����С��0
				}
				count[1]%=60;
				//ˢ��oled
				oled_flash(3);//��ʱ��״̬ˢ��
			}
		}
		if(key3==0)//�����������
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
	while(1)//�ȴ�������������
	{
		if(key1==0)//����ʱ���
		{
			delay_ms(10);
			if(key1==0)
			{
				while(key1==0);
				time[2]++;
				if(time[2]==60)//��λ����
				{
					time[2]=0;
					time[1]++;
				}
				if(time[1]==60)//��λ����
				{
					time[1]=0;
					time[0]++;
				}
				if(time[0]==24)//Сʱλ����
				{
					time[0]=0;
					time[1]=0;
					time[2]=0;//ȫ������
				}
				oled_set_flash();//ˢ����ʾ
			}
		}
		if(key2==0)//����ʱ���
		{
			delay_ms(10);
			if(key2==0)
			{
				while(key2==0);
				if(time[0]==0 && time[1]==0 && time[2]==0)//Сʱλ0��
				{
					time[0]=23;
					time[1]=59;
					time[2]=60;//ȫ������
				}
				else if(time[1]==0 && time[2]==0)//��λ0��
				{
					time[2]=60;
					time[1]=59;
					time[0]--;
				}
				else if(time[2]==0)//��λ0��
				{
					time[2]=60;
					time[1]--;
				}
				time[2]--;//������ȼ���Ƿ���0����Ϊû��24��60��
				oled_set_flash();//ˢ����ʾ
			}
		}
		if(key3==0)//�����������
		{
			delay_ms(10);
			if(key3==0)
			{
				while(key3==0);
				oled_flash(0);//ˢ����ʾ
				break;
			}
		}
	}
}