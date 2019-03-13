#ifndef _USERWIFI_H_
#define _USERWIFI_H_
#include "sys.h"
#include "queue.h"
#include "config.h"

//!UDP���ݰ�����С   546 = 8 * 67 + PACKAGE_HEAD_FRAME_LENGTH
#define  UDP_SEND_SIZE          (8*67+PACKAGE_HEAD_FRAME_LENGTH)
//!�ǲ�����ʱ���豸
#define  IAM_MASTER_CLOCK       0  


/** 
  * @brief  ��λ��������պ��Ƿ���Ҫ����
  */  
typedef enum{
	NOT_NEED_RETURN_INFO = 0,
	NEED_RETURN_INFO
}CMD_GetState;

/** @defgroup ����TCP����UDP���͸�˽���Ʒ�����
  * @hint ���߶���һ���ҽ���һ����TCPδ��֤��������ʹ��
  * @{
  */
//#define  SEND_WITH_TCP  1  //��tcp��������
#define SEND_WITH_UDP     1   //��udp��������
/**
  * @}
  */

/** @defgroup Ĭ�ϵ�Զ�̷�����IPV4��ַ
  * @{
  */
#define DESTIP_TXRX_GROUP1 115
#define DESTIP_TXRX_GROUP2 159
#define DESTIP_TXRX_GROUP3 154
#define DESTIP_TXRX_GROUP4 160
/**
  * @}
  */

/** @defgroup APģʽ����
  * @{
  */
#define AP_MODE_SOCKET_DEST_IP       "192.168.100.2"
#define AP_MODE_SOCKET_DEST_PORT     5002
#define AP_MODE_SOCKET_MODULE_PORT   5001
/**
  * @}
  */

/** @defgroup ��λ������ 
  * @{
  */
#define  GET_TIME_SYNC_PC           0xa1   //������λ����ͬ���ź�
#define  RETURN_INFO			    0xa2   //
#define  GET_TEST_NAME              0xa3//��ȡ��������
#define  GET_TIME_SYNC_MAIN_CLOCK   0xa4  //����������ͬ���ź�
#define  GET_WIFI_SEND_EN           0xa5
#define  GET_WIFI_SEND_DISABLE      0xa6
#define  GET_CHANNEL_MODEL          0xa7
#define  GET_CAN_SEND_EN            0xa8
#define  GET_REMOTE_IP_PORT         0xa9
#define  PAGING                     0xaa  //Ѱ���ź�
/**
  * @}
  */
  
/** @defgroup ����λ���ķ���ָ���
  * @{
  */
#define ANAL_RSP_LENGTH            3
/**
  * @}
  */

//!ϵͳʱ��
extern u32   SYSTEMTIME;
//!������
extern u32   YYMMDD;

//!�Ƿ��Ѿ�ͬ��
extern u8    Time_Sync_Flag;

//!���ݲɼ��ͷ���ʹ��
extern u8    Wifi_Send_EN;
//!CAN�ɼ��ͷ���ʹ��
extern u8    CAN_Send_EN;

//!adc���ջ���
extern Queue adc_queue;	

//!˽���Ʒ�����IP��ַ
extern u8 destIp_txrx[IPV4_LENGTH];
//!ADC����ָ���˽���Ʒ������˿�
extern unsigned short destSocket_txrx;
//!�ڵ�ģ��-˽���Ʒ�����UDP����ӿڵ�����
extern unsigned short socketDescriptor_txrx;
//!�ڵ�ģ�����ݽ��գ�˽���Ʒ��������˿ڣ�����
extern unsigned short moduleSocket_txrx;

//!ͬ��ʱ�Ӷ����IP��ַ��255.255.255.255�㲥
extern u8 destIp_sync[IPV4_LENGTH];
//!ͬ��ʱ�Ӷ���Ķ˿�
extern unsigned short destSocket_sync;
//!ͬ��ʱ��ģ��Ķ˿�
extern unsigned short moduleSocket_sync;
//!�ڵ�ģ��-ͬ��ģ��UDP����ӿڵ�����
extern unsigned short socketDescriptor_sync;

//!��������λ��IP��ַ��255.255.255.255�㲥
extern u8 localDestIp_txrx[IPV4_LENGTH];    //�����շ� IP
//!��������λ���˿�
extern unsigned short localDestSocket_txrx;
//!�ڵ�ģ�����ݽ��գ���λ�����˿ڣ�����
extern unsigned short localModuleSocket_txrx;	
//!�ڵ�ģ��-��������λ��UDP����ӿڵ�����
extern unsigned short localSocketDescriptor_txrx;

void receive_udp_package(void);
void wifi_send_package_test(void);
u8 OpenLudpSocket(u8* destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor);
u8 OpenTcpSocket(u8 *destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor);
u8 wifi_send_package(void);
u8 order_anay(u8 arr[]);
void Send_Sync_Time(void);
#endif


