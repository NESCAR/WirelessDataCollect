#ifndef __CONFIG_H__
#define __CONFIG_H__ 
#include "sys.h"
#include "usart.h"

/*
��flashʱ���״̬
*/
typedef enum
{ 
	LOAD_PARA_CRC_ERROR = 0,
	LOAD_PARA_TOO_LONG_ERROR, //̫��
	LOAD_PARA_SUCCESS
	
}FLASH_Load_Status;

/*
дflashʱ���״̬
*/
typedef enum
{ 
    WRITE_PARA_FAILED = 0,//����ʧ��
	WRITE_PARA_SUCCESS   //����ɹ�
}FLASH_Write_Status;

/*
дҪ���͵�frameʱ���״̬
*/
typedef enum
{ 
    WRITE_FRAME_JOIN_SSID_TOO_LONG = 0,//SSID̫��
	WRITE_FRAME_PSK_TOO_LONG,  //PSK̫��
	
	WRITE_FRAME_SUCCESS
}FRAME_Write_Status;
/*
flash�ڴ洢���ݵĸ��ַ��ţ������ָ����
*/
#define FLASH_LABLE_TYPE      u8
#define	FLASH_LABEL_END      ('\r')//������־
#define	FLASH_LABEL_SPLIT    ('\n')//�ָ�



//*************************
//����ָ��
//
//*************************
//  !!!!! ע�⣺ÿ������µ�ָ�Ҫ��getHelp���������˵���ĵ�
#define CMD_SET_JOIN_SSID      "SET_RSI_JOIN_SSID"  //����SSID
#define CMD_SET_PSK            "SET_RSI_PSK"  //����PSK

#define CMD_HELP               "HELP"//�������
#define CMD_SAVE_ALL_PARA      "SAVE_ALL_PARA"//�������в���
#define CMD_RESET_SYSTEM       "RESET_SYSTEM"  //ϵͳ����




#define RSI_JOIN_SSID_MAX_LENGTH   64  //�֧�ֵ�wifi����byteλ��
#define RSI_PSK_MAX_LENGTH         34  //�֧�ֵ�wifi����byteλ��
#define PARA_CAT_CH_MAX_LENGTH   (USART_REC_CMD_LEN+USART_REC_VAL_LEN+50)  //�������ϵ�һ���ĳ���u8

#define USART_REC_CMD_LEN  		 100  	//�������cmd�����ֽ���100
#define USART_REC_VAL_LEN  		 100  	//�������Value�����ֽ���100

#define RSI_JOIN_SSID_STRNAME "RSI_JOIN_SSID"
extern u8 RSI_JOIN_SSID[RSI_JOIN_SSID_MAX_LENGTH];  //Ҫ�����ssid
#define RSI_PSK_STRNAME "RSI_PSK"
extern u8 RSI_PSK[RSI_PSK_MAX_LENGTH];  //Ҫ�����ssid��psk@ PSK.If we are using WPA2, this is the key, In open mode NULL

#define FLASH_HEAD_LENGTH_BYTES        4   //flashͷ���洢�������ֽڳ�          
#define FLASH_SAVE_ADDR_MAIN           ADDR_FLASH_SECTOR_5  //���ݱ���Ŀ�ʼ��ַ(��)128KB������������4*16KB+64KB������0-4��
#define FLASH_SAVE_ADDR_BACKUP         ADDR_FLASH_SECTOR_6  //���ݱ��汸�ݿ�ʼ��ַ128KB
#define FLASH_SAVE_SECTOR_MAIN         FLASH_Sector_5
#define FLASH_SAVE_SECTOR_BACKUP       FLASH_Sector_6


#define FLASH_SAVE_BYTE_MAX_LENGTH     (128*1024)//��ౣ��������ֽ���

//usart����Ĵ���
#define CMD_VALUE_SPLIT_CHAR           ' '  //CMD����ֵ�ķָ�
#define CMD_VALUE_NOT_SPLIT_STA        0   //��û�е���ָ��
#define CMD_VALUE_SPLITED_STA          1   //����cmd��value�ָ��

typedef enum
{ 
    CMD_VALUE_SPLIT_ERROR = 0,//splitʧ��
	CMD_VALUE_SPLIT_OK,   //split�ɹ�
	CMD_SPLIT_OK,         //ֻ��cmd
	NONE_CMD_VALUE_MSG   //û��cmd��value��Ϣ
}CMD_VALUE_Split_Status;

typedef enum
{ 
    CMD_NOT_COMPLETED = 0, //cmdָ�û������
	CMD_COMPLETED   //cmdָ���Ѿ�������
}CMD_Receive_Status;


extern u8 catPara[PARA_CAT_CH_MAX_LENGTH];



void writeSectorPara(void);//�������
u8 splitCmd(volatile CMD_QUEUE * pQueue,u8 * pCmd,u8 * pValue);
u8 dealCmdMsg(volatile CMD_QUEUE * pQueue);
u8 loadParaAndCheck(u8 * catPara,u32 startAddr);//��ȡ��У������
u8 updateFrameParas(void);//��������֡�ڵĲ���
void getHelp(void);
#endif

