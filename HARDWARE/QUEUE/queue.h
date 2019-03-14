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
  * @brief  ��������
  * @{
  */ 
#define  UDP_SEND_SIZE          (8*60 + PACKAGE_HEAD_FRAME_LENGTH)    /*!< UDP���ݰ�����С   616 = 8 * 67 + PACKAGE_HEAD_FRAME_LENGTH>*/
#define  QUEUE_SIZE             700                                 /*!< ������������>*/
#define  ARR_SIZE               (QUEUE_SIZE + UDP_SEND_SIZE+50)      /*!< �����������>*/
/**
  * @}
  */
  
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
  * @brief  ����
  * @{
  */
typedef struct{
    u8 arr[ARR_SIZE];
	u32 YYYY_MM_DD;
	u32 HeadTime;
	volatile u16 head,tail;
	u8 test_name[MAX_TEST_NAME_LENGTH];   
}Queue;
/**
  * @}
  */




void queue_init(volatile Queue * pQueue);
u8 queue_get(volatile Queue * pQueue);
void queue_put(volatile Queue * pQueue, u8 ch);
void queue_addtime_addIO(volatile Queue * pQueue, u32 count, u8 nodeId, u8 IO_input1, u8 IO_input2);
void queue_oversize(volatile Queue * pQueue,u32 length);
void queue_clear(volatile Queue * pQueue);
u8 queue_empty(volatile Queue queue);
u8 queue_full(volatile Queue queue);
u32 queue_length(volatile Queue queue);
static void queue_add_name(volatile Queue * pQueue,volatile u8 testname[MAX_TEST_NAME_LENGTH]);

#endif
