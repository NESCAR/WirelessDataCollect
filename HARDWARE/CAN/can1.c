/**
  ******************************************************************************
  * @file    can1.c
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   can1 function   CAN1����
  ******************************************************************************
  */

//-------------include---------------//
#include "can1.h"
#include "gpio.h"
#include "userwifi.h"

//!CAN����ʹ��
u8 CAN1_Send_EN = 0;
/**
  * @brief  CAN��ʼ��
  * @param  tsjw������ͬ����Ծʱ�䵥Ԫ
			@arg CAN_SJW_1tq ~ CAN_SJW_4tq
  * @param  tbs2��ʱ���2��ʱ�䵥Ԫ
			@arg CAN_BS2_1tq ~ CAN_BS2_8tq;
  * @param  tbs1��ʱ���1��ʱ�䵥Ԫ
			@arg CAN_BS1_1tq ~ CAN_BS1_16tq
  * @param  brp�������ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
  * @param  mode��CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
			@arg CAN_Mode_Normal����ͨģʽ
			@arg CAN_Mode_LoopBack���ػ�ģʽ
  * @exam   CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal);//can1��ʼ��500k������
  * @retval None
  */
void CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	#if CAN1_RX0_INT_ENABLE 
		NVIC_InitTypeDef  NVIC_InitStructure;
	#endif
	/* ʹ�����ʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_PERIPH_CAN1_PORT_TX|RCC_PERIPH_CAN1_PORT_RX, ENABLE);                  											 
	RCC_APB1PeriphClockCmd(RCC_PERIPH_CAN1, ENABLE);	

	/* ��ʼ��GPIO*/
	GPIO_InitStructure.GPIO_Pin = CAN1_PIN_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(CAN1_PORT_TX, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CAN1_PIN_RX;
	GPIO_Init(CAN1_PORT_RX, &GPIO_InitStructure);

	/* ���Ÿ���ӳ������*/
	GPIO_PinAFConfig(CAN1_PORT_TX,CAN1_PIN_SOURCE_TX,CAN1_PORT_AF);
	GPIO_PinAFConfig(CAN1_PORT_RX,CAN1_PIN_SOURCE_RX,CAN1_PORT_AF);

	/* CAN��Ԫ����*/
	CAN_InitStructure.CAN_TTCM      = DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM      = DISABLE;	//����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM      = DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART      = DISABLE;	//ʹ�ܱ����Զ��ط� 
	CAN_InitStructure.CAN_RFLM      = DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP      = DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode      = mode;	 //ģʽ���� 
	CAN_InitStructure.CAN_SJW       = tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1       = tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2       = tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 

	/* ���ù�����*/
	CAN_FilterInitStructure.CAN_FilterNumber         = 0;	  //������0
	CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit; //32λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;////32λID
	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE; //���������0
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��

	/* �����ж�*/
	#if CAN1_RX0_INT_ENABLE
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
	NVIC_InitStructure.NVIC_IRQChannel                   = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif
}   

//!ʹ��RX0�ж�
#if CAN1_RX0_INT_ENABLE	
/**
  * @brief  CAN�жϷ�����
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
    CAN_Receive(CAN1, 0, &RxMessage);
    order_anay(RxMessage.Data);
	CAN1_Send_EN = 1;
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=0x32;	 // ��׼��ʶ��Ϊ0
  TxMessage.ExtId=0x32;	 // ������չ��ʾ����29λ��
  TxMessage.IDE=0;		  // ʹ����չ��ʶ��
  TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=len;							 // ������֡��Ϣ
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))
	{
		i++;
		mbox= CAN_Transmit(CAN1, &TxMessage); 
	}
  if(i>=0XFFF)return 1;
  return 0;		

}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}













