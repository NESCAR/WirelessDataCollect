#ifndef __LED_H
#define __LED_H
#include "sys.h"



//ĸ��LED�˿ڶ���
#define LED1_PORT GPIOB
#define LED1_PIN GPIO_Pin_3
#define LED2_PORT GPIOD
#define LED2_PIN GPIO_Pin_2
#define LED1 PBout(3)	// LED1
#define LED1_OFF(x) LED1=x //�Ƿ�ر�LED1
#define LED2 PDout(2)	// LED2	
#define LED2_OFF(x) LED2=x //�Ƿ�ر�LED2
//�Ӱ�LED�˿ڶ���
#define LED3_PORT GPIOC
#define LED3_PIN GPIO_Pin_0
#define LED4_PORT GPIOC
#define LED4_PIN GPIO_Pin_1
#define LED3 PCout(0)	// LED1
#define LED3_OFF(x) LED3=x //�Ƿ�ر�LED1
#define LED4 PCout(1)	// LED2	
#define LED4_OFF(x) LED4=x //�Ƿ�ر�LED2

//��Դͨ��оƬshutdown�˿ڶ��壨�͵�ƽ��Ч��
#define V5V_SHUTOWN_PORT GPIOC
#define V5V_SHUTOWN_PIN GPIO_Pin_13
#define V24V_SHUTOWN_PORT GPIOB
#define V24V_SHUTOWN_PIN GPIO_Pin_14
#define V5V_SHUTOWN(x) PCout(13)=~x //x=1���رյ�Դ��x=0��������Դ
#define V24V_SHUTOWN(x) PBout(14)=~x //x=1���رյ�Դ��x=0��������Դ

//ģ���ź�����ģʽѡ��
#define CH1_PORT GPIOA
#define CH1_PIN GPIO_Pin_15
#define CH2_PORT GPIOC
#define CH2_PIN GPIO_Pin_10
#define CH3_PORT GPIOC
#define CH3_PIN GPIO_Pin_12
#define CH4_PORT GPIOC
#define CH4_PIN GPIO_Pin_11
#define Channel1 PAout(15)
#define Channel2 PCout(10)
#define Channel3 PCout(12)
#define Channel4 PCout(11)


//�����źŵ�ƽ���
#define DIG1_PORT GPIOA
#define DIG1_PIN GPIO_Pin_1
#define DIG2_PORT GPIOA
#define DIG2_PIN GPIO_Pin_0
#define DIGITAL_INPUT1 GPIO_ReadInputDataBit(DIG1_PORT,DIG1_PIN)
#define DIGITAL_INPUT2 GPIO_ReadInputDataBit(DIG2_PORT,DIG2_PIN)




void GP_IO_Init(void);//��ʼ��	
void Channel_model(u8 modle[]);
#endif
