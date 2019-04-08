/**
  ******************************************************************************
  * @file    usart.c
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    8-April-2019
  * @brief   usart function   ���ں���
  ******************************************************************************
  */

//-------------include---------------//
#include "sys.h"
#include "usart.h"
#include "config.h"

/* ֧��printf���������*/  
#if 1
#pragma import(__use_no_semihosting)                            
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
/* ���ʹ���˽���*/
#if EN_USART1_RX    
//! ��������Ļ�����
CMD_QUEUE CMD_RX_BUF = {.CmdCompleteFlag = CMD_NOT_COMPLETED,.head = 0,.tail = 0};     

/**
  * @brief  ���ڳ�ʼ��
  * @param  bound��������
  * @retval None
  */
void uart_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);     //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);    //ʹ��USART1ʱ��

	/* ����1��Ӧ���Ÿ���ӳ��*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1

	/* USART1�˿�����*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);                   //��ʼ��PA9��PA10

	/* USART1 ��ʼ������*/
	USART_InitStructure.USART_BaudRate = bound;                                          //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                          //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                               //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                                  //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                     //�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);                                            //��ʼ������1

	USART_Cmd(USART1, ENABLE);                                                           //ʹ�ܴ���1 

	/* ʹ�ܴ��ڽ���*/
	#if EN_USART1_RX	
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);              //��������ж�

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;           //����1�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;     //��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		        //�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			        //IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	                            //����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	#endif
	
}

/**
  * @brief  �����жϷ�����
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	static u8 ResLast;
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART1->DR;
		if(CMD_RX_BUF.CmdCompleteFlag == CMD_NOT_COMPLETED)  //һ��ֻ�ܴ���һ��ָ��
		{
			if(Res == '\n' && ResLast == '\r')//һ��ָ�������
			{
				CMD_RX_BUF.CmdCompleteFlag=CMD_COMPLETED;
			}
			uart_queue_put(&CMD_RX_BUF,Res);			
		}
		
		ResLast = Res;

    }
}
#endif	

/**
  * @brief  put�������ݶ���
  * @param  pQueue��Ŀ�����
  * @param  ch��Ҫ������ַ�
  * @retval None
  */
void uart_queue_put(volatile CMD_QUEUE * pQueue, u8 ch)
{
    pQueue->buff[pQueue->tail] = ch;
	pQueue->tail = (pQueue->tail + 1) % USART_REC_LEN;
}

/**
  * @brief  put����������ݶ���
  * @param  pQueue��Ŀ�����
  * @param  ch��Ҫ������׸��ַ���ַ
  * @param  len��Ҫ������ַ�����
  * @retval None
  */
void uart_queue_put_many(volatile CMD_QUEUE * pQueue, u8 * ch,u16 len)
{
	for(u16 i=0;i<len;i++){;
		 uart_queue_put(pQueue,*(ch+i));
	}
}

/**
  * @brief  ���ݳ�����
  * @param  pQueue��Ŀ�����
  * @retval None
  */
u8 uart_queue_pop(volatile CMD_QUEUE * pQueue)
{
    u8 data = pQueue->buff[pQueue->head];
    pQueue->head = (pQueue->head + 1) % USART_REC_LEN;
    return data;
}

/**
  * @brief  ��ȡ���еĳ���
  * @param  pQueue��Ŀ�����
  * @retval ���еĳ���
  */
u16 uart_queue_length(volatile CMD_QUEUE * pQueue)
{
	return (pQueue->tail-pQueue->head+USART_REC_LEN)%USART_REC_LEN;
}

/**
  * @brief  ��ն���
  * @param  pQueue��Ŀ�����
  * @retval None
  */
void uart_queue_clear(volatile CMD_QUEUE * pQueue)
{
	pQueue->CmdCompleteFlag = CMD_NOT_COMPLETED;
	pQueue->head = pQueue->tail;
}
