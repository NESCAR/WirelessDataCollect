#include "sys.h"
#include "userwifi.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "wificonf.h"
#include "myiic.h"
#include "queue.h"
#include "can.h"
#include "can2.h"
#include "timer.h"
#include "adc.h"

#include "rsi_global.h"
#include "rsi_app.h"

/*
���ȼ�
	
	//�ⲿ�ж�
	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	
    //TIM 4
   	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�0
	
    //TIM 3
   	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�0
	
	//CAN1
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     // �����ȼ�Ϊ2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	
	//CAN2
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     // �����ȼ�Ϊ2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ1
	
*/
extern u8 CAN_Send_EN;
extern u8 CAN1_Send_EN;
extern u8 CAN2_Send_EN;
extern u8 DATA_AUTO_CHECK_EN;
u8 IO_input[3];
u8 can_send_package(void);
void Initialization (void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	GP_IO_Init();
	//uart_init(115200);		
	delay_init(168); 
	WIFI_SPI_Conf();	
	WIFI_BOOT();
	WIFI_Conf();
	queue_init(&adc_queue);
	delay_ms(1000);
	ADC_CTRL_Conf();//ADC������ų�ʼ��
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal);   //500K
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,12,CAN_Mode_Normal);   //250k
	TIM3_Int_Init(999,83); //1000us
	TIM4_Int_Init(4,83); //5us
}

u8 Status=1;
extern u32 bytes_sent;
int main(void)
{     

	Initialization();//��ʼ��ϵͳ

	#ifdef SEND_WITH_UDP
		OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//������������
	#else
		OpenTcpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//����һ�������շ�socket
	//  rsi_send_data(socketDescriptor_txrx, "qqqqqqqqqqqqqqqq", 16,RSI_PROTOCOL_TCP_V4,&bytes_sent);
	#endif
	OpenLudpSocket(localDestIp_txrx,localDestSocket_txrx,localModuleSocket_txrx,&localSocketDescriptor_txrx);//�����������ݴ���
	OpenLudpSocket(destIp_sync,destSocket_sync,moduleSocket_sync,&socketDescriptor_sync);//ʱ��ͬ��socket
  DATA_AUTO_CHECK_EN= 1;
	
	while(1)
	{
	//	receive_udp_package();
		
		wifi_send_package();//�������ݣ�ÿ��ʱ�Ӹ��º�������ݵ���һ������UDP_SEND_SIZE  8bytesʱ��+2bytes����IO+8*N bytes ADC�ź�
		
//		wifi_send_package_test();
//		GPIO_SetBits(GPIOA,GPIO_Pin_9);
		DATA_AUTO_CHECK_EN= 1;
//		delay_ms(60);
		#if IAM_MASTER_CLOCK
			if(sync_interval_time>=SYNC_INTERVAL_TIME&&Wifi_Send_EN)
			{
				LED2 = !LED2;//������ʾ�ڸ���ʱ��
				sync_interval_time = 0;
				Send_Sync_Time();//ʱ��ͬ��һ��
			}
		#endif		
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








