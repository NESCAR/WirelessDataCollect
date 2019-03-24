/**
  ******************************************************************************
  * @file    gpio.c
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    24-March-2019
  * @brief   GPIO configuration functions   GPIO���÷���
  ******************************************************************************
  */

//-------------include---------------//
#include "gpio.h" 

//!ͨ����״̬��1����ѹģʽ��0������ģʽ
s8 channel[4];
/**
  * @brief  ����GPIO��ʼ��
  * @param  None
  * @retval None
  */
void GP_IO_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	/*ʱ��ʹ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIODʱ��
	
	/*�����ź�����*/
	GPIO_InitStructure.GPIO_Pin = DIG1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DIG1_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = DIG2_PIN;
	GPIO_Init(DIG2_PORT, &GPIO_InitStructure);//��ʼ��	
	/*ADG711�Ŀ���ͨ��*/
	GPIO_InitStructure.GPIO_Pin = CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(CH1_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = CH2_PIN;
	GPIO_Init(CH2_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = CH3_PIN;
	GPIO_Init(CH3_PORT, &GPIO_InitStructure);//��ʼ��	
	GPIO_InitStructure.GPIO_Pin = CH4_PIN;
	GPIO_Init(CH4_PORT, &GPIO_InitStructure);//��ʼ��
	/*Ĭ��ͨ������*/
	Channel1=ADC_VOLTAGE_MODE;Channel2=ADC_VOLTAGE_MODE;
	Channel3=ADC_VOLTAGE_MODE;Channel4=ADC_VOLTAGE_MODE;
	/*�Ӱ�LED*/
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = LED3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(LED3_PORT, &GPIO_InitStructure);//��ʼ��
	LED1_OFF(1);//����ָʾ��
	LED2_OFF(1);LED3_OFF(1);//�ص�
	/*��Դ��ͨ�ϣ��͵�ƽ��Ч(�ص�Դ)*/
	GPIO_InitStructure.GPIO_Pin = V24V_SHUTOWN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(V24V_SHUTOWN_PORT, &GPIO_InitStructure);//��ʼ��
	V24V_SHUTOWN(0);//Ĭ�Ͽ�����Դ
}


/**
  * @brief  ����ͨ��ģʽ
  * @param  None
  * @retval None
  */
void setAdcModel(u8 modle[])
{		
	Channel1 = modle[0];
	channel[0] = modle[0];
	Channel2 = modle[1];
	channel[1] = modle[1];
	Channel3 = modle[2];
	channel[2] = modle[2];
	Channel4 = modle[3];
	channel[3] = modle[3];
}




