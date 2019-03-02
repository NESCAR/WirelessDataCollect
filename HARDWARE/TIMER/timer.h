#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 								  
////////////////////////////////////////////////////////////////////////////////// 	
extern u32 SYSTEMTIME;

#define SYNC_INTERVAL_TIME    2000  //������2s����һ��
#define LED_SHINE_IN_TIMER   1//�Ƿ��õ��ڶ�ʱ������˸
#define IO_SHINE_IN_TIMER    0//�Ƿ���IOÿ��һ��ʱ��ȡ��
#if IAM_MASTER_CLOCK
extern u16 sync_interval_time;
#endif
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);

#endif
