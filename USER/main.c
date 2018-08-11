#include "sys.h"
#include "userwifi.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "spi.h"
#include "myiic.h"
#include "queue.h"
#include "can.h"
#include "can2.h"
#include "timer.h"

#include "rsi_global.h"
#include "rsi_app.h"

/*
���ȼ�
    //TIM 3
   	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�0
	
	//�ⲿ�ж�
	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	
	//CAN1
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // �����ȼ�Ϊ2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	
	//CAN2
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // �����ȼ�Ϊ2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ1
	
*/
extern u8 CAN_Send_EN;
extern u8 CAN1_Send_EN;
extern u8 CAN2_Send_EN;
u8 IO_input[3];
u8 can_send_package(void);
void Initialization (void)
{
//	uint32_t client_port=0;	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC_Config();	
	LED_GPIO_Init();
	EXTI_Conf();
	NVIC_Config();
	uart_init(115200);		
	delay_init(168); 
	
    SPI_Conf();	
	WIFI_BOOT();
	WIFI_Conf();
    queue_init(&adc_queue);
	ADS8266_config();
	delay_ms(1000);
	TIM3_Int_Init(999,83); //1ms
//	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal);   //500K
//	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,12,CAN_Mode_Normal);   //250k
		
}
u8 Status=1;
//extern u32 bytes_sent;
//extern int delta_time;
int main(void)
{        
	Initialization();//��ʼ��ϵͳ
	OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//����һ�������շ�socket
	OpenLudpSocket(destIp_sync,destSocket_sync,moduleSocket_sync,&socketDescriptor_sync);//ʱ��ͬ��socket
	//rsi_socket_close(socketDescriptor_txrx, moduleSocket_txrx);//�رյ�
	while(1)
	{
//		rsi_send_ludp_data(socketDescriptor_txrx, (uint8 *)"123",3, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx ,&bytes_sent);
//		LOGS("data_sent = %d\n",bytes_sent);
		
		receive_udp_package();//�����������ʱ��ͬ���źš�PC�˵������
		wifi_send_package();//�������ݣ�ÿ��ʱ�Ӹ��º�������ݵ���һ������UDP_SEND_SIZE  8bytesʱ��+2bytes����IO+8*N bytes ADC�ź�
		#if IAM_MASTER_CLOCK
			if(sync_interval_time>=SYNC_INTERVAL_TIME)
			{
				LED2 = !LED2;//������ʾ�ڸ���ʱ��
				sync_interval_time = 0;
				Send_Sync_Time();//ʱ��ͬ��һ��
			}
		#endif		
		//can_send_package();
	}
}


u8 can_send_package()
{ 
	if(CAN_Send_EN&&CAN1_Send_EN){   
		if(queue_empty(adc_queue)) delay_ms(2);
		IO_input[0] = DIGITAL_INPUT1;
	    IO_input[1] = DIGITAL_INPUT2;
		IO_input[2] = WIFI_CLIENT_ID;
		CAN1_Send_Msg((u8 *) &adc_queue.YYYY_MM_DD, 8);
		CAN1_Send_Msg((u8 *) &adc_queue.arr[adc_queue.head],8);
		CAN1_Send_Msg(IO_input,3);
		CAN_Send_EN = 0;
		CAN1_Send_EN = 0;
	}
	if(CAN_Send_EN && CAN2_Send_EN){
	
		if(queue_empty(adc_queue)) delay_ms(2);
		IO_input[0] = DIGITAL_INPUT1;
	    IO_input[1] = DIGITAL_INPUT2;
		IO_input[2] = WIFI_CLIENT_ID;
		CAN2_Send_Msg((u8 *) &adc_queue.YYYY_MM_DD, 8);
		CAN2_Send_Msg((u8 *) &adc_queue.arr[adc_queue.head],8);
		CAN2_Send_Msg(IO_input,3);
	    CAN_Send_EN = 0;
		CAN2_Send_EN = 0;
	}
	return 1;
}








