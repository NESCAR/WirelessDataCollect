#ifndef __CAN2_IMU_H
#define __CAN2_IMU_H	 
#include "sys.h"	    


//CAN1����RX0�ж�ʹ��
#define CAN2_RX1_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.

extern u8 CAN2_Send_EN;		

u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
 
u8 CAN2_Send_Msg(u8* msg,u8 len);						//��������

u8 CAN2_Receive_Msg(u8 *buf);							//��������


#endif

















