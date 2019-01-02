#include "led.h" 

s8 channel[4];
   
//LED IO��ʼ��
//LED1 PB4
//LED2 PD2
// Vin Iin modle choose  �ߵ�ƽΪ��ѹ����ģʽ
//EN1 PA15
//EN2 PC10
//EN3 PC11
//EN4 PC12
void LED_GPIO_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	//ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIODʱ��
	
	//�����ź�����
	GPIO_InitStructure.GPIO_Pin = DIG1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DIG1_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = DIG2_PIN;
	GPIO_Init(DIG2_PORT, &GPIO_InitStructure);//��ʼ��	
	//ADG711�Ŀ���ͨ��
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
	//Ĭ��ͨ������
	Channel1=0;Channel2=0;Channel3=0;Channel4=0;
	//LED
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);//��ʼ��
	LED1_OFF(1);LED2_OFF(1);//�ص�
}



void Channel_model(u8 modle[])
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




