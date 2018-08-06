#ifndef __IIC_H
#define __IIC_H
#include "led.h"
#define IIC_SCL_H		GPIO_SetBits(GPIOB, GPIO_Pin_15)//WB_Gpio_Put(33,GPIO_HIGH)
#define IIC_SCL_L		GPIO_ResetBits(GPIOB, GPIO_Pin_15)//WB_Gpio_Put(33,0)

#define IIC_SDA_H		GPIO_SetBits(GPIOB, GPIO_Pin_14)//WB_Gpio_Put(32,GPIO_HIGH)
#define IIC_SDA_L		GPIO_ResetBits(GPIOB, GPIO_Pin_14)//WB_Gpio_Put(32,0)

#define READ_SDA    	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//WB_Gpio_Get(32)//����SDA 

//#define SDA_IN()		WB_Gpio_Init(32,GPIO_INPUT,GPIO_HIGH); 
//#define SDA_OUT()		WB_Gpio_Init(32,GPIO_OUTPUT,GPIO_HIGH); 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  
#endif
















