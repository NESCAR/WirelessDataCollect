/**
  ******************************************************************************
  * @file    queue.h
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   adc data queue configuration functions   ADC���ݶ��з���
  ******************************************************************************
  */
#ifndef   QUEUE_H_
#define   QUEUE_H_

//-------------include---------------//
#include <sys.h>

/** 
  * @brief  ������������
  * @{
  */   
#define MAX_TEST_NAME_LENGTH         64       /*!< �������������>*/
#define PACKAGE_TIME_IO_LENGTH       16       /*!< ʱ�䡢IO��ADC�����ȳ���>*/
#define PACKAGE_HEAD_FRAME_LENGTH    (MAX_TEST_NAME_LENGTH+PACKAGE_TIME_IO_LENGTH)   /*!< ֡ͷ�ܳ���>*/
#define DEFAULT_TEST_NAME            "DefaultName/2019-03-09\0"       /*!< Ĭ�ϲ�������>*/
/**
  * @}
  */
  
/** 
  * @brief  ��������
  * @note   ���д洢ADC��CAN���ݣ����ݳ�����ADC��������
  * @{
  */ 
#define  ADC_SEND_SIZE          (8*50 + PACKAGE_HEAD_FRAME_LENGTH)     /*!< ADC���ݰ�����С   480 = 8 * 50 + PACKAGE_HEAD_FRAME_LENGTH  UDP��������Ҫ����576��20��8��548��>*/
#define  CAN_SEND_SIZE          (25 * 16 + PACKAGE_HEAD_FRAME_LENGTH)  /*!< CAN���ݰ�����С   480 = 25* 16 + PACKAGE_HEAD_FRAME_LENGTH  UDP��������Ҫ����576��20��8��548��>*/
#define  QUEUE_SIZE             800                                    /*!< �������������������ADC_SEND_SIZE��һЩ����Ȼ���ڱ��������Ϣ����>*/
#define  ARR_SIZE               (QUEUE_SIZE + ADC_SEND_SIZE+52)        /*!< �����������>*/
/**
  * @}
  */

/** 
  * @brief  ����
  * @{
  */
typedef struct{
    u8 arr[ARR_SIZE];
	u32 YYYY_MM_DD;
	u32 HeadTime;
	volatile u16 head,tail;
}Queue;
/**
  * @}
  */

/** 
  * @brief  CANx_ID���������ֲ�ͬ��CAN���
  * @{
  */
typedef enum{
	CAN1_ID = 0x01,
	CAN2_ID
}CANx;
/**
  * @}
  */

/** 
  * @brief  wifi���ݰ��洢�������ͣ�ADC����CAN
  * @{
  */
typedef enum{
	CAN_DATA_PACKAGE = 0x01,
	ADC_DATA_PACKAGE
}WiFi_DATA_Type;
/**
  * @}
  */

//! ����ʵ������
extern volatile u8 test_name[MAX_TEST_NAME_LENGTH]; 


void queue_init(volatile Queue * pQueue);
u8 queue_get(volatile Queue * pQueue);
void queue_put(volatile Queue * pQueue, u8 ch);
void queue_arr_memcpy(Queue * pQueue, u8 * buf , u8 len);
void queue_addtime_addIO(volatile Queue * pQueue, u32 count, u8 id, u8 IO_input1, u8 IO_input2,u8 dataType);
void queue_oversize(volatile Queue * pQueue,u32 length);
void queue_clear(volatile Queue * pQueue);
u8 queue_empty(volatile Queue * queue);
u8 queue_full(volatile Queue * queue);
u32 queue_length(volatile Queue * queue);
static void queue_add_name(volatile Queue * pQueue,volatile u8 testname[MAX_TEST_NAME_LENGTH]);

#endif
