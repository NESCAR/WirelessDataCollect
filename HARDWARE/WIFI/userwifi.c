#include "userwifi.h"
#include  "wificonf.h"
#include "rsi_global.h"
#include "led.h"
#include "rsi_app.h"
#include "config.h"
extern rsi_app_cb_t rsi_app_cb;
/* Gloab  data*/

u8 DATA_AUTO_CHECK_EN = 1;	

//timer 
u32 SYSTEMTIME=0;

u32  YYMMDD =0;
u8 Time_Sync_Flag = 0;//ʱ��ͬ���ź�
u8 Wifi_Send_EN = 1;//�������ݵ�����
u8 CAN_Send_EN = 0;

//IIC
Queue adc_queue;	 //adc���ջ���


//�ͻ���ģʽ��һЩ����
//wifi_main 
u8 localDestIp_txrx[4]={255,255,255,255};    //������
//u8 destIp_txrx[4]={255,255,255,255};    //������Զ�������շ�


u8 destIp_txrx[4]={DESTIP_TXRX_GROUP1,DESTIP_TXRX_GROUP2,DESTIP_TXRX_GROUP3,DESTIP_TXRX_GROUP4};    //������Զ�������շ�


u8 destIp_sync[4]={255,255,255,255};  //ͬ��
unsigned short destSocket_txrx= 5001;
unsigned short moduleSocket_txrx=5002;
unsigned short destSocket_sync= 5003;//����Ҫһ��
unsigned short moduleSocket_sync=5003;
unsigned short localDestSocket_txrx= 5004;//��������Զ��IP
unsigned short localModuleSocket_txrx=5005;


unsigned short socketDescriptor_txrx=0;
unsigned short socketDescriptor_sync=1;
unsigned short localSocketDescriptor_txrx=2;

u32 bytes_sent=0;


 
#define ANAL_RSP_LENGTH 3
u8 AnalRsp[ANAL_RSP_LENGTH];//anal���������Ҫ����wifi��Ϣ
void receive_udp_package()
{
	rsi_recvFrameUdp *data_recv=NULL;

	u8 RspCode;
	unsigned short  recvSocket;
	RspCode = Check_PKT();
	
	switch (RspCode)
	{
		case RSI_RSP_DATA_RECEIVE:  //���յ�����
			data_recv = (rsi_recvFrameUdp *)&(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad);
		    recvSocket = rsi_bytes2R_to_uint16(data_recv->recvSocket);
		    if(recvSocket ==socketDescriptor_sync)//�������ʱ��ͬ����Ϣ
			{ 
				if(order_anay(data_recv->recvDataBuf) == NEED_RETURN_INFO)//���������1˵������Ҫ�ظ���Ϣ
				{
					AnalRsp[0] = RETURN_INFO;//��ʾ���Ƿ�����Ϣ
					AnalRsp[1] = data_recv->recvDataBuf[0];//���ش�����������
					AnalRsp[2] = nodeId;
					u8 temp = DATA_AUTO_CHECK_EN;DATA_AUTO_CHECK_EN = 0;
					rsi_send_ludp_data(socketDescriptor_sync,AnalRsp ,ANAL_RSP_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_sync, destSocket_sync ,&bytes_sent);
					DATA_AUTO_CHECK_EN = temp;
				}
			}
			
			break;
		case RSI_FWUP_RSP://���߸��¹̼���RS9113�ģ�
			rsi_wireless_fwupgrade();
			break;
		default:
			break;
	
	}
}


void wifi_send_package_test()
{
	int Head;
	uint32 Length;
	u8 temp= DATA_AUTO_CHECK_EN;
	for(int i =0;i<UDP_SEND_SIZE;i++)
	{
		queue_put(&adc_queue,i);
	}

	Length = queue_length(adc_queue);
	queue_addtime_addIO(&adc_queue,Length, nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2);
	if(adc_queue.head + UDP_SEND_SIZE > QUEUE_SIZE ) queue_oversize(&adc_queue,adc_queue.head + UDP_SEND_SIZE - QUEUE_SIZE);
	Head = adc_queue.head;
	adc_queue.head = adc_queue.tail; 

	DATA_AUTO_CHECK_EN = 0;
	//������
	rsi_send_ludp_data(localSocketDescriptor_txrx, &adc_queue.arr[Head],Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
	receive_udp_package();
	delay_ms(1000);
	//Զ�̷�����
	rsi_send_ludp_data(socketDescriptor_txrx, &adc_queue.arr[Head],Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
	DATA_AUTO_CHECK_EN = temp;
	
}

int16 TcpStatus =-1;
int TcpCount = 0;
u8 wifi_send_package()
{
	int Head;
	uint32 Length;

	
	if(!Wifi_Send_EN) return 0;

	if(Time_Sync_Flag==1)
	{
		Length = queue_length(adc_queue);
		
		queue_addtime_addIO(&adc_queue,Length, nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2);    //  head <- head-10; //
		
		if(adc_queue.head + UDP_SEND_SIZE > QUEUE_SIZE ) queue_oversize(&adc_queue,adc_queue.head + UDP_SEND_SIZE - QUEUE_SIZE);
		Head = adc_queue.head;
		adc_queue.head = adc_queue.tail; 
		//  why not use   rsi_send_data()	
		u8 temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		//���͵�Զ�̷�����
	#ifdef SEND_WITH_UDP
			rsi_send_ludp_data(socketDescriptor_txrx, &adc_queue.arr[Head],Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
	    
		#else
			rsi_send_data(socketDescriptor_txrx,  &adc_queue.arr[Head], Length+PACKAGE_HEAD_FRAME_LENGTH,RSI_PROTOCOL_TCP_V4,&bytes_sent);
	#endif
		DATA_AUTO_CHECK_EN = temp;
		
		
//		//��ʱ��֤����udp��������
//		delay_us(100);
//		for(int n=0;n<20;n++){
//			receive_udp_package();
//			delay_us(100);
//		}
		delay_ms(5);
		
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		//���͵�������
		rsi_send_ludp_data(localSocketDescriptor_txrx, &adc_queue.arr[Head],Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
		DATA_AUTO_CHECK_EN = temp;
		Time_Sync_Flag = 0;//ʱ��ͬ��λ����
	}
	
	if(queue_length(adc_queue) >= UDP_SEND_SIZE - PACKAGE_HEAD_FRAME_LENGTH )//�Ҿ���>=�ȽϺã���һû���ü�����==UDP_SEND_SIZE - 10�������
	{
		
		Length = queue_length(adc_queue);
		
		queue_addtime_addIO(&adc_queue,Length,nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2);   //  head <- head-10;
		if(adc_queue.head + UDP_SEND_SIZE > QUEUE_SIZE ) queue_oversize(&adc_queue,adc_queue.head + UDP_SEND_SIZE - QUEUE_SIZE);
		Head = adc_queue.head;
		adc_queue.head = adc_queue.tail; 
		//���͵�Զ�̷�����
		u8 temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
#ifdef SEND_WITH_UDP
		rsi_send_ludp_data(socketDescriptor_txrx, &adc_queue.arr[Head],Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
#else
		while(TcpStatus!=0&&TcpCount <100)
		{
			TcpCount++;
			TcpStatus = rsi_send_data(socketDescriptor_txrx,  &adc_queue.arr[Head],(uint32)(Length + PACKAGE_HEAD_FRAME_LENGTH),RSI_PROTOCOL_TCP_V4,&bytes_sent);
		}
		TcpCount=0;
		TcpStatus=-1;
#endif
		DATA_AUTO_CHECK_EN = temp;
		
		//��ʱ��֤����udp��������
//		delay_us(100);
//		for(int n=0;n<20;n++){
//			receive_udp_package();
//			delay_us(100);
//		}
		delay_ms(5);
		//���͵�������
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		rsi_send_ludp_data(localSocketDescriptor_txrx, &adc_queue.arr[Head],Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
		DATA_AUTO_CHECK_EN = temp;
	}
	return 1;
}

//���������1˵������Ҫ�ظ���Ϣ
//���߱��ν��յ�����Ϣ����Ҳ����0
extern u8 txrx_refreshed;
u8 order_anay(u8 arr[])
{
	switch(arr[0])
	{
		case GET_TIME_SYNC_PC://ʱ��ͬ���ź�
			if(DATA_AUTO_CHECK_EN)
			{
			memcpy(&YYMMDD,&arr[1],4);
			memcpy(&SYSTEMTIME,&arr[5],4);
			Time_Sync_Flag = 1;
			LED2_CONV();
			LED4_CONV();
		  }
			break;
		case 	GET_TIME_SYNC_MAIN_CLOCK:
			memcpy(&YYMMDD,&arr[1],4);
			memcpy(&SYSTEMTIME,&arr[5],4);
			Time_Sync_Flag = 1;
			LED2_CONV();
			LED4_CONV();
			return NOT_NEED_RETURN_INFO;				
		case RETURN_INFO://�����˻ظ��ź�
			return NOT_NEED_RETURN_INFO;//��ʾ����Ҫ������Ϣ
		case GET_WIFI_SEND_EN:
			//��������˱��豸
			if(arr[1]==nodeId){
				Wifi_Send_EN =1;  //wifi��ʼ����
			}
			else{
				return NOT_NEED_RETURN_INFO;
			}
			break;
		case GET_WIFI_SEND_DISABLE:        		
			if(arr[1]==nodeId){
				Wifi_Send_EN =0;//wifiֹͣ����
			}
			else{
				return NOT_NEED_RETURN_INFO;
			}
			break;
		case GET_CHANNEL_MODEL:         // ͨ��ģʽѡ��
			if(arr[1]==nodeId)//�������ָ���˱�ID
				Channel_model(&arr[2]);
			else  //�������Ǹ����豸�ģ��򷵻�0����ʾ����Ҫ����ack
				return NOT_NEED_RETURN_INFO;
			break;
		case GET_CAN_SEND_EN:
			if(arr[1]==nodeId){
				CAN_Send_EN =1;//wifiֹͣ����
			}
			else{
				return NOT_NEED_RETURN_INFO;
			}
			break;
		case GET_REMOTE_IP_PORT:            //������ַ
			memcpy(destIp_txrx,&arr[1],4);
			memcpy(&destSocket_txrx,&arr[5],2);
			rsi_socket_close(socketDescriptor_txrx, moduleSocket_txrx);//�رյ�ԭ����Զ�̷�������socket
			#ifdef SEND_WITH_UDP
			OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);
			#else
			OpenTcpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//����һ�������շ�socket
			#endif
			break;
		case GET_TEST_NAME://��ȡ��������	
			if(strlen((c8*)(arr+1)) < MAX_TEST_NAME_LENGTH){
				memset((u8 *)test_name,0,MAX_TEST_NAME_LENGTH);//ȫ��resetΪ0
				strcpy((char *)test_name,(c8*)(arr+1));
				printf("\r\nGet Test Name : \"%s\"(UTF-8)\r\n",(arr+1));//���UTF-8����
			}
			break;	
		case PAGING://Ѱ���ź�
			break;		
		default:
			return NOT_NEED_RETURN_INFO;
	}
	return NEED_RETURN_INFO;
	
}

//��udp
u8 OpenLudpSocket(u8 *destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor)
{
	int RspCode;
	u8 temp= DATA_AUTO_CHECK_EN ;
	DATA_AUTO_CHECK_EN= 0;
	OpenSocket(destIp,destSocket,moduleSocket,RSI_SOCKET_LUDP);
	RspCode=Read_PKT();
	if(RspCode!=RSI_RSP_SOCKET_CREATE)
	{
		DATA_AUTO_CHECK_EN = temp;
		return 1;
	}
	
	*socketDescriptor = rsi_bytes2R_to_uint16(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.socketFrameRcv.socketDescriptor);
	DATA_AUTO_CHECK_EN = temp;
	return 0;
}

//��tcp
u8 OpenTcpSocket(u8 *destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor)
{
	u8 temp= DATA_AUTO_CHECK_EN ;
	DATA_AUTO_CHECK_EN= 0;
	
	int RspCode;
	OpenSocket(destIp,destSocket,moduleSocket,RSI_SOCKET_TCP_CLIENT);
	RspCode=Read_PKT();
	if(RspCode!=RSI_RSP_SOCKET_CREATE)
	{
		DATA_AUTO_CHECK_EN = temp;
		return 1;
	}
	DATA_AUTO_CHECK_EN = temp;
	*socketDescriptor = rsi_bytes2R_to_uint16(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.socketFrameRcv.socketDescriptor);
	
	return 0;
}
#if IAM_MASTER_CLOCK
#define SYNC_TIME_BYTES   9   //ͬ��ʱ�������
void Send_Sync_Time(void)
{
	uint8 time[SYNC_TIME_BYTES] = {0};//ʱ����Ϣ
	time[0] = GET_TIME_SYNC_MAIN_CLOCK;//ʱ��ͬ������
	time[1] = (uint8) YYMMDD;
	time[2] = (uint8) (YYMMDD>>8);
	time[3] = (uint8) (YYMMDD>>16);
	time[4] = (uint8) (YYMMDD>>24);
	time[5] = (uint8) SYSTEMTIME;
	time[6] = (uint8) (SYSTEMTIME>>8);
	time[7] = (uint8) (SYSTEMTIME>>16);
	time[8] = (uint8) (SYSTEMTIME>>24);
	u8 temp= DATA_AUTO_CHECK_EN ;
	DATA_AUTO_CHECK_EN= 0;
	rsi_send_ludp_data(socketDescriptor_sync,time ,SYNC_TIME_BYTES, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_sync, destSocket_sync ,&bytes_sent);
	receive_udp_package();
	DATA_AUTO_CHECK_EN = temp;
	//rsi_send_data(socketDescriptor_sync, time, SYNC_TIME_BYTES,RSI_PROTOCOL_UDP_V4,&bytes_sent);
}
#endif

//void EXTI15_10_IRQHandler(void)
//{
//	EXTI->PR		|=1<<10;
//	wifi_send_package_test();
//	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
//}
