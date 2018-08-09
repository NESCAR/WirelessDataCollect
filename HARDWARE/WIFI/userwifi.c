#include "userwifi.h"
#include  "spi.h"
#include "rsi_global.h"
#include "led.h"
#include "rsi_app.h"

extern rsi_app_cb_t rsi_app_cb;
/* Gloab  data*/

//timer 
u32 SYSTEMTIME=0;
u32  YYMMDD =0;
u8 Time_Sync_Flag=0;//ʱ��ͬ���ź�
u8 Wifi_Send_EN =1;
u8 CAN_Send_EN = 0;

//IIC
Queue adc_queue;	 //adc���ջ���

//wifi_main
u8 destIp_txrx[4]={255,255,255,255};    //�����շ�
u8 destIp_sync[4]={255,255,255,255};  //ͬ��
unsigned short destSocket_txrx= 5001;
unsigned short moduleSocket_txrx=5002;
unsigned short destSocket_sync= 5003;//����Ҫһ��
unsigned short moduleSocket_sync=5003;
unsigned short socketDescriptor_txrx=0;
unsigned short socketDescriptor_sync=1;
u32 bytes_sent=0;


rsi_recvFrameUdp *data_recv=NULL;

void receive_udp_package()
{
	u8 RspCode;
	unsigned short  recvSocket;
	RspCode = Check_PKT();
	
	switch (RspCode)     //???
	{
		case 0x00:
			data_recv = (rsi_recvFrameUdp *)&(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad);
		    recvSocket = rsi_bytes2R_to_uint16(data_recv->recvSocket);
		    if(recvSocket ==socketDescriptor_sync)//�������ʱ��ͬ����Ϣ
			{ 
				order_anay(data_recv->recvDataBuf);
			}
			break;
		case 0x59:
			rsi_wireless_fwupgrade();
			break;
		default:
			break;
	
	}
}

u8 wifi_send_package()
{
	int Head;
	int Length;
	
	if(!Wifi_Send_EN) return 0;

	if(Time_Sync_Flag==1)
	{
		queue_addtime_addIO(&adc_queue, WIFI_CLIENT_ID, DIGITAL_INPUT1,DIGITAL_INPUT2);    //  head <- head-10; 
		if(adc_queue.head + UDP_SEND_SIZE > QUEUE_SIZE ) queue_oversize(&adc_queue,adc_queue.head + UDP_SEND_SIZE - QUEUE_SIZE);
		Head = adc_queue.head;
		Length = queue_length(adc_queue);
		adc_queue.head = adc_queue.tail; 
		
		//  why not use   rsi_send_data()				
		rsi_send_ludp_data(socketDescriptor_txrx, &adc_queue.arr[Head],Length, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
		Time_Sync_Flag = 0;//ʱ��ͬ��λ����
	}
	
	if(queue_length(adc_queue) >= UDP_SEND_SIZE - 10 )//�Ҿ���>=�ȽϺã���һû���ü�����==UDP_SEND_SIZE - 10�������
	{
		
		queue_addtime_addIO(&adc_queue,WIFI_CLIENT_ID, DIGITAL_INPUT1,DIGITAL_INPUT2);   //  head <- head-10;
		if(adc_queue.head + UDP_SEND_SIZE > QUEUE_SIZE ) queue_oversize(&adc_queue,adc_queue.head + UDP_SEND_SIZE - QUEUE_SIZE);
		Head = adc_queue.head;
		Length = queue_length(adc_queue);
		adc_queue.head = (adc_queue.tail+UDP_SEND_SIZE)%QUEUE_SIZE; 
		rsi_send_ludp_data(socketDescriptor_txrx, &adc_queue.arr[Head],UDP_SEND_SIZE, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx ,&bytes_sent);
	}
	return 1;
}

u8 order_anay(u8 arr[])
{
	switch(arr[0])
	{
		case GET_TIME_SYNC://ʱ��ͬ���ź�
			memcpy(&YYMMDD,&arr[1],4);
			memcpy(&SYSTEMTIME,&arr[5],4);
			Time_Sync_Flag = 1;
			break;		
		case GET_WIFI_SEND_EN:
			Wifi_Send_EN =1;  //wifi��ʼ����
			break;
		case GET_WIFI_SEND_DISABLE:          //wifiֹͣ����
			Wifi_Send_EN =0;
			break;
		case GET_CHANNEL_MODEL:         // ͨ��ģʽѡ��
			Channel_model(&arr[1]);
			break;
		case GET_CAN_SEND_EN:
			CAN_Send_EN = 1; // CANת�����ݣ�ʱ��+2·IO+4·AD
			break;
		case GET_REMOTE_IP_PORT:            //������ַ
			memcpy(destIp_txrx,&arr[1],4);
		    memcpy(&destSocket_txrx,&arr[5],2);
			rsi_socket_close(socketDescriptor_txrx, moduleSocket_txrx);//�رյ�ԭ����socket
			OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);
			break;			
		default:
			return 0;
	}
	return 1;
	
}

u8 OpenLudpSocket(u8 *destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor)
{
	int RspCode;
	OpenSocket(destIp,destSocket,moduleSocket,RSI_SOCKET_LUDP);
	RspCode=Read_PKT();
	if(RspCode!=RSI_RSP_SOCKET_CREATE)
	{
		return 1;
	}
	
	*socketDescriptor = rsi_bytes2R_to_uint16(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.socketFrameRcv.socketDescriptor);
	
	return 0;
}

#if IAM_MASTER_CLOCK
#define SYNC_TIME_BYTES   9   //ͬ��ʱ�������
void Send_Sync_Time(void)
{

	uint8 time[SYNC_TIME_BYTES] = {0};//ʱ����Ϣ
	time[0] = GET_TIME_SYNC;//ʱ��ͬ������
	time[1] = (uint8) YYMMDD;
	time[2] = (uint8) (YYMMDD>>8);
	time[3] = (uint8) (YYMMDD>>16);
	time[4] = (uint8) (YYMMDD>>24);
	time[5] = (uint8) SYSTEMTIME;
	time[6] = (uint8) (SYSTEMTIME>>8);
	time[7] = (uint8) (SYSTEMTIME>>16);
	time[8] = (uint8) (SYSTEMTIME>>24);
	rsi_send_ludp_data(socketDescriptor_sync,time ,SYNC_TIME_BYTES, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_sync, destSocket_sync ,&bytes_sent);
}
#endif






