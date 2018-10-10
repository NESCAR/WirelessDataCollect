#include "timer.h"
#include "led.h"
#include "myiic.h"
#include "queue.h"
#include "string.h"
#include "userwifi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz  84M
//����ʹ�õ��Ƕ�ʱ��3!


void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3

	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

#if IAM_MASTER_CLOCK
u16 sync_interval_time = 0;
#endif
//short int time_inter=0;
//volatile u32 time1 = 0;
//volatile u32 time2 = 0;
//volatile u32 status = 0;
//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
//		time1 = TIM3->CNT;
		SYSTEMTIME++;
#if LED_SHINE_IN_TIMER
		if(SYSTEMTIME%1000==0)
		{
			LED1=!LED1;//DS1��ת
		}
		if(SYSTEMTIME%50==0){//50ms���һ������
			PAout(9) = ~PAout(9);
			PAout(10) = ~PAout(10);
		}
#endif
		
#if IAM_MASTER_CLOCK
		sync_interval_time++;
#endif
	  if(queue_empty(adc_queue)) //������п���
		{
			adc_queue.HeadTime = SYSTEMTIME;
		}
		if(Wifi_Send_EN)//��ʼ���������ٿ�ʼ�ɼ�
		{
			ADS8266_read();
		}
//		time2 = TIM3->CNT;
//		time_inter = time2-time1;
	}
//	if(TIM3->CNT>1000)//������ʾ�Ƿ��г�ʱ
//		status++;
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}



