/**
  ******************************************************************************
  * @file    stmflash.c
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   STM32 flash reading and writting functions   STM32���ڲ�flash��д����
  ******************************************************************************
  */


//-------------include---------------//
#include "stmflash.h"
#include "delay.h"
#include "usart.h" 
 
 
/**
  * @brief  ��ȡָ����ַ����(32λ����) 
  * @param  faddr������ַ
  * @retval ��Ӧ����.
  */
u32 STMFLASH_ReadWord(u32 faddr){
	return *(vu32*)faddr; 
}  

/**
  * @brief  ��ȡָ����ַ�İ���(16λ����) 
  * @param  faddr������ַ
  * @retval ��Ӧ����.
  */
u16 STMFLASH_ReadHalfWord(u32 faddr){
	return *(vu16*)faddr; 
}

/**
  * @brief  ��ȡָ����ַ���ֽ�(8λ����) 
  * @param  faddr������ַ
  * @retval ��Ӧ����.
  */
u8 STMFLASH_ReadByte(u32 faddr){
	return *(vu8*)faddr; 
}

/**
  * @brief  ��ȡĳ����ַ���ڵ�flash����
  * @param  addr��flash��ַ
  * @retval 0~11,��addr���ڵ�����
  */
uint16_t STMFLASH_GetFlashSector(u32 addr){
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}

/**
  * @brief  ��ָ����ַ��ʼд��ָ���֣�32λ�����ȵ�����
  * @note   ��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
            д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
            д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
            û����Ҫ����,��������������Ȳ�����,Ȼ����������д.
  * @param  WriteAddr����ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
  * @param  pBuffer������ָ��
  * @param  NumToWrite����(32λ)��(����Ҫд���32λ���ݵĸ���.) 
  * @retval 0~11,��addr���ڵ�����
  */
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite){ 
	FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
	/* �Ƿ���ַ*/
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4){
		return;	
	}
	/* flash����*/
	FLASH_Unlock();	
	/* FLASH�����ڼ�,�����ֹ���ݻ���*/
	FLASH_DataCacheCmd(DISABLE);

	addrx=WriteAddr;//д�����ʼ��ַ				
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
	if(addrx<0X1FFF0000){			//ֻ�����洢��,����Ҫִ�в�������!!
		while(addrx<endaddr){		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF){ //�з�0XFFFFFFFF�ĵط�,Ҫ�����������������FF����д
				  /* VCC=2.7~3.6V֮��!!*/
				status=FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),VoltageRange_3);
				if(status!=FLASH_COMPLETE){
					break;	
				}
			}else{
				addrx += 4;
			}
		} 
	}
	if(status==FLASH_COMPLETE){
		while(WriteAddr<endaddr){//д����
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE){ //д������
				break;	//д���쳣
			}
		WriteAddr+=4;
		pBuffer++;
		} 
	}
	FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
} 

/**
  * @brief  ��ָ����ַ��ʼ����ָ���֣�32λ�����ȵ�����
  * @param  ReadAddr����ʼ��ַ
  * @param  pBuffer������ָ��
  * @param  NumToRead����(4��ַ)��
  * @retval None
  */
void STMFLASH_ReadWords(u32 ReadAddr,u32 *pBuffer,u32 NumToRead){
	u32 i;
	for(i=0;i<NumToRead;i++){
		/* ��ȡ4���ֽ�.*/
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);
		/* ��ַƫ��4���ֽ�.*/
		ReadAddr+=4;	
	}
}

/**
  * @brief  ��ָ����ַ��ʼ����ָ���ֽڣ�8λ�����ȵ�����
  * @param  ReadAddr����ʼ��ַ
  * @param  pBuffer������ָ��
  * @param  NumToRead���ֽڣ�8λ����
  * @retval None
  */
void STMFLASH_ReadBytes(u32 ReadAddr,u8 *pBuffer,u32 NumToRead){
	u32 i;
	for(i=0;i<NumToRead;i++){
		/* ��ȡ1���ֽ�*/
		pBuffer[i]=STMFLASH_ReadByte(ReadAddr);
		/* ƫ��1���ֽ�*/	
		ReadAddr+=1;
	}
}
