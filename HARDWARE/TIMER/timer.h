#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 								  
////////////////////////////////////////////////////////////////////////////////// 	
extern u32 SYSTEMTIME;

#define SYNC_INTERVAL_TIME    2000  //基本是2s更新一下

void TIM3_Int_Init(u16 arr,u16 psc);

#endif
