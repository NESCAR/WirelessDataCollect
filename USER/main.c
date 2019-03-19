/**
  ******************************************************************************
  * @file    main.c
  * @author  zju.nesc.iotlab    �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   main function   ������
  ******************************************************************************
  */

//-------------include---------------//
#include "sys.h"
#include "userwifi.h"
#include "delay.h"  
#include "usart.h"   
#include "gpio.h"
#include "wificonf.h"
#include "queue.h"
#include "can1.h"
#include "can2.h"
#include "timer.h"
#include "adc.h"
#include "stmflash.h" 
#include "config.h"
#include "rsi_global.h"
#include "rsi_app.h"
#include "crc.h"

u8 IO_input[3];
extern u32 bytes_sent;

void testAdc(void);
u8 can_send_package(void);
void Initialization (void);

/**
  * @brief  ϵͳ��ʼ��
  * @param  None
  * @retval None
  */
void Initialization (void)
{
	/*�������ȼ�����*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/*��ʼ��GPIO*/
	GP_IO_Init();
	/*��ʼ��UART*/
	uart_init(115200);
	/*��ʼ����ʱ����*/
	delay_init(168); 
	
	/* �������ã�һ�����в���Ҫ*/
	/*setFactory();*/
	
	#if PRINT_UART_LOG
	printf("System Initing...!\r\n");
	#endif
	
	/*���ز���*/
	loadParafromMainOrBackupFlash();//getPara();
	/*����wifiΪ�ͻ���ģʽ*/
	setClientModePara();InitWiFi();
	/*���ģ������������������ʧ�ܣ�������ΪAPģʽ*/
	checkModuleSta(); 
	/*����������Ҫ���׽���*/
	if(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL){
		openAllSocket();
	}else if(RSI_WIFI_OPER_MODE == RSI_WIFI_AP_MODE_VAL){
		OpenSocket((u8*)AP_MODE_SOCKET_DEST_IP,AP_MODE_SOCKET_DEST_PORT,AP_MODE_SOCKET_MODULE_PORT,RSI_SOCKET_TCP_SERVER);
		Read_PKT();
		printf("Open TCP Socket!\r\n");
	}
	
	/*��������*/
	queue_init(&adc_queue);
	
	/*ADC������ų�ʼ��*/
	ADC_CTRL_Conf();
	
	/*msʱ��*/
	TIM3_Int_Init(TIM3_ARR,TIM3_PSC); //1000us
	/*ϵͳʱ��100us�ж�*/
	TIM4_Int_Init(TIM4_ARR,TIM4_PSC);
	#if PRINT_UART_LOG
	printf("System Inited Successfully!\r\n");
	/*�����ĵ�*/
	getHelp();
	/*������ӡ*/
	getPara();
	#endif
}

/**
  * @brief  CAN���ݷ���
  * @param  None
  * @retval None
  */
u8 can_send_package()
{ 
	if(CAN_Send_EN && CAN1_Send_EN){
		if(queue_empty(adc_queue)) delay_ms(2);
		IO_input[0] = DIGITAL_INPUT1;
		IO_input[1] = DIGITAL_INPUT2;
		IO_input[2] = nodeId;
		CAN1_Send_Msg((u8 *) &adc_queue.YYYY_MM_DD, 8);
		CAN1_Send_Msg((u8 *) &adc_queue.arr[adc_queue.head],8);
		CAN1_Send_Msg(IO_input,3);
		CAN_Send_EN = 0;
		CAN1_Send_EN = 0;
	}
	if(CAN_Send_EN && CAN2_Send_EN){

		if(queue_empty(adc_queue)) delay_ms(2);
		IO_input[0] = DIGITAL_INPUT1;
		IO_input[1] = DIGITAL_INPUT2;
		IO_input[2] = nodeId;
		CAN2_Send_Msg((u8 *) &adc_queue.YYYY_MM_DD, 8);
		CAN2_Send_Msg((u8 *) &adc_queue.arr[adc_queue.head],8);
		CAN2_Send_Msg(IO_input,3);
		CAN_Send_EN = 0;
		CAN2_Send_EN = 0;
	}
	return 1;
}



/**
  * @brief  ����ADC����ӡ������
  * @param  None
  * @retval None
  */
void testAdc(void){
	u8 * AdcTemp;
	ADC_CONV_H();
	delay_us(5);
	ADC_CONV_L();
	delay_us(5);
	ADC_CONV_H();
	delay_us(100);
	AdcTemp = ADC_Read(ADC_MAX_BYTES);
	#if PRINT_UART_LOG
//	printf("%d ",(AdcTemp[0]*256+AdcTemp[1]));
//	printf("%d ",(AdcTemp[2]*256+AdcTemp[3]));
//	printf("%d ",(AdcTemp[4]*256+AdcTemp[5]));
//	printf("%d \r\n",(AdcTemp[6]*256+AdcTemp[7]));
	printf("%.3f   ",(double)(AdcTemp[0]*256+AdcTemp[1])*5.0/32768.0);	
	printf("%.3f   ",(double)(AdcTemp[2]*256+AdcTemp[3])*5.0/32768.0);	
	printf("%.3f   ",(double)(AdcTemp[4]*256+AdcTemp[5])*5.0/32768.0);	
	printf("%.3f   ",(double)(AdcTemp[6]*256+AdcTemp[7])*5.0/32768.0);	
	printf("\r\n");
	delay_ms(1000);
	#endif
}

int main(void)
{     
	setBoardSta(BOARD_INITING);Initialization();setBoardSta(BOARD_INITED);//��ʼ��ϵͳ
	setBoardSta(BOARD_RUNNING);//������
	u8 RspCode;u16 tcpRecvBuffLen;u8 * pRecvData;
	while(1)
	{
		if(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL){
//			CAN1_Send_Msg((u8 *)&SYSTEMTIME,4);
//			delay_ms(1000);
//			CAN2_Send_Msg((u8 *)&SYSTEMTIME,4);
//			CAN1_Receive_Msg(CanBuf);
//			delay_ms(1000);
		wifi_send_package();//�������ݣ�ÿ��ʱ�Ӹ��º�������ݵ���һ������UDP_SEND_SIZE  8bytesʱ��+2bytes����IO+8*N bytes ADC�ź�
//			testAdc();
//			receive_udp_package();\\stm32_wifi_ap_1_x\../HARDWARE/WIFI/userwifi.c\adc_queue.tail
//			wifi_send_package_test();
			#if IAM_MASTER_CLOCK
				if(sync_interval_time >= SYNC_INTERVAL_TIME&&Wifi_Send_EN)
				{
					LED2_CONV();//DS2��ת
					LED4_CONV();
					sync_interval_time = 0;
					Send_Sync_Time();//ʱ��ͬ��һ��
				}
			#endif
		}else if(RSI_WIFI_OPER_MODE == RSI_WIFI_AP_MODE_VAL){
			delay_ms(10);
			RspCode =Check_PKT();
			switch (RspCode)
			{
				case RSI_RSP_DATA_RECEIVE://RSI_RSP_DATA_RECEIVE �ӵ������ٷ���ȥ
					pRecvData =rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.recvFrameTcp.recvDataBuf;
					tcpRecvBuffLen = rsi_bytes4R_to_uint32(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.recvFrameTcp.recvBufLen);
					#if PRINT_UART_LOG
					printf("Recv %d Byte CMD \"%s\" From TCP Socket!\r\n",tcpRecvBuffLen,pRecvData);
					#endif
					rsi_send_data( rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.recvFrameTcp.recvSocket[0], //id
						pRecvData, 				//��������ָ��
						tcpRecvBuffLen,
						RSI_PROTOCOL_TCP_V4,&bytes_sent);		//����
					if(CMD_RX_BUF.CmdCompleteFlag == CMD_NOT_COMPLETED){//�����ûû����������Ҫ��������
						//Ϊ�˷ŵ�USART BUFF�н�����ͬ�Ĵ�����Ҫ�ж��Ƿ񳤶ȹ���
						if(tcpRecvBuffLen<(USART_BUFF_LEN)){
							//��������������Ϊһ��tcp������������Ҫ����usart����һ��һ����������
							uart_queue_clear(&CMD_RX_BUF);
							//���뵽BUFF�У����ڶ�ʱ�����Զ�����
							uart_queue_put_many(&CMD_RX_BUF,pRecvData,tcpRecvBuffLen);
							uart_queue_put_many(&CMD_RX_BUF,(u8*)"\r\n",2);//�������\r\n,����û���յ�\r\nҲû�й�ϵ
							CMD_RX_BUF.CmdCompleteFlag = CMD_COMPLETED;
						}
					}else if(CMD_RX_BUF.CmdCompleteFlag == CMD_COMPLETED){
						rsi_send_data( rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.recvFrameTcp.recvSocket[0], //id
							(u8*)"Cannot Deal With More Than 2 CMD!\r\nPlese Wait!\r\n", 				//��������ָ��
							tcpRecvBuffLen,
							RSI_PROTOCOL_TCP_V4,&bytes_sent);		//����
						#if PRINT_UART_LOG
						printf("WARNING : Cannot Deal With More Than 2 CMDs!\r\n");
						#endif						
					}
					break;
				default:
					break;
			}
		}			
	}
}









