/**
  ******************************************************************************
  * @file    userwifi.c
  * @author  zju.nesc.iotlab   �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   wifi application functions    WiFiģ��Ӧ�÷���
  ******************************************************************************
  */

//-------------include---------------//
#include "userwifi.h"
#include  "wificonf.h"
#include "rsi_global.h"
#include "gpio.h"
#include "rsi_app.h"
#include "config.h"
#include "timer.h"
#include "can1.h"
#include "can2.h"

u8     DATA_AUTO_CHECK_EN = 1;	     //�Ƿ����ж����Զ�check����
u32    SYSTEMTIME = 0;               //ϵͳʱ��
u32    YYMMDD =0;                    //������
u8     Time_Sync_Flag = 0;           //���ʱ���Ƿ�ͬ��
volatile u8 Wifi_Send_EN = 0;        //���ݷ��ͺͽ���ʹ��
u8     CAN_Send_EN = 0;              //CAN���ݷ��ͺͽ���ʹ��
Queue  adc_queue;                    //ADC���ݴ洢
Queue  can_queue;                    //can���ݴ洢
u8     localDestIp_txrx[4] = {255,255,255,255};
u8     destIp_txrx[4] = {DESTIP_TXRX_GROUP1,DESTIP_TXRX_GROUP2,DESTIP_TXRX_GROUP3,DESTIP_TXRX_GROUP4};
u8     destIp_sync[4] = {255,255,255,255};  //ͬ��
u16    destSocket_txrx = 5001;
u16    moduleSocket_txrx =5002;
u16    destSocket_sync = 5003;
u16    moduleSocket_sync = 5003;
u16    localDestSocket_txrx = 5014;
u16    localModuleSocket_txrx = 5005;
u16    socketDescriptor_txrx = 0;
u16    socketDescriptor_sync = 1;
u16    localSocketDescriptor_txrx = 2;
u32    bytes_sent = 0;//�ֽ����ݷ��͸���
u8     AnalRsp[ANAL_RSP_LENGTH];//anal���������Ҫ����״̬��Ϣ

/**
  * @brief  ����UDP��������
  * @param  None
  * @retval None
  */
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
			//!���߸���RS9113�̼�
//		case RSI_FWUP_RSP:
//			rsi_wireless_fwupgrade();
//			break;
		default:
			break;
	
	}
}

/**
  * @brief  WiFi���Ͳ���
  * @param  None
  * @retval None
  */
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
	queue_addtime_addIO(&adc_queue,Length, nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2,ADC_DATA_PACKAGE);
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

s16 TcpStatus = -1;
s32 TcpCount = 0;
/**
  * @brief  ����ADC��CAN����
  * @param  None
  * @retval ���ݷ���״̬��������ɣ�1��δ�������ͣ�0��
  */
u8 wifi_send_package()
{
	u16 Adc_Head;u16 Can_Head;
	u32 Adc_Length;u32 Can_Length;
	u8 temp;

	
	if(!Wifi_Send_EN){
		return 0;
	}
	
	/* ��ͬ��ʱ�ӵ���*/
	if(Time_Sync_Flag==1)
	{
		Adc_Length = queue_length(adc_queue);
		Can_Length = queue_length(can_queue);
		
		/* ADC Queue����֡ͷ*/
		queue_addtime_addIO(&adc_queue,Adc_Length, nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2,ADC_DATA_PACKAGE);
		
		/* CAN Queue����֡ͷ*/
		queue_addtime_addIO(&can_queue,Can_Length, nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2,CAN_DATA_PACKAGE);
		
		/* ADC����ֳ����Σ���ǰ��һ�θ��Ƶ�����*/
		if(adc_queue.head + Adc_Length > QUEUE_SIZE ) {
			queue_oversize(&adc_queue,adc_queue.head + PACKAGE_HEAD_FRAME_LENGTH + Adc_Length - QUEUE_SIZE);
		}
		/* CAN����ֳ����Σ���ǰ��һ�θ��Ƶ�����*/
		if(can_queue.head + Can_Length > QUEUE_SIZE ) {
			queue_oversize(&can_queue,can_queue.head + PACKAGE_HEAD_FRAME_LENGTH + Can_Length - QUEUE_SIZE);
		}
		
		/**��ȡ����ͷ�������¶���*/
		Adc_Head = adc_queue.head;
		Can_Head = can_queue.head;
		adc_queue.head = adc_queue.tail; 
		can_queue.head = can_queue.tail; 

		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		/* ADC���ݷ��͵�Զ�̷�����*/
		rsi_send_ludp_data(socketDescriptor_txrx, &adc_queue.arr[Adc_Head],Adc_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
		
		DATA_AUTO_CHECK_EN = temp;
		delay_ms(5);
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		/* ���͵�Զ�̷�����*/
		rsi_send_ludp_data(socketDescriptor_txrx, &can_queue.arr[Can_Head],Can_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
		
		DATA_AUTO_CHECK_EN = temp;
		/* ��ʱ��ֹ��������*/
		delay_ms(5);
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		/* ADC���ݷ��͵�������*/
		rsi_send_ludp_data(localSocketDescriptor_txrx, &adc_queue.arr[Adc_Head],Adc_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
		
		DATA_AUTO_CHECK_EN = temp;
		delay_ms(5);
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		/* CAN���ݷ��͵�������*/
		rsi_send_ludp_data(localSocketDescriptor_txrx, &can_queue.arr[Can_Head],Can_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
		DATA_AUTO_CHECK_EN = temp;		
		
		Time_Sync_Flag = 0;//ʱ��ͬ��λ����
	}
	/* ADC��������*/
	if(queue_length(adc_queue) >= (UDP_SEND_SIZE - PACKAGE_HEAD_FRAME_LENGTH )){
		
		Adc_Length = queue_length(adc_queue);
		
		queue_addtime_addIO(&adc_queue,Adc_Length,nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2,ADC_DATA_PACKAGE);   //  head <- head-10;
		if(adc_queue.head + UDP_SEND_SIZE > QUEUE_SIZE ){
			queue_oversize(&adc_queue,adc_queue.head + UDP_SEND_SIZE - QUEUE_SIZE);
		}
		Adc_Head = adc_queue.head;
		adc_queue.head = adc_queue.tail; 
		//���͵�Զ�̷�����
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		rsi_send_ludp_data(socketDescriptor_txrx, &adc_queue.arr[Adc_Head],Adc_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);

		DATA_AUTO_CHECK_EN = temp;
		delay_ms(5);
		//���͵�������
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		rsi_send_ludp_data(localSocketDescriptor_txrx, &adc_queue.arr[Adc_Head],Adc_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
		DATA_AUTO_CHECK_EN = temp;
	}
	/* CAN��������*/
	if( queue_length(can_queue) >= (UDP_SEND_SIZE - PACKAGE_HEAD_FRAME_LENGTH )){
		Can_Length = queue_length(can_queue);
		
		/* CAN Queue����֡ͷ*/
		queue_addtime_addIO(&can_queue,Can_Length, nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2,CAN_DATA_PACKAGE);
		
		/* ����ֳ����Σ���ǰ��һ�θ��Ƶ�����*/
		if(adc_queue.head + UDP_SEND_SIZE > QUEUE_SIZE ) {
			queue_oversize(&can_queue,can_queue.head + UDP_SEND_SIZE - QUEUE_SIZE);
		}
		
		/**��ȡ����ͷ�������¶���*/
		Can_Head = can_queue.head;
		can_queue.head = can_queue.tail; 
		
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		/* ���͵�Զ�̷�����*/
		rsi_send_ludp_data(socketDescriptor_txrx, &can_queue.arr[Can_Head],Can_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
		
		DATA_AUTO_CHECK_EN = temp;
		delay_ms(5);
		temp = DATA_AUTO_CHECK_EN;
		DATA_AUTO_CHECK_EN = 0;
		/* CAN���ݷ��͵�������*/
		rsi_send_ludp_data(localSocketDescriptor_txrx, &can_queue.arr[Can_Head],Can_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
		DATA_AUTO_CHECK_EN = temp;
	}
	/* CAN���������ݴ洢ʱ���������usΪ��λ���ͷ�����*/
	if(queue_length(can_queue) > 0){
		Can_Length = queue_length(can_queue);
		if(((SYSTEMTIME - ((u32)(can_queue.arr[can_queue.head+1]&0xff)|((u32)(can_queue.arr[can_queue.head+2]&0xff)<<8)|((u32)(can_queue.arr[can_queue.head+3]&0xff)<<16)
			      |((u32)(can_queue.arr[can_queue.head+4]&0xff)<<24)))*(TIM4_ARR + 1)*(TIM4_PSC + 1) / TIM3_4_PCLK_MHZ) > CAN_OVERTIME_SEND_TIME){
			/* CAN Queue����֡ͷ*/
			queue_addtime_addIO(&can_queue,Can_Length, nodeId, DIGITAL_INPUT1,DIGITAL_INPUT2,CAN_DATA_PACKAGE);
			
			/* ����ֳ����Σ���ǰ��һ�θ��Ƶ�����*/
			if(can_queue.head + Can_Length > QUEUE_SIZE ) {
				queue_oversize(&can_queue,can_queue.head + Can_Length + PACKAGE_HEAD_FRAME_LENGTH - QUEUE_SIZE);
			}
			
			/**��ȡ����ͷ�������¶���*/
			Can_Head = can_queue.head;
			can_queue.head = can_queue.tail; 
			
			temp = DATA_AUTO_CHECK_EN;
			DATA_AUTO_CHECK_EN = 0;
			/* ���͵�Զ�̷�����*/
			rsi_send_ludp_data(socketDescriptor_txrx, &can_queue.arr[Can_Head],Can_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)destIp_txrx, destSocket_txrx, &bytes_sent);
			
			DATA_AUTO_CHECK_EN = temp;
			delay_ms(5);
			temp = DATA_AUTO_CHECK_EN;
			DATA_AUTO_CHECK_EN = 0;
			/* CAN���ݷ��͵�������*/
			rsi_send_ludp_data(localSocketDescriptor_txrx, &can_queue.arr[Can_Head],Can_Length+PACKAGE_HEAD_FRAME_LENGTH, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
			DATA_AUTO_CHECK_EN = temp;					
		}
	}
	return 1;
}

/**
  * @brief  �����
  * @param  ���յ��������׵�ַ
  * @retval �Ƿ���Ҫ������Ϣ����λ����NOT_NEED_RETURN_INFO �� NEED_RETURN_INFO��
  */
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
				TEST_LED_CONV();
			}
			break;
		case 	GET_TIME_SYNC_MAIN_CLOCK:
			memcpy(&YYMMDD,&arr[1],4);
			memcpy(&SYSTEMTIME,&arr[5],4);
			Time_Sync_Flag = 1;
			TEST_LED_CONV();
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
		//! ͨ��ģʽѡ��
		case GET_CHANNEL_MODEL:         
			if(arr[1]==nodeId)
				setAdcModel(&arr[2]);
			else  //�������Ǹ����豸�ģ��򷵻�0����ʾ����Ҫ����ack
				return NOT_NEED_RETURN_INFO;
			break;
		//! [CMD,nodeId,CANx,FREQ,FilterListNum,[FilterList]]
		//! ������ڵ�ID��CANX��ͨ��Ƶ��10KHz������ID��Ŀ������ID�б�
		case GET_CAN_FILTER_ID:
			if(arr[1] == nodeId){
				if(arr[2] == CAN1_ID){
					if(arr[3] == 50){ //500KHz
						CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal,(u32*)&arr[5],arr[4]);   //500K
					}else if(arr[3] == 25){ //250KHz
						CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,12,CAN_Mode_Normal,(u32*)&arr[5],arr[4]);   //250K
					}
				}else if(arr[2] == CAN2_ID){
					if(arr[3] == 50){ //500KHz
						CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal,(u32*)&arr[5],arr[4]);   //500K
					}else if(arr[3] == 25){ //250KHz
						CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,12,CAN_Mode_Normal,(u32*)&arr[5],arr[4]);   //250K
					}
				}
			}
			else{
				return NOT_NEED_RETURN_INFO;
			}
			break;
		case GET_REMOTE_IP_PORT:            //������ַ
			memcpy(destIp_txrx,&arr[1],4);
			memcpy(&destSocket_txrx,&arr[5],2);
			rsi_socket_close(socketDescriptor_txrx, moduleSocket_txrx);//�رյ�ԭ����Զ�̷�������socket
			OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);
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


/**
  * @brief  ��UDP�׽���
  * @param  destIp��Զ��IP
  * @param  destSocket��Զ�̶˿�
  * @param  moduleSocket�����豸ģ��˿�
  * @param  socketDescriptor���׽�������
  * @retval ��UDP�ɹ����1�����ɹ���0���ɹ���
  */
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

/**
  * @brief  ��TCP�׽���
  * @param  destIp��Զ��IP
  * @param  destSocket��Զ�̶˿�
  * @param  moduleSocket�����豸ģ��˿�
  * @param  socketDescriptor���׽�������
  * @retval ��UDP�ɹ����1�����ɹ���0���ɹ���
  */
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
/**
  * @brief  ͬ��ʱ��ģ�鷢��ͬ��ʱ���ź�
  * @param  None
  * @retval None
  */
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
}
#endif
