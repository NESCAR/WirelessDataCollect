/**
  ******************************************************************************
  * @file    timer.c
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   timer configuration and running functions   ��ʱ�����ú����з���
  ******************************************************************************
  */

//-------------include---------------//
#include "timer.h"
#include "gpio.h"
#include "queue.h"
#include "string.h"
#include "userwifi.h"
#include "config.h"
#include "adc.h"

/**
  * @brief  ��ʱ��4��ʼ��
  * @param  arr���Զ���װֵ
  * @param  psc��Ԥ��Ƶֵ
  * @note   freq = Fclk / (arr + 1) / (psc + 1) ������TIM4 Fclk = 84MHz
  * @retval None
  */
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* ʹ��TIM4ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  

	/* �����Զ���װ��ֵ*/
	TIM_TimeBaseInitStructure.TIM_Period = arr;
	/* ����Ԥ��Ƶֵ */
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;
	/* ���ϼ���ģʽ */
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	/* ��ʱ��ʱ�� (CK_INT) Ƶ���������˲�����ʹ�õĲ���ʱ�ӣ�ETR��TIx��֮��ķ�Ƶ��*/
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	/* TIM4��������*/
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);

	/* ����TIM4�����ж�*/
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 
	/* ʹ��TIM4*/
	TIM_Cmd(TIM4,ENABLE); 

	/* TIM4�ж�*/
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; 
	/* ��ռ���ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
	/* �����ȼ�3*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**
  * @brief  ��ʱ��4�жϴ�����������ϵͳʱ�Ӽ�ʱ
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{
		/* ϵͳʱ�ӣ���ʱ��λ��TIM4���õ��ж�����ֵ*/
		SYSTEMTIME++;
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	//����жϱ�־λ
}

/**
  * @brief  ��ʱ��3��ʼ��
  * @param  arr���Զ���װֵ
  * @param  psc��Ԥ��Ƶֵ
  * @note   freq = Fclk / (arr + 1) / (psc + 1) ������TIM4 Fclk = 84MHz
  * @retval None
  */
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//!ͬ��ʱ��ģ������ϴ�ͬ����ʱ����
#if IAM_MASTER_CLOCK
volatile u32 sync_interval_time = 0;
#endif

//!msʱ��
u32 MS_TIME = 0;
/**
  * @brief  ��ʱ��3�жϴ����������ڴ�����������ԵĻ
  * @note   �ж�������TIM3��ʼ��ʱ��������ʱ����1ms����
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	//!adc��ֵ����
	u8 * adcTamp;
	MS_TIME ++ ;
	//!����ж�
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
		#if IAM_MASTER_CLOCK
			sync_interval_time++;
		#endif
		
		/* ���50ms���壬���ڲ���ͬ��ʱ�Ӿ���*/
		/*! @note ����ʱ����Ҫ�رմ��ڴ�ӡ���� @ref PRINT_UART_LOG*/
		#if IO_SHINE_IN_TIMER
			if(MS_TIME%20==0){//50ms���һ������
				PAout(9) = ~PAout(9);
				PAout(10) = ~PAout(10);
			}
		#endif
			
		/* LED��˸*/
		#if LED_SHINE_IN_TIMER	
			if(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL){/*!< CLIENTģʽ�µ�����˸>*/
				if(MS_TIME%1000==0){
					LED1_CONV();//���ذ�DS1
					LED3_CONV();//DS3��ת
				}
			}else if(RSI_WIFI_OPER_MODE == RSI_WIFI_AP_MODE_VAL){/*!< APģʽ�¿�����˸>*/
				if(MS_TIME%200==0){
					LED1_CONV();//���ذ�DS1
					LED3_CONV();//DS3��ת
				}				
			}

		#endif
		/* �����ڻ���APģʽ�µ�ָ��*/
		if(MS_TIME%2000 == 0){
			dealCmdMsg(&CMD_RX_BUF);
//			printf("tail : %d\r\nHead : %d\r\n",adc_queue.tail,adc_queue.head);
		}
		/* ������п��ˣ�ʱ�������*/
		if(queue_empty(adc_queue)){
			adc_queue.HeadTime = SYSTEMTIME;
			adc_queue.YYYY_MM_DD = YYMMDD;
		}
		/* ��ʼ���������ٿ�ʼ�ɼ�*/
		if(Wifi_Send_EN){
			/*! @note 
				//       ת��ʱ�� = N * Tconv + (N-1) * 1us,Tconv = 2us for AD7606-4,Tconv = 3us for AD7606-6
				//       AD7606-4,64 Sample ratio,T = 193
				//       CONV  :  H(25ns,ת����)  ->  L(25ns)   ->   H(25ns,ת����)
			*/
			ADC_CONV_L();//���ʱ��25ns
			/**�ɼ����ݣ�˳�㵱����ʱ��*/
			adcTamp = ADC_Read(ADC_MAX_BYTES);
			/* ���˸��ֽ�����*/
			for(int i=0;i<ADC_MAX_BYTES;i++){
				queue_put((Queue *)&adc_queue, *(adcTamp+i));
//				queue_put((Queue *)&adc_queue, 1);
					
			}
			/* ���߿�ʼ��һ��ת��*/
			ADC_CONV_H();//���ʱ��25ns
			
			
		}	
	 }

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}



