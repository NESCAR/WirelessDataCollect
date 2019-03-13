#include "wificonf.h"
#include "rsi_global.h"
#include "rsi_app.h"
#include "userwifi.h"
#include "typetrans.h"
#include "config.h"
//ʱ������



void RCC_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE );
}
 

//����WIFI���ж�NVIC
void WIFI_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=WIFI_INTR_EXTI_CH;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//���ó�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void WIFI_SPI_Conf(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	RCC_Config();	
	WIFI_EXTI_Conf();	
	WIFI_NVIC_Config();	
	//CS:PC5
	GPIO_InitStructure.GPIO_Pin = WIFI_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(WIFI_CS_PORT, &GPIO_InitStructure);
	
	//RST
	GPIO_InitStructure.GPIO_Pin = WIFI_RST_PIN;
	GPIO_Init(WIFI_RST_PORT, &GPIO_InitStructure);
		
	//INTR�ж�
	GPIO_InitStructure.GPIO_Pin = WIFI_INTR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(WIFI_INTR_PORT, &GPIO_InitStructure); 
	SYSCFG_EXTILineConfig(WIFI_INTR_EXTI_PORTSOURCE,WIFI_INTR_EXTI_PINSOURCE);  
	
	//CLK;MISO;MOSI
	GPIO_InitStructure.GPIO_Pin = WIFI_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(WIFI_CLK_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = WIFI_MOSI_PIN;	
	GPIO_Init(WIFI_MOSI_PORT, &GPIO_InitStructure);	//��ʼ��
	GPIO_InitStructure.GPIO_Pin = WIFI_MISO_PIN;	
	GPIO_Init(WIFI_MISO_PORT, &GPIO_InitStructure);	//��ʼ��

	WIFI_CS_H();
	//����
	GPIO_PinAFConfig(WIFI_CLK_PORT,WIFI_CLK_AF_PINSOURCE,WIFI_SPIx_AF);
	GPIO_PinAFConfig(WIFI_MISO_PORT,WIFI_MISO_AF_PINSOURCE,WIFI_SPIx_AF);
	GPIO_PinAFConfig(WIFI_MOSI_PORT,WIFI_MOSI_AF_PINSOURCE,WIFI_SPIx_AF);

	//wifi spi������
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;	
	SPI_Init(WIFI_SPIx, &SPI_InitStructure);  

	SPI_Cmd(WIFI_SPIx, ENABLE); 
	WIFI_CS_L(); 
}   
//����WIFI�ж�
void WIFI_EXTI_Conf(void)
{
	EXTI_InitTypeDef   EXTI_Type;
	EXTI_Type.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_Type.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Type.EXTI_Line=WIFI_INTR_EXTI_LINE;
	EXTI_Type.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_Type);
}
//WiFi SPI���շ�
u8 WIFI_SPIx_TxRx(u8 TxData)
{		 			 
	WIFI_SPIx->DR=TxData;
	while ((WIFI_SPIx->SR&SPI_I2S_FLAG_RXNE) == 0); 
	return WIFI_SPIx->DR;//SPI_I2S_ReceiveData(SPI1);		    
}

//�յ���������
extern u8 DATA_AUTO_CHECK_EN;
void EXTI4_IRQHandler(void)
{
	EXTI->PR		|=1<<4;
	rsi_app_cb.pkt_pending ++;//= RSI_TRUE;	
	if( BOARD_STA == BOARD_RUNNING){//�����У���ʼ����ʱ���ܽ�����һ��
		if((DATA_AUTO_CHECK_EN)&&(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL))//����Clienģʽ������Ҫʹ���Զ�check
		{
			receive_udp_package();
		}		
	}

}
/**********************************
//�������ã�����ΪAPģʽǰ��Ҫ���ò���
//
**********************************/
void setApModePara(void){
	RSI_IP_CFG_MODE    = RSI_STATIC_IP_MODE;//����Ϊ��̬��ַ
	RSI_WIFI_OPER_MODE = RSI_WIFI_AP_MODE_VAL;//����ΪAPģʽ
	RSI_BAND           = RSI_BAND_2P4GHZ;//APģʽֻ֧��2.4G
}


/**********************************
//�������ã�����ΪClientģʽǰ��Ҫ���ò���
//
**********************************/
void setClientModePara(void){
	RSI_IP_CFG_MODE     = RSI_DHCP_IP_MODE;//����ΪDHCP��ַ
	RSI_WIFI_OPER_MODE  = RSI_WIFI_CLIENT_MODE_VAL;//����Ϊ�ͻ���ģʽ
	RSI_BAND            = RSI_DUAL_BAND ;//�ͻ���ģʽ֧��˫Ƶ
}

//���ģ������״̬,���û�гɹ����Ӷ�Σ�������ΪAPģʽ
void checkModuleSta(void){
	u8 RspCode;u8 status;
	u8 resetNum=6;//��������Ĵ���
	/*STAģʽ��ʹ�õ�ָ��.����Ч*/	
	if(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL){
		delay_ms(1000);delay_ms(1000);//�ȴ�һ��ʱ��
		//��ȡ��ǰ������״̬
		rsi_query_conn_status();
		RspCode=Read_PKT();		
		//���û��������������
		while(resetNum-- > 0 ){
			if(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.conStatusFrameRcv.state[0] != 0){//�������
				#if PRINT_UART_LOG
				printf("Module Connected Ap Successfully!\r\n");//1:����
				#endif
			
				/*��ѯnet���� [�ɹ�����֮��]*/
				RspCode=rsi_query_net_parms();         		//this is command
				RspCode=Read_PKT();
				if(RSI_RSP_NETWORK_PARAMS == RspCode){
					memcpy(localhost,rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.qryNetParmsFrameRcv.ipaddr,IPV4_LENGTH);
					#if PRINT_UART_LOG
					printf("Module Ip : %d:%d:%d:%d\r\n",localhost[0],localhost[1],localhost[2],localhost[3]);//0:δ����	
					#endif
				}
				
				/*��ѯRSSI [�ɹ�����֮��]*/
				RspCode=rsi_query_rssi();          			//this is command
				RspCode=Read_PKT();
				#if PRINT_UART_LOG
				printf("RSSI:0x%0.2X\r\n",rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.rssiFrameRcv.rssiVal[0]);	
				#endif	
				return;	//�����������ֱ�ӷ���			
			}else{//���û������
				#if PRINT_UART_LOG
				printf("Module Connected Ap Unsuccessfully!\r\n");//1:����
				printf("Soft Resetting Module ...\r\n");//����
				#endif
				rsi_module_soft_reset();//�������
				delay_ms(1000);
				#if PRINT_UART_LOG
				printf("Soft Reset Module Successfully!\r\n");//����
				printf("\r\n");//����ΪClient
				#endif
				setClientModePara();//����Ϊclientģʽ����
				WIFI_BOOT();//boot
				WIFI_Conf();//conf	
				delay_ms(500);
				rsi_query_conn_status();//�鿴��ǰ������״̬
				RspCode=Read_PKT();					
			}
		
		};
		//˵���������ʧ���ˣ�����ΪAPģʽ
		if(rsi_app_cb.uCmdRspFrame->uCmdRspPayLoad.conStatusFrameRcv.state[0] == 0){//û������
			#if PRINT_UART_LOG
			printf("Module Connected Ap Unsuccessfully!\r\n");//0:δ����
			#endif
			
			//resetģ��
			do{
				#if PRINT_UART_LOG
				printf("Soft Resetting Module ...\r\n");
				#endif
				rsi_module_soft_reset();
				delay_ms(1500);
//				#if PRINT_UART_LOG
//				printf("Module Return %d\r\n",RspCode);
//				#endif
				RspCode = Read_PKT();
			}while(RspCode != RSI_RSP_CONNECTION_STATUS);//������Ϊʲô��ֻ�ܷ���0x48
			#if PRINT_UART_LOG
			printf("Soft Reset Module Successfully!\r\n");
			#endif
			
			
			#if PRINT_UART_LOG
			printf("Setting Module As Ap...\r\n");//0:δ����
			#endif
			//����ΪAPģʽ
			setApModePara();
			
			WIFI_BOOT();
			WIFI_Conf();
			#if PRINT_UART_LOG
			printf("Setted Module As Ap Successfully!\r\n");//0:δ����
			#endif
		}

	}
}


void InitWiFi(void){
	WIFI_SPI_Conf();
	u8 status;
	status = WIFI_BOOT();
	if(status!=0){//˵��������
		#if PRINT_UART_LOG
		printf("WiFi Boot Unsuccessfully!\r\n");
		return;//boot�����⣬ֱ�ӷ���
		#endif
	}else if(status == 0){
		#if PRINT_UART_LOG
		printf("WiFi Boot Successfully!\r\n");
		#endif	
	}
	status = WIFI_Conf();
	if(status!=0){//˵��������
		#if PRINT_UART_LOG
		printf("WiFi Config Unsuccessfully!\r\n");
		#endif
	}else if(status == 0){
		#if PRINT_UART_LOG
		printf("WiFi Config Successfully!\r\n");
		#endif	
	}
	delay_ms(1000);
}

void openAllSocket(void){
	u8 status;
	DATA_AUTO_CHECK_EN= 0;
	#if SEND_WITH_UDP
		status = OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//������������
		if(status != 0){//������
			printf("WiFi Data-UDP Connect Unsuccessfully!\r\n");
		}else if(status == 0){
			printf("WiFi Data-UDP Connect Successfully!\r\n");
		}
	#else
		OpenTcpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);//����һ�������շ�socket
		//  rsi_send_data(socketDescriptor_txrx, "qqqqqqqqqqqqqqqq", 16,RSI_PROTOCOL_TCP_V4,&bytes_sent);
	#endif
		OpenLudpSocket(localDestIp_txrx,localDestSocket_txrx,localModuleSocket_txrx,&localSocketDescriptor_txrx);//�����������ݴ���
	#if IAM_MASTER_CLOCK
		OpenLudpSocket(destIp_sync,destSocket_sync,moduleSocket_sync,&socketDescriptor_sync);//ʱ��ͬ��socket	
	#endif
	DATA_AUTO_CHECK_EN= 1;
}