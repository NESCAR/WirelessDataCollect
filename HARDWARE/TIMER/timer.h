/**
  ******************************************************************************
  * @file    timer.ch
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   timer configuration and running functions   ��ʱ�����ú����з���
  ******************************************************************************
  */

#ifndef _TIMER_H
#define _TIMER_H

//-------------include---------------//
#include "sys.h"


//! ������10s(10000ms)����һ��
#define SYNC_INTERVAL_TIME    10000
//! �Ƿ��õ��ڶ�ʱ������˸
#define LED_SHINE_IN_TIMER    1
//! �Ƿ���IOÿ��һ��ʱ��ȡ��
#define IO_SHINE_IN_TIMER     0     

//!ϵͳʱ�ӣ���ʱ��λ����TIM4�����������
//! @note ��ʱ��λT = Tclk / (arr + 1) / (psc + 1)
extern u32 SYSTEMTIME;

//!ͬ��ʱ��ģ������ϴ�ͬ����ʱ����
#if IAM_MASTER_CLOCK
extern u16 sync_interval_time;
#endif


void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);

#endif
