/**
  ******************************************************************************
  * @file    sys.c
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    8-April-2019
  * @brief   sys function   ϵͳ����
  ******************************************************************************
  */

//-------------include---------------//
#include "sys.h"  

/**
  * @brief  �������·���ʵ��ִ�л��ָ��WFI(THUMBָ�֧�ֻ������)
  * @param  None
  * @retval None
  */
__asm void WFI_SET(void)
{
	WFI;		  
}

/**
  * @brief  �ر������ж�(���ǲ�����fault��NMI�ж�)
  * @param  None
  * @retval None
  */
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}

/**
  * @brief  ���������ж�
  * @param  None
  * @retval None
  */
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
/**
  * @brief  ����ջ����ַ
  * @param  addr:ջ����ַ
  * @retval None
  */
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
















