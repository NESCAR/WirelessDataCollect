/**
  ******************************************************************************
  * @file    usart.h
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    8-April-2019
  * @brief   usart function   ���ں���
  ******************************************************************************
  */

#ifndef __USART_H
#define __USART_H

//-------------include---------------//
#include "stdio.h"	
#include "string.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

/** 
  * @brief  ��������
  * @{
  */
#define USART_REC_LEN  			200  	/*!< �����������ֽ���>*/
#define USART_BUFF_LEN  		250  	/*!< �������buff����>*/
#define EN_USART1_RX 			  1		  /*!< ʹ�ܣ�1��/��ֹ��0������1����>*/
#define PRINT_UART_LOG      1     /*!< �����־������	>*/
/**
  * @}
  */
	
/** 
  * @brief  ��������ṹ��
  * @{
  */
typedef struct{
	//���ջ���,���USART_REC_LEN���ֽ�.
	u8 buff[USART_BUFF_LEN];
	u8 CmdCompleteFlag;
  int head,tail;
}CMD_QUEUE;
/**
  * @}
  */

//! ��������Ļ�����
extern CMD_QUEUE CMD_RX_BUF;



void uart_init(u32 bound);
void uart_queue_put(volatile CMD_QUEUE * pQueue, u8 ch);
void uart_queue_put_many(volatile CMD_QUEUE * pQueue, u8 * ch,u16 len);//put���
u8 uart_queue_pop(volatile CMD_QUEUE * pQueue);
u16 uart_queue_length(volatile CMD_QUEUE * pQueue);
void uart_queue_clear(volatile CMD_QUEUE * pQueue);


#endif


