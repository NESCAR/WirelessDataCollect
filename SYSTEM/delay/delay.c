/**
  ******************************************************************************
  * @file    delay.c
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    8-April-2019
  * @brief   delay function   ��ʱ����
  ******************************************************************************
  */

//-------------include---------------//
#include "delay.h"
#include "sys.h"

static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��os��,����ÿ�����ĵ�ms��
	
/**
  * @brief  ��ʼ���ӳٺ���
  * @param  SYSCLK��ϵͳʱ��Ƶ��
  * @retval None
  */
void delay_init(u8 SYSCLK)
{
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us=SYSCLK/8;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
	fac_ms=(u16)fac_us*1000;				//��OS��,����ÿ��ms��Ҫ��systickʱ����   
}								    

/**
  * @brief  ��ʱnus
  * @param  nus��Ҫ��ʱ��us��.
  * @retval None
  */
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL =0X00;       				//��ռ����� 
}

/**
  * @brief  ��ʱnms
  * @param  nms��Ҫ��ʱ��ms��
  * @note   SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
						nms<=0xffffff*8*1000/SYSCLK
						SYSCLK��λΪHz,nms��λΪms
						��168M������,nms<=798ms 
  * @retval None
  */
void delay_xms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���� 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;     		  		//��ռ�����	  	    
} 
//��ʱnms 
//nms:0~65535
/**
  * @brief  ��ʱnms
  * @param  nms��Ҫ��ʱ��ms��
  * @note   nmsȡֵ��Χ0~65535
  * @retval None
  */
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;
	u16 remain=nms%540;
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 
		

