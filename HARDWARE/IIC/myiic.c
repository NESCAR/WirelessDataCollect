#include "myiic.h"
#include "delay.h"

// SCL PB6
// SDA PB7

u8 val;
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

  //GPIOB6,B7��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	IIC_SCL=1;
	IIC_SDA=1;
	ADC_DISANBLE=0;
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();	 //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(2);  // ADC 2us 
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(1);  //ADC 1us 
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(1);  // ADC 0
	IIC_SCL=1; 
	delay_us(1);    //++
	IIC_SDA=1;//����I2C���߽����ź�
	//delay_us(4);  							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//		0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();	  //SDA����Ϊ����  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//������ACKӦ��			
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 					 
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{						
	u8 t;   
	SDA_OUT(); 		
	IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{			  
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);   //ADC
		IIC_SCL=1;
		delay_us(1); //1us
		IIC_SCL=0;	
		//delay_us(2); //����Ҫ
	}	 
} 		
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	for(i=0;i<8;i++ )
	{
		IIC_SCL=0; 
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)receive++;   
		delay_us(1); 
	}					 
	if (!ack)
		IIC_NAck();//����nACK
	else
		IIC_Ack(); //����ACK   
	return receive;
}

u8 ADS8266_config()
{
	IIC_Init();
	ADC_DISANBLE=0;
	 //write Model 
	IIC_Start();
	IIC_Send_Byte(ADS7924W_Adress);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	IIC_Send_Byte(MODECNTRL_Adress);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	IIC_Send_Byte(MODECNTRL_Reg);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	IIC_Stop();
	// write SLEEPTime and ACQTime
	IIC_Start();
	IIC_Send_Byte(ADS7924W_Adress);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	IIC_Send_Byte(SLPCONFIG_Adress | 0x80);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	
	IIC_Send_Byte(SLPCONFIG_Reg);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	IIC_Send_Byte(ACQCONFIG_Reg);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	
	IIC_Stop();
	return 1;
}
u8 ADS8266_read()
{
	u8 i;

	
//	if(queue_full(adc_queue))
//	{
//		return 0;
//	}
    
	IIC_Start();
	
	IIC_Send_Byte(ADS7924W_Adress);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	IIC_Send_Byte(DATA0_U_Adress|0x80);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	
	IIC_Start();
	IIC_Send_Byte(ADS7924R_Adress);
	if(IIC_Wait_Ack())
	{
		return 0;
	}
	for(i=0;i<8;i++)
	{
		val = IIC_Read_Byte(1);
		queue_put(&adc_queue, val);
			
	}

	IIC_Stop();
	
	return 1;
}





























