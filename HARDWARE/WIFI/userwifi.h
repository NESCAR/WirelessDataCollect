#ifndef _USERWIFI_H_
#define _USERWIFI_H_
#include "sys.h"
#include "queue.h"
 
#define  UDP_SEND_SIZE          546  // ��ò�Ҫ����548    546 = 8 * 67 + 10

#define  IAM_MASTER_CLOCK       1  //�ǲ�����ʱ���豸

#define  GET_TIME_SYNC          0xa1
#define  GET_WIFI_SEND_EN       0xa5
#define  GET_WIFI_SEND_DISABLE  0xa6
#define  GET_CHANNEL_MODEL      0xa7
#define  GET_CAN_SEND_EN        0xa8
#define  GET_REMOTE_IP_PORT     0xa9


extern u32 SYSTEMTIME;
extern u32  YYMMDD;
extern u8 Time_Sync_Flag;
extern u8 Wifi_Send_EN;
extern u8 CAN_Send_EN;

extern Queue adc_queue;	 //adc���ջ���

extern char* destIp_txrx;    //�����շ� IP
extern char* destIp_sync;  //ͬ��    IP

extern unsigned short destSocket_txrx;
extern unsigned short moduleSocket_txrx;

extern unsigned short destSocket_sync;
extern unsigned short moduleSocket_sync;

extern unsigned short socketDescriptor_txrx;
extern unsigned short socketDescriptor_sync;

void receive_udp_package(void);

u8 OpenLudpSocket(char* destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor);
u8 wifi_send_package(void);
u8 order_anay(u8 arr[]);

void Send_Sync_Time(void);

#endif


