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
#include "config.h"


//! ������10s(10000ms)����һ��
#define SYNC_INTERVAL_TIME    10000
//! �Ƿ��ù������ڶ�ʱ������˸
#define WORKING_LED_SHINE_IN_TIMER  1
//! �Ƿ���չʾnodeId���ڶ�ʱ������˸
#define NODE_ID_LED_SHINE_IN_TIMER	1
//! �Ƿ���IOÿ��һ��ʱ��ȡ��
#define IO_SHINE_IN_TIMER     0     
//! TIME3 ��ARR��PSC
#define TIM3_ARR              999
#define TIM3_PSC              83
//! TIME4 ��ARR��PSC
#define TIM4_ARR              99
#define TIM4_PSC              83
//! TIM3��4����ʱ��Ƶ��MHz
#define TIM3_4_PCLK_MHZ       84

//!ϵͳʱ�ӣ���ʱ��λ����TIM4�����������
//! @note ��ʱ��λT = Tclk / (arr + 1) / (psc + 1)
extern u32 SYSTEMTIME;

//!ͬ��ʱ��ģ������ϴ�ͬ����ʱ����
extern volatile u32 sync_interval_time;

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);

#endif
