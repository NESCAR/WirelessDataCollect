#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 
#include "queue.h"
// ADC_INT PB5 
// SCL PB6
// SDA PB7 	
 	   		   
//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB7����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB7���ģʽ
//IO��������	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 
#define ADC_DISANBLE  PBin(5)  // INIT
//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

// 
extern Queue adc_queue;

//ADS7924 �Ĵ�������
// AO = GND
#define ADS7924W_Adress 0x90  //д��ַ
#define ADS7924R_Adress 0x91  //����ַ


#define MODECNTRL_Adress 0x00
#define MODECNTRL_Reg    0xFC//0xFC

#define SLPCONFIG_Adress 0x13
#define SLPCONFIG_Reg    0x20

#define ACQCONFIG_Adress 0x14
#define ACQCONFIG_Reg 0x1f

// Tacq= 0x1f*2+6 = 68us
// Tconv = 4 us
//SLPTime = 2.5ms/4
// Tup = 0
// t=4*(Tacq+Tconv+Tup)+SLPTime


#define DATA0_U_Adress 0x02
u8 ADS8266_config(void);
u8 ADS8266_read(void);
#endif
















