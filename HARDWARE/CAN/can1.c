/**
  ******************************************************************************
  * @file    can1.c
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   CAN1 function   CAN1����
  ******************************************************************************
  */

//-------------include---------------//
#include "can1.h"
#include "gpio.h"
#include "userwifi.h"

//!CAN1����ʹ��
u8 CAN1_Send_EN = 1;
/**
  * @brief  CAN1��ʼ��
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
  * @param  filter_list���洢������ID�����飬8���ֽ�Ϊһ��
  * @param  list_len��������������1~14
  * @note   BaudRate = Tpclk / (tq * ( (tbs1+1) + (tbs2 +2) + 1))
  * @note   Tpclkһ��Ϊ42MHz
  * @exam   CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal);//can1��ʼ��500k������
  * @retval None
  */
void CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 * filter_list,u8 list_len){
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
	GPIO_Init(CAN1_PORT_TX, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CAN1_PIN_RX;
	GPIO_Init(CAN1_PORT_RX, &GPIO_InitStructure);

	/* ���Ÿ���ӳ������*/
	GPIO_PinAFConfig(CAN1_PORT_TX,CAN1_PIN_SOURCE_TX,CAN1_PORT_AF);
	GPIO_PinAFConfig(CAN1_PORT_RX,CAN1_PIN_SOURCE_RX,CAN1_PORT_AF);

	/* CAN��Ԫ����*/
	CAN_InitStructure.CAN_TTCM      = DISABLE;	  //��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM      = DISABLE;	  //����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM      = DISABLE;    //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART      = ENABLE;	  //ʹ�ܱ����Զ��ط� 
	CAN_InitStructure.CAN_RFLM      = DISABLE;	  //���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP      = DISABLE;	  //���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode      = mode;	      //ģʽ���� 
	CAN_InitStructure.CAN_SJW       = tsjw;	      //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1       = tbs1;       //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2       = tbs2;       //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp;        //��Ƶϵ��(Fdiv)Ϊbrp+1
	CAN_Init(CAN1, &CAN_InitStructure);           // ��ʼ��CAN1 

	/* ���ù�����*/
	for(u8 filter_num = 0;filter_num < list_len;filter_num++){
		CAN_FilterInitStructure.CAN_FilterNumber         = filter_num;	          //������
//		CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
		CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdList; //Listģʽ	
		CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit; //32λ�˲�
		CAN_FilterInitStructure.CAN_FilterIdHigh         = *(filter_list + 0 + filter_num * 8) + 256 * (*(filter_list + 1 + filter_num * 8));
		CAN_FilterInitStructure.CAN_FilterIdLow          = *(filter_list + 2 + filter_num * 8) + 256 * (*(filter_list + 3 + filter_num * 8));
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = *(filter_list + 4 + filter_num * 8) + 256 * (*(filter_list + 5 + filter_num * 8));
		CAN_FilterInitStructure.CAN_FilterMaskIdLow      = *(filter_list + 6 + filter_num * 8) + 256 * (*(filter_list + 7 + filter_num * 8));;
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;      //������0������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;                //���������
		CAN_FilterInit(&CAN_FilterInitStructure);                                 //�˲�����ʼ��	
	}


	/* �����ж�*/
	#if CAN1_RX0_INT_ENABLE
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);	    
	NVIC_InitStructure.NVIC_IRQChannel                   = CAN1_RX0_IRQn;
	/* !! �ж����ȼ�CAN1��CAN2������ͬ*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif
}   

//!ʹ��RX0�ж�
#if CAN1_RX0_INT_ENABLE	
/**
  * @brief  CAN1�жϷ�����
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void){
  	CanRxMsg RxMessage;

	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		/* ������п��ˣ�ʱ�������*/
		if(queue_empty(can_queue)){
			can_queue.HeadTime = SYSTEMTIME;
			can_queue.YYYY_MM_DD = YYMMDD;
		}
		/* ����CAN����*/
		CAN_Receive(CAN1, CAN_FIFO0 ,&RxMessage);
		/* ����CAN��ID*/
		queue_put(&can_queue,CAN1_ID);
		/* ������queue.arrβ����������tail*/
		queue_arr_memcpy(&can_queue, (u8 *)&RxMessage , sizeof(RxMessage));		
		
	#if PRINT_UART_LOG
		printf("CAN1 Data : ");
		for(int i = 0; i < RxMessage.DLC;i++){
			printf("%d ",RxMessage.Data[i]);
		}
		printf("\r\n");
	#endif
	 }
	 CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	 CAN_FIFORelease(CAN1,CAN_FIFO0);
}
#endif

/**
  * @brief  can1����һ������
  * @param  msg������ָ��,���Ϊ8���ֽ�
  * @param  len�����ݳ���(���Ϊ8)	
  * @retval ���ͳɹ����
           @arg CAN1_SEND_ERROR ʧ��
           @arg CAN1_SEND_OK  �ɹ�
  */
u8 CAN1_Send_Msg(u8* msg,u8 len){	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	//! ��׼��ʶ��Ϊ0,��֡�����ȼ�
	TxMessage.StdId   = 0x32;	
	//! ������չ��ʾ����29λ������֡�����ȼ�
	TxMessage.ExtId   = 0x32;
	//! ʹ����չ��ʶ��	
	TxMessage.IDE     = 0;		
	//! ��Ϣ����Ϊ����֡��һ֡8λ	
	TxMessage.RTR     = 0;
	//! ������Ϣ����	
	TxMessage.DLC     = len;	
	//! ������Ϣ
	for(i=0;i<len;i++){
		TxMessage.Data[i] = msg[i];  
	}		
	//! ������Ϣ
	mbox = CAN_Transmit(CAN1, &TxMessage);   
	i = 0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF)){
		i++;
		mbox= CAN_Transmit(CAN1, &TxMessage); 
	}
	if(i>=0XFFF){
		return CAN1_SEND_ERROR;
	}
	return CAN1_SEND_OK;		

}

/**
  * @brief  can1�ڽ������ݲ�ѯ
  * @param  buf���������ݵ�bufferָ��
  * @retval �����ֽڳ���
  */
u8 CAN1_Receive_Msg(u8 *buf){
 	u32 i;
	CanRxMsg RxMessage;
	//! û�н��յ�����,ֱ���˳� 
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0){
		return 0;		
	}
	//! ��ȡ����	
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    for(i=0;i<RxMessage.DLC;i++){
		buf[i]=RxMessage.Data[i]; 
	}
	return RxMessage.DLC;	
}













