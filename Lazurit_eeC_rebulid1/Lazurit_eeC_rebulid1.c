#include <STC12C5A60S2.H>
#include "oled.h"
//#include "bmp.h"

#define key1 P35
#define key2 P36
#define key3 P37
#define buzzer P00//�������Ҫһ�����������ڸ���

unsigned int time[3]={23,59,55};//�洢��ʾʱ��
unsigned int count[2]={0};//�洢����ʱʱ��
unsigned int counter_type = 0;//����ʱ���Ƿ��ڹ���
unsigned int work_type = 0;//��ʱ��1�ǹ���������ʱ(0)���ǵ���ʱ(1)
unsigned char IOtime[7]={'\0'};//�洢���ڸ���ʱ��
unsigned char buf = '\0';
unsigned int io_i = 0;
double buzzer_delay = 0;

#include "Lazurit_eeC_rebulid1.h"


/*****��ʱ��0�жϷ������(��ͨʱ��++)*****/
void Timer0_Routine() interrupt 1
{
	static unsigned int timer0 = 0;
	timer0++;
	
	//num of c means delay c ms
	if(timer0>=3610)//���3614����999ms903.45us(Ӧ��û����.....)
	{
		timer0 = 0;//��0
		//ʱ������
		time[2]++;
		if(time[2]==60)
		{
			time[1]++;
		}
		if(time[1]==60)
		{
			time[0]++;
		}
		time[0]%=24;//����ʱ���볬����Χ
		time[1]%=60;
		time[2]%=60;
		//ˢ����ʾ
		oled_flash(1);//��ͨˢ��
	}
}

/*****��ʱ��1�жϷ������(��ʱʱ��++)*****/
void Timer1_Routine() interrupt 3
{
	static unsigned int timer1 = 0;
	timer1++;

	//num of d means delay d ms
	if(timer1>=3610)//���3614����999ms903.45us
	{
		timer1 = 0;//��0
		if(work_type == 0)//��ʱ��
		{
			oled_flash(2);
			count[1]++;
			if(count[1]==60&&count[0]<60)//����Ҫ������ӳ���60
			{
				count[0]++;//ʱ���λ
			}
			count[1]%=60;//ÿ60�͹�0
		}
		else if(work_type == 1)//����ʱ
		{
			//�����û�е�ʱ��
			if(count[0]==0&&count[1]==0)
			{
				buzzer = 1;//���÷���������
				buzzer_delay = time[0]*3600+time[1]*60+time[2];//��������ʼ����ʱ�䵥λ��
				TR1 = 0;//�ص����Ӳ����ʱ��1
			}
			else if(count[1]==0)//���ٵ���ʱʱ������(�������Ҫ����λ��Ϊ0����������������elseif)
			{
				count[1]=59;
				count[0]--;
			}
			else
			{
				count[1]--;
			}
			//ˢ����ʾ
			oled_flash(3);
		}
	}
}

/********�����жϷ�����***********/
void serial() interrupt 4
{
	ES=0;		//��ʱ�رմ����ж�
	RI=0;
	buf=SBUF;	//���յ�����Ϣ��SBUF�ŵ�buf�С�
/*************�����ݽ����Զ������***************/
	IOtime[io_i++]=buf;
	if(io_i == 6)
	{
		io_i = 0;
		time[0] = (IOtime[0]-'0')*10 + (IOtime[1]-'0');
		time[1] = (IOtime[2]-'0')*10 + (IOtime[3]-'0');
		time[2] = (IOtime[4]-'0')*10 + (IOtime[5]-'0');
	}
/****************************/
	ES=1;		//���¿��������ж�
}

int main(void)
{	
	//��ʼ��OLED  
	OLED_Init();
	OLED_Clear();
	
	OLED_ShowString(0,0,"Waiting 8266Init",16);
	OLED_ShowString(0,2,"by Lazurit",16);
	OLED_ShowCHinese(100,3,4);//ͼ��
	OLED_ShowString(0,4,"2021.12.3",16);
	while(!P17);//��8266��p17����
	UartInit();//���ڳ�ʼ��
	while(P17);//��8266��p17����

	Timer0Init();//�Զ�ʱ��0��ʼ��
	Timer1Init();//�Զ�ʱ��1��ʼ��
	
	//AUXR |= 0x02;//����ʹ��Ƭ����չram,��֪��12c5a��û��
	EA = 1;//���жϿ�
	
	buzzer = 0;//�����ϵ���һ��
	P0M0 = 0xff;//���������������
	
	oled_flash(0);
	while(1) 
	{		
		//��ʱ��
		if (key1 == 0)
		{
			delay_ms(10);
			if (key1 == 0)
			{
				if(counter_type==0)//û�й���������Ҫ��ʼ
				{
					work_type = 0;//�����жϷ������������м�ʱ���Ƕ�ʱ
					count[0]=0;count[1]=0;//��ʼ�� count����
					TR1 = 1;//��ʱ��1����
					counter_type = !counter_type;//ˢ��work_type��״̬
				}
				else if(counter_type==1)//�����ˣ�����Ҫֹͣ
				{
					count[0]=0;count[1]=0;//��ʼ�� count����
					TR1 = 0;//��ʱ��1�ر�
					counter_type = !counter_type;//ˢ��work_type��״̬
				}
				while(!key1);
			}
		}
		
		//����ʱ��
		if (key2 == 0)
		{
			delay_ms(10);
			if (key2 == 0)
			{
				while(!key2);
				work_type = 1;
				count[0]=0;count[1]=0;//��ʼ�� count����
				countdown_reset();//�������ù���
				TR1 = 1;//��ʱ��1����
			}
		}
		
		if((time[0]*3600+time[1]*60+time[2]) - buzzer_delay >= 6)//����������6���ֹͣ
		{
			buzzer = 0;//�رշ�����
			buzzer_delay = 0;
		}
		
		if (key3 == 0)
		{
			delay_ms(10);
			if (key3 == 0)
			{
				while(!key3);
				TR0 = 0;//�رն�ʱ��0
				TR1 = 0;//�رն�ʱ��1
				time_set();//���µ�oledˢ�°취�����û�����ʱ��
				TR0 = 1;//���ö�ʱ��0
			}
		}
		
	}
}