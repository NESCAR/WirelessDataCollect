/**
  ******************************************************************************
  * @file    queue.c
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   adc data queue configuration functions   ADC���ݶ��з���
  ******************************************************************************
  */

//-------------include---------------//
#include "queue.h"
#include "usart.h"
#include <string.h>

//! ����ʵ������
volatile u8 test_name[MAX_TEST_NAME_LENGTH] = DEFAULT_TEST_NAME;  
/**
  * @brief  ��������
  * @param  pQueue������
  * @retval None
  */
void queue_init(volatile Queue * pQueue)
{
    memset((void *)pQueue, 0, sizeof(Queue));
}

/**
  * @brief  ��ȡ����ͷ������
  * @param  pQueue������
  * @retval ��ͷ�ֽ�����
  */
u8 queue_get(volatile Queue * pQueue)
{
    u8 data = pQueue->arr[pQueue->head];
    pQueue->head = (pQueue->head + 1) % QUEUE_SIZE;
    return data;
}

/**
  * @brief  u8���ݼ������
  * @param  pQueue������
  * @param  ch��Ҫ�����u8����
  * @retval None
  */
 void queue_put(volatile Queue * pQueue, u8 ch)
{
    pQueue->arr[pQueue->tail] = ch;
	pQueue->tail = (pQueue->tail + 1) % QUEUE_SIZE;
}

/**
  * @brief  u8���ݿ�����queue.arrβ��
  * @param  pQueue������
  * @param  buf��Ҫ�����u8����
  * @param  len��u8���鳤��
  * @retval None
  */
void queue_arr_memcpy(Queue * pQueue, u8 * buf , u8 len){
	if((len + pQueue->tail) <= QUEUE_SIZE){
		memcpy((u8 *)&(pQueue->arr[pQueue->tail]),buf,len);
		
	}else{
		memcpy((u8 *)&(pQueue->arr[pQueue->tail]),buf,QUEUE_SIZE - pQueue->tail);
		memcpy((u8 *)&(pQueue->arr[0]),buf+QUEUE_SIZE - pQueue->tail,len - QUEUE_SIZE + pQueue->tail);
	}
	pQueue->tail = (pQueue->tail + len)  % QUEUE_SIZE;
	
}

/**
  * @brief  ������գ�tail=head
  * @param  pQueue������
  * @retval None
  */
void queue_clear(volatile Queue * pQueue)
{
	pQueue->head = pQueue->tail;
}

/**
  * @brief  �������֡ͷ�����������ա���ʱ����ADC�ֽ����ݡ��ڵ����ݳ��ȡ������źš���������
  * @param  pQueue������
  * @param  count��ADC�ֽڸ�ʽ
  * @param  id���ڵ�ID��
  * @param  IO_input1�������ź�1
  * @param  IO_input2�������ź�2
  * @param  dataType��
           @arg CAN_DATA_PACKAGE
		       @arg ADC_DATA_PACKAGE
  * @retval None
  */
void queue_addtime_addIO(volatile Queue * pQueue, u32 count, u8 id, u8 IO_input1, u8 IO_input2,u8 dataType)
{
	pQueue->head = (pQueue->head  + QUEUE_SIZE -PACKAGE_HEAD_FRAME_LENGTH)% QUEUE_SIZE;  //��ǰ����Ϊ����֡ͷ���ȣ������������ơ�ʱ�䡢IO�ߵ͵�ƽ��
//	printf("Head: %d \r\nTail: %d \r\n\r\n",pQueue->head,pQueue->tail);
	pQueue->arr[(pQueue->head+0)% QUEUE_SIZE] = (u8)(pQueue->YYYY_MM_DD);
	pQueue->arr[(pQueue->head+1)% QUEUE_SIZE] = (u8)(pQueue->YYYY_MM_DD>>8);
	pQueue->arr[(pQueue->head+2)% QUEUE_SIZE] = (u8)(pQueue->YYYY_MM_DD>>16);
	pQueue->arr[(pQueue->head+3)% QUEUE_SIZE] = (u8)(pQueue->YYYY_MM_DD>>24);
	
	pQueue->arr[(pQueue->head+4)% QUEUE_SIZE] = (u8)(pQueue->HeadTime);
	pQueue->arr[(pQueue->head+5)% QUEUE_SIZE] = (u8)(pQueue->HeadTime>>8);
	pQueue->arr[(pQueue->head+6)% QUEUE_SIZE] = (u8)(pQueue->HeadTime>>16);
	pQueue->arr[(pQueue->head+7)% QUEUE_SIZE] = (u8)(pQueue->HeadTime>>24);
	
	pQueue->arr[(pQueue->head+8)% QUEUE_SIZE] = (u8)count;
	pQueue->arr[(pQueue->head+9)% QUEUE_SIZE] = (u8)(count>>8);
	pQueue->arr[(pQueue->head+10)% QUEUE_SIZE] = (u8)(count>>16);
	pQueue->arr[(pQueue->head+11)% QUEUE_SIZE] = (u8)(count>>24);
	
	
	pQueue->arr[(pQueue->head+12)% QUEUE_SIZE] = id;
	pQueue->arr[(pQueue->head+13)% QUEUE_SIZE] = IO_input1 | (IO_input2 << 1);
	pQueue->arr[(pQueue->head+14)% QUEUE_SIZE] = dataType;
	pQueue->arr[(pQueue->head+15)% QUEUE_SIZE] = pQueue->arr[(pQueue->head + 4)% QUEUE_SIZE];//У��λ
	
	queue_add_name(pQueue,test_name);//��ӱ���ʵ������ƣ�����ΪMAX_TEST_NAME_LENGTH

}

/**
  * @brief  ��ĳһ������ӱ���ʵ�������
  * @param  pQueue������
  * @param  testname����������
  * @note   ֻ���ڱ��ļ���ʹ��
  * @retval None
  */
static void queue_add_name(volatile Queue * pQueue, volatile u8 testname[MAX_TEST_NAME_LENGTH])
{
	for(int i = MAX_TEST_NAME_LENGTH;i>0;i--)
	{
		//��ʱ�䡢IO��֡ͷ���濪ʼ�����������
		//��ʱ�䡢IO�ĳ���Ϊ16����ӵ�16����ʼ�����и�ֵtestname
		pQueue->arr[(pQueue->head+PACKAGE_TIME_IO_LENGTH+i-1)% QUEUE_SIZE] = testname[i-1];
	}
}

/**
  * @brief  ������ֱ��д�����ݣ���ͷ���ݰ��Ƶ���β
  * @param  pQueue������
  * @param  length�����ݳ���
  * @retval None
  */
void queue_oversize(volatile Queue * pQueue,u32 length)
{
	memcpy((u8 *)&pQueue->arr[QUEUE_SIZE],(u8 *)&pQueue->arr[0],length);
	
}

/**
  * @brief  �鿴�Ƿ��ǿն���
  * @param  queue������
  * @retval �Ƿ�ն��У�1���ն��У�0���ǿն��У�
  */
u8 queue_empty(volatile Queue queue)
{
    if(queue.head == queue.tail)
        return 1;
    return 0;
}

/**
  * @brief  �鿴���г���
  * @param  queue������
  * @retval ���г���
  */
u32 queue_length(volatile Queue queue)
{
	return (queue.tail-queue.head+QUEUE_SIZE)%QUEUE_SIZE;
}

/**
  * @brief  �鿴�����Ƿ���
  * @param  queue������
  * @retval �Ƿ������У�1�������У�0���������У�
  */
u8 queue_full(volatile Queue queue)
{
    if((queue.tail + 20) % QUEUE_SIZE >= queue.head)
        return 1;
    return 0;
}


