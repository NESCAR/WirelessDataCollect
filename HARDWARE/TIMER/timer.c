#include "timer.h"
#include "led.h"
#include "myiic.h"
#include "queue.h"
#include "string.h"
#include "userwifi.h"
#include "config.h"
#include "adc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz  84M
//����ʹ�õ��Ƕ�ʱ��3!

void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///ʹ��TIM4ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//��ʼ��TIM4

	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //����ʱ��4�����ж�
	TIM_Cmd(TIM4,ENABLE); //ʹ�ܶ�ʱ��4

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{
		SYSTEMTIME++;
#if IO_SHINE_IN_TIMER
			if(SYSTEMTIME%100==0)//50ms���һ������
			{
				PAout(9) = ~PAout(9);
				PAout(10) = ~PAout(10);
			}
#endif
#if LED_SHINE_IN_TIMER	
		if(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL){
			if(SYSTEMTIME%1000==0)
			{
				LED1_CONV();//���ذ�DS1
				LED3_CONV();//DS3��ת
			}			
		}else if(RSI_WIFI_OPER_MODE == RSI_WIFI_AP_MODE_VAL){//APģʽ�¿�����˸
			if(SYSTEMTIME%200==0)
			{
				LED1_CONV();//���ذ�DS1
				LED3_CONV();//DS3��ת
			}				
		}

#endif
		if(SYSTEMTIME%500 == 0){
			dealCmdMsg(&CMD_RX_BUF);
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	//����жϱ�־λ
}

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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

#if IAM_MASTER_CLOCK
volatile u32 sync_interval_time = 0;
#endif
//short int time_inter=0;
//volatile u32 time1 = 0;
//volatile u32 time2 = 0;
//volatile u32 status = 0;
//��ʱ��3�жϷ�����
//100us INC
//volatile u32 AckFlag = 0;//���ڲ����Ƿ����û�н��յ�Ack�����������
void TIM3_IRQHandler(void)
{
	u8 * adcTamp;
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
	#if IAM_MASTER_CLOCK
			sync_interval_time++;
	#else

			if(queue_empty(adc_queue)) //������п���
			{
				adc_queue.HeadTime = SYSTEMTIME;
			}
			if(Wifi_Send_EN)//��ʼ���������ٿ�ʼ�ɼ�
			{
				//  CONV  :  H(25ns,ת����)  ->  L(25ns)   ->   H(25ns,ת����)
				ADC_CONV_L();//���ʱ��25ns
				/**�ɼ����ݣ�˳�㵱����ʱ��*/
				adcTamp = ADC_Read(ADC_MAX_BYTES);
				//���˸�����
				for(int i=0;i<8;i++)
				{
					queue_put(&adc_queue, *(adcTamp+i));
						
				}
				//���߿�ʼ��һ��ת��
				ADC_CONV_H();//���ʱ��25ns
				//ת��ʱ�� = N * Tconv + (N-1) * 1us,Tconv = 2us for AD7606-4,Tconv = 3us for AD7606-6
				//AD7606-4,64 Sample ratio,T = 193
			}
#endif		
	 }

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//����жϱ�־λ
}



