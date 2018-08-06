#include "iic.h"


void delay_5ms(void)
{
	uint32_t i=0x8;
	while(i--);
}

void SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
}
void SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
}

//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOFʱ��

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	GPIO_SetBits(GPIOB, GPIO_Pin_14);//GPIOF9,F10���øߣ�����
	GPIO_SetBits(GPIOB, GPIO_Pin_15);//GPIOF9,F10���øߣ�����
	
//	IIC_SCL_H;//�ߵ�ƽ
//	IIC_SDA_H;

}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_H;	  	  
	IIC_SCL_H;
	delay_5ms();//(4);
 	IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	delay_5ms();//(4);
	IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
	delay_5ms();//(4);
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_L;
	IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	delay_5ms();//(4);
	IIC_SCL_H; 
	delay_5ms();//(4);
	IIC_SDA_H;//����I2C���߽����ź�
	delay_5ms();//(4);									   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	
	delay_5ms();//(4);	   
	IIC_SCL_H;
	delay_5ms();//(4);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
		delay_5ms();//(1);
	}
	IIC_SCL_L;//ʱ�����0 
  delay_5ms();//(50);	
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	delay_5ms();//(5);
	IIC_SCL_H;
	delay_5ms();//(5);
	IIC_SCL_L;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	delay_5ms();//(5);
	IIC_SCL_H;
	delay_5ms();//(5);
	IIC_SCL_L;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
		{
			IIC_SDA_H;
		}
		else
		{
			IIC_SDA_L;
		}
        txd<<=1; 	  
		delay_5ms();//(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_H;
		delay_5ms();//(2); 
		IIC_SCL_L;	
		delay_5ms();//(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	 {
        IIC_SCL_L; 
        delay_5ms();//(50);
				IIC_SCL_H;
		    delay_5ms();//(50);
        receive<<=1;
        if(READ_SDA)
				receive++;	
				
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data)
{
	uint8_t i;		  	    																 
	IIC_Start();  
	IIC_Send_Byte(daddr);	   //����д����//����������ַ0xB8,д���� 
	IIC_Wait_Ack();

	IIC_Send_Byte(addr);//���͵�ַ
	IIC_Wait_Ack();
	
	IIC_Send_Byte(data); //�������ģʽ			   
	IIC_Wait_Ack();	 
	
	IIC_Stop();//����һ��ֹͣ����
}
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr)  
{
	uint8_t i;		  	    																 
	IIC_Start();  
	IIC_Send_Byte(daddr);	   //����д����//����������ַ0xB8,д���� 
	IIC_Wait_Ack();

	IIC_Send_Byte(addr);//���͵�ַ
	IIC_Wait_Ack();

	IIC_Start();  	 	   
	IIC_Send_Byte(daddr+1); //�������ģʽ			   
	IIC_Wait_Ack();	 

//	for(i=0;i<4;i++)
//	{
//	dat[i]=IIC_Read_Byte(1);//��ǰ�ĸ�������ACK
//	}
	i=IIC_Read_Byte(0);//����5������NACK

	IIC_Stop();//����һ��ֹͣ����

	return i;

}























