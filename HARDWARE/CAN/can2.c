/**
  ******************************************************************************
  * @file    can2.c
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   CAN2 function   CAN2����
  ******************************************************************************
  */

//-------------include---------------//
#include "can1.h"
#include "can2.h"
#include "gpio.h"
#include "delay.h"
#include "string.h"
#include "userwifi.h" 


CanRxMsg RxMessage;//����ȫ�ֱ����洢CAN�յ������ݣ�����main.c��ʹ��
u8 CAN2_Send_EN = 0;


/**
  * @brief  CAN2��ʼ��
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
  * @param  filter_list���洢��������չID������
  * @param  list_len���˲�ID����
  * @note   BaudRate = Tpclk / (tq * ( (tbs1+1) + (tbs2 +2) + 1))
  * @note   Tpclkһ��Ϊ42MHz
  * @exam   CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal);//can2��ʼ��500k������
  * @retval None
  */
void CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u32 * filter_list,u8 list_len){
	#if PRINT_UART_LOG
	printf("\r\nCAN2 Initing...\r\n");
	#endif
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	#if CAN2_RX1_INT_ENABLE 
		NVIC_InitTypeDef  NVIC_InitStructure;
	#endif
	/* ʹ�����ʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_PERIPH_CAN2_PORT_TX|RCC_PERIPH_CAN2_PORT_RX, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_PERIPH_CAN1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_PERIPH_CAN2, ENABLE);

	/* ��ʼ��GPIO*/
	GPIO_InitStructure.GPIO_Pin    =  CAN2_PIN_TX;
	GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType  =  GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed  =  GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd   =  GPIO_PuPd_UP;
	GPIO_Init(CAN2_PORT_TX, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin    =  CAN2_PIN_RX;
	GPIO_Init(CAN2_PORT_RX, &GPIO_InitStructure);

	/* ����*/
	GPIO_PinAFConfig(CAN2_PORT_TX,CAN2_PIN_SOURCE_TX,GPIO_AF_CAN2);
	GPIO_PinAFConfig(CAN2_PORT_RX,CAN2_PIN_SOURCE_RX,GPIO_AF_CAN2);

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM      = DISABLE;	   //��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM      = DISABLE;	   //����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM      = DISABLE;     //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART      = ENABLE;	   //��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM      = DISABLE;	   //���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP      = DISABLE;	   //���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode      = mode;	       //ģʽ���� 
	CAN_InitStructure.CAN_SJW       = tsjw;	       //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1       = tbs1;        //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2       = tbs2;        //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp;         //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN2, &CAN_InitStructure);            // ��ʼ��CAN2 
	
	#if PRINT_UART_LOG
	printf("CAN2 Filter List:\r\n");
	printf("--------------------------------\r\n");
	#endif	
	/* ���ù�����*/
//	/* Example. ���˳���׼֡ID��0x7e9����չ֡ID��0x1800f001*/
//	CAN_FilterInitStructure.CAN_FilterNumber         = 14;	          //������
//	CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdList;
//	CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit; //32λ�˲�
////	u16 std_id = 0x7e9;
////	CAN_FilterInitStructure.CAN_FilterIdHigh         = (u16)(std_id<<5);
////	CAN_FilterInitStructure.CAN_FilterIdLow          = 0|CAN_ID_STD;
//	u32 ext_id1 = 0x1800f001;
//	CAN_FilterInitStructure.CAN_FilterIdHigh         = (u16)((ext_id1<<3)>>16) & 0xffff;
//	CAN_FilterInitStructure.CAN_FilterIdLow          = (u16)((ext_id1<<3) & 0xffff) | CAN_ID_EXT;
//	u32 ext_id2 = 0x1800f002;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = (u16)(((ext_id2<<3)>>16) & 0xffff);
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = (u16)(((ext_id2<<3) & 0xffff) | CAN_ID_EXT);
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;      //������0������FIFO1
//	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;                //���������
//	CAN_FilterInit(&CAN_FilterInitStructure);                                 //�˲�����ʼ��	
	u32 ext_id1,ext_id2;
	for(u8 filter_num = 0;filter_num < list_len;filter_num+=2){
		CAN_FilterInitStructure.CAN_FilterNumber         = filter_num/2 + 14;	      //��������can2֧�ִ�14��ʼ
		CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdList; //Listģʽ	
		CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit; //32λ�˲�
		ext_id1 = filter_list[filter_num];
		/* ����list������������ż���������ڶ����˲����Ĵ�������ֵ*/
		if(list_len - filter_num == 1){ //����ֻʣ��һ��IDû�й��ˣ��򽫵ڶ����Ĵ���Ҳ���óɸ�ID
			ext_id2 = filter_list[filter_num];
		}else{                          //���д��ڵ���2��IDû�й���
			ext_id2 = filter_list[filter_num+1];
		}
		printf("  0x%X , 0x%X \r\n",ext_id1,ext_id2);
		CAN_FilterInitStructure.CAN_FilterIdHigh         = (u16)((ext_id1<<3)>>16) & 0xffff;
		CAN_FilterInitStructure.CAN_FilterIdLow          = (u16)((ext_id1<<3) & 0xffff) | CAN_ID_EXT;
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = (u16)((ext_id2<<3)>>16) & 0xffff;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow      = (u16)((ext_id2<<3) & 0xffff) | CAN_ID_EXT;		
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;      //������0������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;                //���������
		CAN_FilterInit(&CAN_FilterInitStructure);                                 //�˲�����ʼ��	
	}
	#if PRINT_UART_LOG
	printf("--------------------------------\r\n");
	#endif
	
	#if CAN2_RX1_INT_ENABLE
	CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);		    
	NVIC_InitStructure.NVIC_IRQChannel                   = CAN2_RX1_IRQn;
	/* !! �ж����ȼ�CAN1��CAN2������ͬ*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif
	#if PRINT_UART_LOG
	printf("CAN2 Inited Successfully...!\r\n\r\n");
	#endif	
}   
 
//!ʹ��RX0�ж�
#if CAN2_RX1_INT_ENABLE
/**
  * @brief  CAN2�жϷ�����
  * @param  None
  * @retval None
  */
void CAN2_RX1_IRQHandler(void)
{
  	CanRxMsg RxMessage;

	if (CAN_GetITStatus(CAN2,CAN_IT_FMP1)!= RESET){
		/* ������п��ˣ�ʱ�������*/
		if(queue_empty(can_queue)){
			can_queue.HeadTime = SYSTEMTIME;
			can_queue.YYYY_MM_DD = YYMMDD;
		}
		/* ����CAN����*/
		CAN_Receive(CAN2, CAN_FIFO1 ,&RxMessage);
		/* ����CAN��ID*/
		queue_put(&can_queue,CAN2_ID);
		/* ����CAN�Ľ���ʱ��ƫ��*/
		u32 deltaTime = SYSTEMTIME - can_queue.HeadTime;
		queue_arr_memcpy(&can_queue, (u8 *)&deltaTime , sizeof(deltaTime));
		/* CAN2���ݿ�����queue.arrβ����������tail*/
		queue_arr_memcpy(&can_queue, (u8 *)&RxMessage , sizeof(RxMessage));
	#if PRINT_UART_LOG
		printf("CAN2 Data : ");
		for(int i = 0; i < RxMessage.DLC;i++){
			printf("%d ",RxMessage.Data[i]);
		}
		printf("\r\n");
	#endif
	 }
	 CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
	 CAN_FIFORelease(CAN2,CAN_FIFO1); //���жϱ�־
}

#endif

/**
  * @brief  can2����һ������
  * @param  msg������ָ��,���Ϊ8���ֽ�
  * @param  len�����ݳ���(���Ϊ8)	
  * @retval ���ͳɹ����
           @arg CAN2_SEND_ERROR ʧ��
           @arg CAN2_SEND_OK  �ɹ�
  */
u8 CAN2_Send_Msg(u8* msg,u8 len){	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	//! ��׼��ʶ��Ϊ0,��֡�����ȼ�
	TxMessage.StdId = 0x32;
	//! ������չ��ʾ����29λ��
	TxMessage.ExtId = 0x32;
	//! ʹ����չ��ʶ��
	TxMessage.IDE   = CAN_Id_Extended;
	//! ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.RTR   = 0;
	//! ������Ϣ����
	TxMessage.DLC   = len;
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ
	mbox = CAN_Transmit(CAN2, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF)){
		i++;
		mbox= CAN_Transmit(CAN2, &TxMessage); 
	}
	if(i>=0XFFF){
	  return CAN2_SEND_ERROR;
	}
	return CAN2_SEND_OK;		

}

/**
  * @brief  can1�ڽ������ݲ�ѯ
  * @param  buf���������ݵ�bufferָ��
  * @retval �����ֽڳ���
  */
u8 CAN2_Receive_Msg(u8 *buf){
 	u32 i;
	CanRxMsg RxMessage;
	//! û�н��յ�����,ֱ���˳� 
    if( CAN_MessagePending(CAN2,CAN_FIFO1)==0){
		return 0;		
	}
	//! ��ȡ����	
    CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);
    for(i=0;i<RxMessage.DLC;i++){
		buf[i]=RxMessage.Data[i]; 
	}
	return RxMessage.DLC;	
}
