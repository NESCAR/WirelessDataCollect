#ifndef __LED_H
#define __LED_H
#include "sys.h"



//LED�˿ڶ���
#define LED1 PCout(10)	// DS0
#define LED2 PDout(2)	// DS1	

//ģ���ź�����ģʽѡ��
#define Channel1 PCout(6)
#define Channel2 PCout(7)
#define Channel3 PCout(8)
#define Channel4 PCout(9)


//�����źŵ�ƽ���
#define DIGITAL_INPUT1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)
#define DIGITAL_INPUT2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)


void LED_GPIO_Init(void);//��ʼ��	
void Channel_model(u8 modle[]);
#endif
