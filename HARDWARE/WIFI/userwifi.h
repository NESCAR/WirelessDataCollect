/**
  ******************************************************************************
  * @file    userwifi.h
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   wifi application functions   WiFiģ��Ӧ�÷���
  ******************************************************************************
  */

#ifndef _USERWIFI_H_
#define _USERWIFI_H_

//-------------include---------------//
#include "sys.h"
#include "queue.h"
#include "config.h"

//!�ǲ�����ʱ���豸
#define  IAM_MASTER_CLOCK       0  


/** 
  * @brief  ��λ��������պ��Ƿ���Ҫ����
  * @{
  */ 
typedef enum{
	NOT_NEED_RETURN_INFO = 0,/*!< ��Ҫ������Ϣ>*/
	NEED_RETURN_INFO         /*!< ����Ҫ������Ϣ>*/
}CMD_GetState;
/**
  * @}
  */


/** @defgroup Ĭ�ϵ�Զ�̷�����IPV4��ַ
  * @{
  */
#define DESTIP_TXRX_GROUP1 115    /*!< IP ��1��>*/
#define DESTIP_TXRX_GROUP2 159    /*!< IP ��2��>*/
#define DESTIP_TXRX_GROUP3 154    /*!< IP ��3��>*/
#define DESTIP_TXRX_GROUP4 160    /*!< IP ��4��>*/
/**
  * @}
  */

/** @defgroup APģʽ����
  * @{
  */
#define AP_MODE_SOCKET_DEST_IP       "192.168.100.2"    /*!< APģʽ�������ߵ�IP>*/
#define AP_MODE_SOCKET_DEST_PORT     5002               /*!< APģʽ�������ߵĶ˿�>*/
#define AP_MODE_SOCKET_MODULE_PORT   5001               /*!< APģʽ�±��豸�Ķ˿�>*/
/**
  * @}
  */
  
/** @defgroup ��ʱ����
  * @{
  */
#define CAN_OVERTIME_SEND_TIME       600000    /*!< 8000ms>*/
/**
  * @}
  */  

/** @defgroup ��λ�����UDP���ݰ���1���ֽ�
  * @{
  */
#define  GET_TIME_SYNC_PC           0xa1   /*!< ������λ����ͬ���ź�>*/
#define  RETURN_INFO			    0xa2   /*!< ������Ϣ>*/
#define  GET_TEST_NAME              0xa3   /*!< ��ȡ��������>*/
#define  GET_TIME_SYNC_MAIN_CLOCK   0xa4   /*!< ����������ͬ���ź�>*/
#define  GET_WIFI_SEND_EN           0xa5   /*!< ���ݲɼ��ͷ���ʹ��>*/
#define  GET_WIFI_SEND_DISABLE      0xa6   /*!< ���ݲɼ��ͷ���ʧ��>*/
#define  GET_CHANNEL_MODEL          0xa7   /*!< ����ͨ���ĵ���/��ѹģʽ>*/
#define  GET_CAN_FILTER_ID          0xa8   /*!< CAN���ݲɼ���ID�б�����>*/
#define  GET_REMOTE_IP_PORT         0xa9   /*!< ���÷������ݶ���˽���Ʒ�������IP>*/
#define  PAGING                     0xaa   /*!< Ѱ���ź�>*/
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
  
/** @defgroup CANʹ��λ
  * @{
  */
#define CAN1_ENABLE_BIT_SLC         0x01  /*!< CAN1ʹ��λ������0λ>*/
#define CAN2_ENABLE_BIT_SLC         0x02  /*!< CAN2ʹ��λ������1λ>*/
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
extern volatile u8    Wifi_Send_EN;
//!CAN�ɼ�ʹ��
extern u8    CAN_Get_EN;
//!ADC�ɼ�ʹ��
extern u8    ADC_Get_EN;

//!adc���ջ���
extern Queue adc_queue;	
//!can���ջ���
extern Queue can_queue;

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

//!�Ƿ����ж����Զ�check����
extern u8 DATA_AUTO_CHECK_EN;

void receive_udp_package(void);
void wifi_send_package_test(void);
u8 OpenLudpSocket(u8* destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor);
u8 OpenTcpSocket(u8 *destIp,unsigned short destSocket,unsigned short moduleSocket,unsigned short * socketDescriptor);
u8 wifi_send_package(void);
u8 order_anay(u8 arr[]);
void Send_Sync_Time(void);
#endif


