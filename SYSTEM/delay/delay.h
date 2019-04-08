/**
  ******************************************************************************
  * @file    delay.h
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    8-April-2019
  * @brief   delay function   ��ʱ����
  ******************************************************************************
  */

#ifndef __DELAY_H
#define __DELAY_H 	

//-------------include---------------//
#include <sys.h>	  

void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























