#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "string.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define USART_BUFF_LEN  		250  	//�������buff����
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define PRINT_UART_LOG          1       //�����־������	
typedef struct{
	//���ջ���,���USART_REC_LEN���ֽ�.
	u8 buff[USART_BUFF_LEN];
	u8 CmdCompleteFlag;
	//����״̬
	//bit15��	������ɱ�־
	//bit14��	���յ�0x0d
	//bit13~0��	���յ�����Ч�ֽ���Ŀ
    int head,tail;
}CMD_QUEUE;

extern CMD_QUEUE CMD_RX_BUF;
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);


//uart���д���
void uart_queue_put(volatile CMD_QUEUE * pQueue, u8 ch);
void uart_queue_put_many(volatile CMD_QUEUE * pQueue, u8 * ch,u16 len);//put���
u8 uart_queue_pop(volatile CMD_QUEUE * pQueue);
u16 uart_queue_length(volatile CMD_QUEUE * pQueue);
void uart_queue_clear(volatile CMD_QUEUE * pQueue);


#endif


