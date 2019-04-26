/**
  ******************************************************************************
  * @file    config.h
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   parameters configuraton functions   �������÷���
  ******************************************************************************
  */

#ifndef __CONFIG_H__
#define __CONFIG_H__ 

//-------------include---------------//
#include "sys.h"
#include "usart.h"


/** 
  * @brief  ��flashʱ���״̬
  * @{
  */ 
typedef enum
{ 
	LOAD_PARA_CRC_ERROR = 0,  /*!< CRCУ��ʧ��>*/
	LOAD_PARA_TOO_LONG_ERROR, /*!< ����̫��>*/
	LOAD_PARA_POINTER_NULL,   /*!< û���ҵ��ض��ĵ�ַ>*/
	LOAD_PARA_SUCCESS         /*!< ��ȡ�����ɹ�>*/
	
}FLASH_Load_Status;
/**
  * @}
  */

/** 
  * @brief  дflashʱ���״̬
  * @{
  */ 
typedef enum
{ 
    WRITE_PARA_FAILED = 0,/*!< ����ʧ��>*/
	WRITE_PARA_SUCCESS   /*!< ����ɹ�>*/
}FLASH_Write_Status;
/**
  * @}
  */


/** 
  * @brief  дҪ���͵�frameʱ���״̬
  * @{
  */ 
typedef enum
{ 
    WRITE_FRAME_JOIN_SSID_TOO_LONG = 0,/*!< SSID̫��>*/
	WRITE_FRAME_PSK_TOO_LONG,  /*!< PSK̫��>*/
	WRITE_FRAME_SUCCESS  /*!< д�ɹ�>*/
}FRAME_Write_Status;
/**
  * @}
  */

/** 
  * @brief  �������е�״̬
  * @{
  */ 
typedef enum
{ 
    BOARD_INITING = 0,  /*!< ���ڳ�ʼ��>*/
	BOARD_INITED,       /*!< ��ʼ�����>*/
	BOARD_RUNNING,      /*!< ��������������>*/
}BOARD_Status;
/**
  * @}
  */

/** 
  * @brief  flash�ڴ洢���ݵĸ��ַ��ţ������ָ������
  * @{
  */
#define FLASH_LABLE_TYPE              u8        /*!< �ָ�Flash�����ݵķָ����ֽڳ���>*/
#define	FLASH_LABEL_END               ('\r')    /*!< ������־>*/
#define	FLASH_LABEL_SPLIT             ('\n')    /*!< �ָ��������ݡ�IP>*/
#define	IP_GROUP_SPLIT                ('.')     /*!< !!��flash����Ȼʹ��\n!! ��������ʱ�ָ�����IP��С�飬������"SET_SERVER_IP 192.168.0.1">*/
/**
  * @}
  */

/** 
  * @brief  ���崮��ָ�����
  * @note !!! ÿ������µ�ָ�Ҫ��getHelp���������˵���ĵ�
  * @{
  */
/** !���ò���������*/
#define CMD_SET_JOIN_SSID      "SET_RSI_JOIN_SSID"   /*!< ����SSID>*/
#define CMD_SET_PSK            "SET_RSI_PSK"         /*!< ����PSK>*/
#define CMD_SET_NODE_ID        "SET_NODE_ID"         /*!< ���ýڵ�id>*/
#define CMD_SET_SERVER_IP      "SET_SERVER_IP"       /*!< ����˽���Ʒ�����IP>*/
#define CMD_SET_MASTER_CLK     "SET_MASTER_CLK"      /*!< �����Ƿ�����ʱ��>*/
/** !������*/
#define CMD_HELP               "HELP"                /*!< �������>*/
#define CMD_GET_PARA           "GET_PARA"            /*!< ����������>*/
#define CMD_SAVE_ALL_PARA      "SAVE_ALL_PARA"       /*!< �������в���>*/
#define CMD_RESET_SYSTEM       "RESET_SYSTEM"        /*!< ϵͳ����>*/
/**
  * @}
  */
  
/** 
  * @brief  ����������������
  * @{
  */
#define RSI_JOIN_SSID_MAX_LENGTH   64        /*!< �֧�ֵ�wifi�����ֽڳ���>*/
#define RSI_PSK_MAX_LENGTH         34        /*!< �֧�ֵ�wifi�����ֽڳ���>*/
#define NODE_ID_2STR_MAX_LEN       3         /*!< id��ת��Ϊ�ַ����������ȣ���255->"255"����Ӧ�޸�nodeId��������>*/
#define PARA_CAT_CH_MAX_LENGTH     (USART_REC_CMD_LEN+USART_REC_VAL_LEN+50)  /*!< �������ϵ�һ���ĳ���u8>*/

#define RSI_JOIN_SSID_STRNAME      "RSI_JOIN_SSID"   /*!< SSID�ı�����>*/
extern u8 RSI_JOIN_SSID[RSI_JOIN_SSID_MAX_LENGTH];   /*!< ssid����>*/

#define RSI_PSK_STRNAME            "RSI_PSK"    /*!< PSK�ı�����>*/
extern u8 RSI_PSK[RSI_PSK_MAX_LENGTH];          /*!< PSK����>*/

#define NODE_ID_STRNAME            "nodeId"     /*!< �ڵ�ID�ŵı�����>*/
extern u8 nodeId;                               /*!< �ڵ�ID�ţ���Ҫ��Ӧ�޸�NODE_ID_2STR_MAX_LEN>*/

#define SERVER_IP_STRNAME          "destIp_txrx"  /*!< ˽���Ʒ�����IP�ı�����>*/
/**
  * @}
  */

/** 
  * @brief  FLASH���������
  * @note   ����������4*16KB+64KB������0-4��
  * @{
  */
#define FLASH_HEAD_LENGTH_BYTES        4                    /*!< flashͷ���洢�������ֽڳ���  >*/        
#define FLASH_SAVE_ADDR_MAIN           ADDR_FLASH_SECTOR_5  /*!< �������ݵĿ�ʼ��ַ(��) >*/  
#define FLASH_SAVE_ADDR_BACKUP         ADDR_FLASH_SECTOR_6  /*!< �������ݱ��ݿ�ʼ��ַ >*/  
#define FLASH_SAVE_SECTOR_MAIN         FLASH_Sector_5       /*!< ���洢��������5����128KB  >*/ 
#define FLASH_SAVE_SECTOR_BACKUP       FLASH_Sector_6       /*!< ���ݴ洢��������6����128KB  >*/ 
#define FLASH_SAVE_BYTE_MAX_LENGTH     (128*1024)           /*!< ��ౣ��������ֽ���������ÿ��FLASH���������������5��6�����洢�ռ�128KB >*/
/**
  * @}
  */

/** 
  * @brief  USART/APģʽ�µ�����/��Ϣ���ȣ����ָ����
  * @{
  */
#define USART_REC_CMD_LEN  		   100  	     /*!< �������cmd�����ֽ���100��UART��APģʽ���洢�ڸô�>*/
#define USART_REC_VAL_LEN  		   100  	     /*!< �������Value�����ֽ���100��UART��APģʽ���洢�ڸô�>*/

#define CMD_VALUE_SPLIT_CHAR           ' '       /*!< CMD����ֵ�ķָ�>*/
/**
  * @}
  */
 

/** 
  * @brief  ģʽ���ţ����岻ͬ��ģʽ�����ִ���
  * @note   ���豸ֻʹ��CLIENT��APģʽ
  * @{
  */ 
extern u32 RSI_WIFI_OPER_MODE;                /*!< ģʽ>*/
#define RSI_WIFI_CLIENT_MODE_VAL       0      /*!< CLIENT���ͻ��ˣ�ģʽ>*/
#define RSI_WIFI_P2P_MODE_VAL          1      /*!< P2Pģʽ>*/
#define RSI_WIFI_ENTERPRISE_MODE_VAL   2      /*!< ENTERPRISEģʽ>*/
#define RSI_WIFI_AP_MODE_VAL           6      /*!< APģʽ>*/
#define RSI_WIFI_PER_MODE_VAL          8      /*!< PERģʽ>*/
#define RSI_WIFI_CONCURRENT_MODE_VAL   9      /*!< CONCURRENT��Э�̣�ģʽ>*/
/**
  * @}
  */


/** 
  * @brief  AP����
  * @{
  */ 
#define NODE_AP_NAME_HEAD       "SORL_WIRELESS_NODE_"  /*!< �ڵ���ΪAP��ʱ�������ǰ׺���������nodeId>*/
/**
  * @}
  */
  
/** 
  * @brief  IP����
  * @{
  */ 
#define IPV4_LENGTH                 4  /*!< IPV4���ֽڳ��ȣ���{192.168.3.1}>*/
#define IPV4_STR_LENGTH            15  /*!< IPV4��ת��Ϊ�ַ���ռ�õ�byte��������192.169.0.1->"192.168.0.1"��ռ��11λ,���15λ}>*/
#define IPV4_1GROUP_STR_LENGTH      3  /*!< IPV4��һ�����ת��Ϊ�ַ���ռ�õ�byte��������192"192"��ռ��3λ}>*/
/**
  * @}
  */
  
  
/** 
  * @brief  �Ƿ��Ѿ�����CMD��VAL�ķָ��
  * @{
  */ 
typedef enum{ 
    CMD_VALUE_NOT_SPLIT_STA      =     0,         /*!< δ����ָ��>*/
    CMD_VALUE_SPLITED_STA    					  /*!< ����cmd��value�ָ��>*/
}CMD_VAL_Get_Split_sta;
/**
  * @}
  */

/** 
  * @brief  CMD��VAL�ָ����
  * @{
  */ 
typedef enum
{ 
    CMD_VALUE_SPLIT_ERROR = 0,    /*!< �ָ�ʧ��,��ʾû��ָ����߷ָ�ָ��ʧ��>*/
	CMD_VALUE_SPLIT_OK,           /*!< �ָ�ɹ�>*/
	CMD_SPLIT_OK,                 /*!< ��ȡCMD�ɹ���ֻ�д�����>*/
	NONE_CMD_VALUE_MSG            /*!< **���**û��cmd��value��Ϣ>*/
}CMD_VALUE_Split_Status;
/**
  * @}
  */

/** 
  * @brief  �����Ƿ������ϣ���\r\n������
  * @{
  */ 
typedef enum
{ 
    CMD_NOT_COMPLETED = 0,   /*!< cmdָ�û������>*/
	CMD_COMPLETED            /*!< cmdָ���Ѿ�������>*/
}CMD_Receive_Status;
/**
  * @}
  */

//!������Ҫ���浽Flash�е����ݣ��������ݳ��ȡ��������ָ�������������CRC��
extern u8 catPara[PARA_CAT_CH_MAX_LENGTH];
//!�ڵ�ID
extern u8 nodeId;
//!��̬��ַ �� DHCP��̬��ַ��һ��APģʽ�¾�̬��CLIENTģʽ��DHCP
extern u8 RSI_IP_CFG_MODE;
//!����֧��2.4GHz��5GHz��˫Ƶ
extern u8 RSI_BAND;
//!�ڵ�����״̬  @enum BOARD_Status
extern u8 BOARD_STA;
//!�ڵ�ĵ�ǰIP
extern u8 localhost[IPV4_LENGTH];
//!���ǲ�����ʱ��
extern u8 IAM_MASTER_CLOCK;

void writeSectorPara(void);
u8 splitCmd(volatile CMD_QUEUE * pQueue,u8 * pCmd,u8 * pValue);
void dealCmdMsg(volatile CMD_QUEUE * pQueue);
u8 loadParaAndCheck(u8 * catPara,u32 startAddr);
u8 updateFrameParas(void);
void getHelp(void);
void getPara(void);
void setFactory(void);
void loadParafromMainOrBackupFlash(void);
void setBoardSta(u8 sta);
u32 getParaLen(void);
#endif

