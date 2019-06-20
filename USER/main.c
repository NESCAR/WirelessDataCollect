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

#define CAN_INIT_REBOOT   0 /*< �Ƿ񿪻�����can�����в���>*/

void testAdc(void);
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
	/*msʱ�䣬����ǰ�棬�벻Ҫ�����ƶ�λ��*/
	TIM3_Int_Init(TIM3_ARR,TIM3_PSC); //1000us	
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
	queue_init(&can_queue);
	
	/*ADC������ų�ʼ��*/
	ADC_CTRL_Conf();
	
	/* CAN����*/
	#if CAN_INIT_REBOOT
	u32 filter[7] = {0x1800f001,0x1800f002,0x1800f003,0x1800f004,0x1800f005,0x1800f006,0x1800f007};
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,12,CAN_Mode_Normal,(u32 *)filter,7);
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_7tq,6,CAN_Mode_Normal,(u32 *)filter,7);
	#endif
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
/**
  * @brief  ����WIFI��촫���ٶ�
  * @param  None
  * @retval None
  */
u8 data[1500];
void testMaxWifiSpeed(){
	
	rsi_send_ludp_data(localSocketDescriptor_txrx, data,1500, RSI_PROTOCOL_UDP_V4, (uint8 *)localDestIp_txrx, localDestSocket_txrx, &bytes_sent);
}
int main(void)
{     
	setBoardSta(BOARD_INITING);Initialization();setBoardSta(BOARD_INITED);//��ʼ��ϵͳ
	setBoardSta(BOARD_RUNNING);//������
	u8 RspCode;u16 tcpRecvBuffLen;u8 * pRecvData;
	memset(data,32,1500);
	while(1)
	{
//		testMaxWifiSpeed();
		if(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL){
			if(IAM_MASTER_CLOCK == 'Y'){
				if(sync_interval_time >= SYNC_INTERVAL_TIME&&Wifi_Send_EN){
					INFO_LED_CONV();//��ת
					sync_interval_time = 0;
					Send_Sync_Time();//ʱ��ͬ��һ��
				}
			}else{  //������ʱ����Ҫ����ADC��CAN����
				wifi_send_package();
			}
		}else if(RSI_WIFI_OPER_MODE == RSI_WIFI_AP_MODE_VAL){
			delay_ms(10);
			RspCode =Check_PKT();
			switch (RspCode){
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









