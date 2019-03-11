#ifndef _USERWIFI_H_
#define _USERWIFI_H_
#include "sys.h"
#include "queue.h"
 
typedef enum{
	NOT_NEED_RETURN_INFO = 0,
	NEED_RETURN_INFO
}CMD_GetState;
 
#define  UDP_SEND_SIZE          552  // ��ò�Ҫ����548    546 = 8 * 67 + 10

#define  IAM_MASTER_CLOCK       0  //�ǲ�����ʱ���豸

//#define  SEND_WITH_TCP    //��tcp��
#define SEND_WITH_UDP     1

//Զ�̷�����IP
// 115.159.154.160
#define DESTIP_TXRX_GROUP1 115
#define DESTIP_TXRX_GROUP2 159
#define DESTIP_TXRX_GROUP3 154
#define DESTIP_TXRX_GROUP4 160

//���޸�				RSI_DHCP_HOST_NAME


//APģʽ��һЩ����
#define AP_MODE_SOCKET_DEST_IP   "192.168.100.2"
#define AP_MODE_SOCKET_DEST_PORT   5002
#define AP_MODE_SOCKET_MODULE_PORT   5001

#define  GET_TIME_SYNC_PC          0xa1
#define  RETURN_INFO			0xa2
#define  GET_TEST_NAME         0xa3//��ȡ��������
#define  GET_TIME_SYNC_MAIN_CLOCK   0xa4  //����������ͬ���ź�
#define  GET_WIFI_SEND_EN       0xa5
#define  GET_WIFI_SEND_DISABLE  0xa6
#define  GET_CHANNEL_MODEL      0xa7
#define  GET_CAN_SEND_EN        0xa8
#define  GET_REMOTE_IP_PORT     0xa9
#define  PAGING     0xaa  //Ѱ���ź�

extern u32 SYSTEMTIME;
extern u32  YYMMDD;
extern u8 Time_Sync_Flag;
extern u8 Wifi_Send_EN;
extern u8 CAN_Send_EN;

extern Queue adc_queue;	 //adc���ջ���

#define IPV4_LENGTH                 4  //IPV4��byte����
#define IPV4_STR_LENGTH            15  //IPV4��ת��Ϊ�ַ���ռ�õ�byte��������192.169.0.1->"192.168.0.1"��ռ��11λ,���15λ
#define IPV4_1GROUP_STR_LENGTH      3  //IPV4��һ�����ת��Ϊ�ַ���ռ�õ�byte��������192"192"��ռ��3λ

extern u8 destIp_txrx[IPV4_LENGTH];    //�����շ� IP
extern u8 destIp_sync[IPV4_LENGTH];  //ͬ��    IP
extern u8 localDestIp_txrx[IPV4_LENGTH];    //�����շ� IP

extern unsigned short destSocket_txrx;
extern unsigned short moduleSocket_txrx;
extern unsigned short localDestSocket_txrx;
extern unsigned short localModuleSocket_txrx;	

extern unsigned short destSocket_sync;
extern unsigned short moduleSocket_sync;

extern unsigned short socketDescriptor_txrx;
extern unsigned short socketDescriptor_sync;
extern unsigned short localSocketDescriptor_txrx;

void receive_udp_package(void);
void wifi_send_package_test(void);
u8 OpenLudpSocket(u8* destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor);
u8 OpenTcpSocket(u8 *destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor);
u8 wifi_send_package(void);
u8 order_anay(u8 arr[]);
void Send_Sync_Time(void);


#endif


