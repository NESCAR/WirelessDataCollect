#ifndef __CAN1_H
#define __CAN1_H	 
#include "sys.h"	    

/** 
  * @brief  �ж�
  * @{
  */
//CAN1ͨ�ö���
#define RCC_PERIPH_CAN1           RCC_APB1Periph_CAN1   
#define CAN1_PORT_AF              GPIO_AF_CAN1

//CAN1��TX
#define RCC_PERIPH_CAN1_PORT_TX   RCC_AHB1Periph_GPIOA 
#define CAN1_PORT_TX              GPIOA
#define CAN1_PIN_SOURCE_TX        GPIO_PinSource12
#define CAN1_PIN_TX               GPIO_Pin_12
//CAN2��RX
#define RCC_PERIPH_CAN1_PORT_RX   RCC_AHB1Periph_GPIOA 
#define CAN1_PORT_RX              GPIOA
#define CAN1_PIN_SOURCE_RX        GPIO_PinSource11
#define CAN1_PIN_RX               GPIO_Pin_11
/**
  * @}
  */


//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.								    
extern u8 CAN1_Send_EN;	
										 							 				    
void CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
 
u8 CAN1_Send_Msg(u8* msg,u8 len);						//��������

u8 CAN1_Receive_Msg(u8 *buf);							//��������
#endif

















