#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "spi.h"
#include "myiic.h"
#include "queue.h"
#include "can.h"
#include "can2.h"
#include "timer.h"
#define  UDP_SEND_MAX 500
void Initialization (void)
{
//	uint32_t client_port=0;	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC_Config();	
	LED_Init();		
	EXTI_Conf();
	NVIC_Config();
	uart_init(115200);		
	delay_init(168);  
	TIM3_Int_Init(999,83); //1ms
	ADS8266_config();
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal);   //500K
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,12,CAN_Mode_Normal);   //250k
	
	//SPI_Conf();	
	//WIFI_BOOT();
	//WIFI_Conf();

	//��һ������ֲԭ�ļ� ��mcu���� 
//	while(!RNG_GetFlagStatus(RNG_FLAG_DRDY));
//	client_port=RNG_GetRandomNumber();
	/*���ϴ��붼�Ǻ�Ӳ����� ��������
	ģ����Ҫ���� SPi������ �ж� ����ģ���IO	*/	
	
	/*
	1.�ײ���ȫ���õĹ��̴���.������MCU����
	RS9113��io������ȷ
	2.��ʼ��ֲ
	RS9113.NBZ.WC.GEN.OSI.1.6.6\hostֻʹ��binary ����ɾ�� ���Ƶ�mcu������
	3.��ֲ.cԴ�ļ�
		WLAN_APP ����Ҫ rsi_wifi_state_mc.c
		WLAN_CORE ����
		WLAN_HAL ֻҪSPI
	
	4.����include·��
	..\host\binary\apis\hal\include;
	..\host\binary\apis\intf\spi\include;
	..\host\binary\apis\wlan\core\include;
	..\host\binary\apis\wlan\ref_apps\include	
	
	5.������ 
	��Ҫ����WLAN_ENABLE �� 	#define WLAN_ENABLE 1	
	SPI�ӿں�				#define RSI_INTERFACE RSI_SPI      
	6.�޸ı���ľ����error
	7.����û�ô���.��ʼ��ֲ�ײ� 0 error 
	wifi_main.c
	��ԭ���ṩ��һ������ģʽ��ʵ��.ȫ�� ������ԱȽϷ����Ӵ�
	�������ܷѾ�
	ֱ��ʹ��������ֲ�õľͿ�����.ԭ����Ҳ����ʹ�� 

	8.ENABLE ������� ��STM32���ͻ ,��Ҫ�޸� RSI_CONNFIG.h �޸����� Ȼ��ע�͵�

	//#define ENABLE                      1
	//#define DISABLE                     0

	�ڶ������޸�Ӳ���ײ�
	1.������ֲSPI
	spi	��Ҫ�޸� ���ͺͽ���  ��Ƭѡ
	 ֱ�����Ѿ�д�õ� ���ͺͽ��� 
		ֻ��Ҫ�޸�
		rsi_spi_recv 
		rsi_spi_send
		���������޸�
	2.Ȼ���Ǹ�λ Io
	3.Ȼ�����ж� ,
	�жϾ����жϷ���������λ
	rsi_app_cb.pkt_pending = 1;//RSI_TRUE;
	������ֲ��� ,�ײ���ֲ��� ��ʣ�²���
	
	
	�������� 
	ʹ�����Լ���װ�ĺ��� ����ϰ��ʹ��
	
	��ʹ�ùٷ���ʵ�� ɾ��,̫����
	��ֲд�õĴ���
	�������������
	
	1.��ʼ����
		WIFI_BOOT
	2.����Ӳ��
	3.rsi_sys_init������ִ��
	 ����������card ready(0x89)
	 ˵����ֲ�ɹ� ,
	 ��������Ӧ����


	4. WIFI_Conf()��һ��apģʽ�Ĵ��� ��Ҫ�޸�rsi_config.h������	
	
	*/
	
}
u8 Status=1;

int main(void)
{        
	Initialization();

	while(1)
	{				 
		delay_ms(1000);
		Status = ADS8266_config();
		
	}
}


//u8 wifi_send_queue()
//{
//	u8 head;
//	if()//ʱ��ͬ���󽫶���������ȫ������
//	{
//		
//		u32 send_length = 8+queue_length(adc_queue);
//		head = adc_queue.head;
//		adc_queue.head=adc_queue.tail;   //head = tail
//		
//		
//	}
//	else if(queue_length(adc_queue)>UDP_SEND_MAX)
//		{
//			bytes_sent = 8+queue_length(adc_queue);
//			head = adc_queue.head;
//			queue_addtime(&adc_queue);
//			adc_queue.head =(adc_queue.head+UDP_SEND_MAX)%
//			
//			
//		}
//}




