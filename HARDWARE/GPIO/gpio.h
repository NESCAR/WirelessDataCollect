/**
  ******************************************************************************
  * @file    gpio.h
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   GPIO configuration functions   GPIO���÷���
  ******************************************************************************
  */

#ifndef __GPIO_H
#define __GPIO_H

//-------------include---------------//
#include "sys.h"

/** 
  * @brief  �Ӱ�LED�˿ڶ���
  * @{
  */ 
#define LED1_PORT    GPIOB
#define LED1_PIN     GPIO_Pin_6
#define LED2_PORT    GPIOB
#define LED2_PIN     GPIO_Pin_7
#define LED3_PORT    GPIOB
#define LED3_PIN     GPIO_Pin_5

#define LED1         PBout(6)
//!LED1�ر�
#define LED1_OFF(x)  (LED1 = x) 
//!LED1ȡ��
#define LED1_CONV()  (LED1 = ~LED1)

#define LED2         PBout(7)
//!LED2�ر�
#define LED2_OFF(x)  (LED2 = x)
//!LED2ȡ��
#define LED2_CONV()  (LED2 = ~LED2)

#define LED3         PBout(5)
//!LED3�ر�
#define LED3_OFF(x)  (LED3 = x)
//!LED3ȡ��
#define LED3_CONV()  (LED3 = ~LED3)
/**
  * @}
  */

/** 
  * @brief  ����������
  * @{
  */ 
#define BEEP_PORT           GPIOB
#define BEEP_PIN            GPIO_Pin_1

#define BEEP                PBout(1)
#define BEEP_ON(x)          (BEEP = x)
#define BEEP_CONV()         (BEEP = ~BEEP)
/**
  * @}
  */
  
/** 
  * @brief  ��Դͨ��оƬshutdown�˿ڶ��壨�͵�ƽ��Ч��
  * @{
  */ 
#define V24V_SHUTOWN_PORT   GPIOB
#define V24V_SHUTOWN_PIN    GPIO_Pin_14
//!������24V��Դ��x=1���رյ�Դ��x=0��������Դ
#define V24V_SHUTOWN(x)     PBout(14)=~x
/**
  * @}
  */

/** 
  * @brief  ģ���ź�����ģʽѡ��
  * @{
  */
#define CH1_PORT          GPIOA
#define CH1_PIN           GPIO_Pin_15
#define CH2_PORT          GPIOC
#define CH2_PIN           GPIO_Pin_10
#define CH3_PORT          GPIOC
#define CH3_PIN           GPIO_Pin_12
#define CH4_PORT          GPIOC
#define CH4_PIN           GPIO_Pin_11
#define Channel1          PAout(15)
#define Channel2          PCout(10)
#define Channel3          PCout(12)
#define Channel4          PCout(11)
#define ADC_CURRENT_MODE  0
#define ADC_VOLTAGE_MODE  1
/**
  * @}
  */


/** 
  * @brief  ģ�����źŵ�ƽ���
  * @{
  */
#define DIG1_PORT       GPIOB
#define DIG1_PIN        GPIO_Pin_3
#define DIG2_PORT       GPIOD
#define DIG2_PIN        GPIO_Pin_2
#define DIGITAL_INPUT1  ((~GPIO_ReadInputDataBit(DIG1_PORT,DIG1_PIN)) & 0x01)//��·�л�Ѹ߶�ȡΪ0���Ͷ�ȡΪ1����ȡ��
#define DIGITAL_INPUT2  ((~GPIO_ReadInputDataBit(DIG2_PORT,DIG2_PIN)) & 0x01)
/**
  * @}
  */

void GP_IO_Init(void);//��ʼ��	
void setAdcModel(u8 modle[]);
#endif
