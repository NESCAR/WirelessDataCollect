#include "adc.h"
#include "rsi_global.h"
#include "rsi_app.h"
#include "queue.h"
#include "userwifi.h"
//ʱ������
void ADC_RCC_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��ʱ��
	RCC_APB1PeriphClockCmd(ADC_SPIx_PERIPH, ENABLE);

}

//ADC�Ŀ����������ã�����SPI����λ�ȣ�����
void ADC_CTRL_Conf(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	ADC_RCC_Config();//ʱ������
	//CS
	GPIO_InitStructure.GPIO_Pin = ADC_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(ADC_CS_PORT, &GPIO_InitStructure);
	ADC_CS_L();
	//CONV
	GPIO_InitStructure.GPIO_Pin = ADC_CONV_PIN;
	GPIO_Init(ADC_CONV_PORT, &GPIO_InitStructure);
	ADC_CONV_H();
	//RAGE:5V or 10V
	GPIO_InitStructure.GPIO_Pin = ADC_RAGE_PIN;
	GPIO_Init(ADC_RAGE_PORT, &GPIO_InitStructure);
	ADC_Set5V();
	//RST
	GPIO_InitStructure.GPIO_Pin = ADC_RST_PIN;
	GPIO_Init(ADC_RST_PORT, &GPIO_InitStructure);
	ADC_RST_L();delay_ms(1);ADC_RST_H();delay_ms(1);ADC_RST_L();//��λ
	//OS1/2/3
	GPIO_InitStructure.GPIO_Pin = ADC_OS1_PIN;
	GPIO_Init(ADC_OS1_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ADC_OS2_PIN;
	GPIO_Init(ADC_OS2_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ADC_OS3_PIN;
	GPIO_Init(ADC_OS3_PORT, &GPIO_InitStructure);
	ADC_64Sample_Mean();//64�β���ƽ��
	//BUSY,����ת����״̬
	GPIO_InitStructure.GPIO_Pin = ADC_BUSY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(ADC_BUSY_PORT, &GPIO_InitStructure); 

	//SPI��CLK;MISO;MOSI
	GPIO_InitStructure.GPIO_Pin = ADC_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(ADC_CLK_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = ADC_MOSI_PIN;	
	GPIO_Init(ADC_MOSI_PORT, &GPIO_InitStructure);	//��ʼ��
	GPIO_InitStructure.GPIO_Pin = ADC_MISO_PIN;	
	GPIO_Init(ADC_MISO_PORT, &GPIO_InitStructure);	//��ʼ��

	ADC_CS_H();
	//����
	GPIO_PinAFConfig(ADC_CLK_PORT,ADC_CLK_AF_PINSOURCE,ADC_SPIx_AF);
	GPIO_PinAFConfig(ADC_MISO_PORT,ADC_MISO_AF_PINSOURCE,ADC_SPIx_AF);
	GPIO_PinAFConfig(ADC_MOSI_PORT,ADC_MOSI_AF_PINSOURCE,ADC_SPIx_AF);

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
	SPI_Init(ADC_SPIx, &SPI_InitStructure);  

	SPI_Cmd(ADC_SPIx, ENABLE); 
	ADC_CS_L();
}
//��ȡADC���ݣ����8��ͨ����16bytes����
//���ʹ��AD7606-4��ֻ��4��ͨ����8bytes����
u8 AD7606_Data_Temp[ADC_MAX_BYTES]={0,0};
u8 * ADC_Read(u16 NumByteToRead)
{
 	u16 i;    												    
	ADC_CS_L();   //Ƭѡ   
	//4�ĸ�ͨ��������ȫ������
    for(i=0;i<NumByteToRead;i++)
	{ 
		AD7606_Data_Temp[i] = ADC_SPIx_ReadWriteByte(0XFF);//ѭ������   
    }
	ADC_CS_H();                            //ȡ��Ƭѡ 
	return AD7606_Data_Temp;
}

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 ADC_SPIx_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(ADC_SPIx, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(ADC_SPIx, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(ADC_SPIx, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(ADC_SPIx); //����ͨ��SPIx������յ�����					    
}
