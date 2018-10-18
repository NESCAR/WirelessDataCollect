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
优先级
    //TIM 3
   	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //子优先级0
	
	//外部中断
	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	
	//CAN1
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 主优先级为2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	
	//CAN2
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 主优先级为2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为1
	
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
	//uart_init(115200);		
	delay_init(168); 
	EXTI_Conf();//必须在wifi设置前
	NVIC_Config();	//必须在wifi设置前
  SPI_Conf();	
	WIFI_BOOT();
	WIFI_Conf();
  queue_init(&adc_queue);
	ADS8266_config();
	delay_ms(1000);
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal);   //500K
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,12,CAN_Mode_Normal);   //250k
	TIM3_Int_Init(999,83); //1000us
	TIM4_Int_Init(99,83); //100us
	
}
u8 Status=1;
extern u32 bytes_sent;

int main(void)
{     
 	
	Initialization();//初始化系统
	
#ifdef SEND_WITH_UDP
	OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//服务器的数据
#else
		OpenTcpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//创建一个数据收发socket
	//  rsi_send_data(socketDescriptor_txrx, "qqqqqqqqqqqqqqqq", 16,RSI_PROTOCOL_TCP_V4,&bytes_sent);
#endif
	OpenLudpSocket(localDestIp_txrx,localDestSocket_txrx,localModuleSocket_txrx,&localSocketDescriptor_txrx);//局域网内数据传输
	OpenLudpSocket(destIp_sync,destSocket_sync,moduleSocket_sync,&socketDescriptor_sync);//时钟同步socket

	while(1)
	{
		receive_udp_package();
		wifi_send_package();//发送数据，每次时钟更新后或者数据到达一定数量UDP_SEND_SIZE  8bytes时间+2bytes数字IO+8*N bytes ADC信号
		#if IAM_MASTER_CLOCK
			if(sync_interval_time>=SYNC_INTERVAL_TIME&&Wifi_Send_EN)
			{
				LED2 = !LED2;//用来表示在更新时钟
				sync_interval_time = 0;
				Send_Sync_Time();//时钟同步一下
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








