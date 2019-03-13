/**
  ******************************************************************************
  * @file    config.c
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   parameters configuraton functions   �������÷���
  ******************************************************************************
  */

//-------------include---------------//
#include "config.h"
#include "stmflash.h"
#include "usart.h"
#include "string.h"
#include "crc.h"
#include "typetrans.h"
#include "userwifi.h"
#include "core_cm4.h"
#include "rsi_api.h"
#include <stdlib.h>
#include <stdio.h>

u8   RSI_JOIN_SSID[RSI_JOIN_SSID_MAX_LENGTH] = "418_Lab_5G";             /*!< WIFI����>*/
u8   RSI_PSK[RSI_PSK_MAX_LENGTH]             = "518518518";              /*!< WIFI����>*/
u8   nodeId                                  = 1;                        /*!< �ڵ�ID��0-255>*/
u32  RSI_WIFI_OPER_MODE                      = RSI_WIFI_CLIENT_MODE_VAL; /*!< CLIENT����APģʽ>*/
u8   RSI_IP_CFG_MODE                         = RSI_DHCP_IP_MODE;         /*!< @ RSI_DHCP_IP_MODE or RSI_STATIC_IP_MODE >*/
u8   RSI_BAND                                = RSI_DUAL_BAND;            /*!< @ RSI_BAND_2P4GHZ or RSI_BAND_5GHZ or RSI_DUAL_BAND>*/
u8   BOARD_STA                               = BOARD_INITING;            /*!< ���ӵ�״̬>*/
u8   catPara[PARA_CAT_CH_MAX_LENGTH]         = {0};                      /*!< �洢���Ӻ������>*/
u8   localhost[IPV4_LENGTH]                  = {0};                      /*!< ����ip>*/


/**
  * @brief  ����ϵͳ��������Ĳ���
  * @param  sta������״̬
  *          @arg BOARD_INITING:���ڳ�ʼ��
  *          @arg BOARD_INITED:��ʼ�����
  *          @arg BOARD_RUNNING:��������������
  * @retval �洢������������ֽڳ��ȣ������ܳ�����ֵ�����ݡ��ָ�����crc����������
  */
void setBoardSta(u8 sta){
	BOARD_STA  = sta;
}

/**
  * @brief  ����ϵͳ��������Ĳ���
  * @param  startAddr��Flash��ʼ��ַ
  * @retval �洢������������ֽڳ��ȣ������ܳ�����ֵ�����ݡ��ָ�����crc����������
  */
u32 getCatParaChLenInFlash(u32 startAddr){
	u32 catParaChLen;
	if(FLASH_HEAD_LENGTH_BYTES==4){//32bits����
		catParaChLen = (u32)STMFLASH_ReadWord(startAddr);
	}else if(FLASH_HEAD_LENGTH_BYTES==2){  //16bits����
		catParaChLen = (u32)STMFLASH_ReadHalfWord(startAddr);
	}else if(FLASH_HEAD_LENGTH_BYTES==1){  //8bits����
		catParaChLen = (u32)STMFLASH_ReadByte(startAddr);
	}
	return catParaChLen;
}

/**
  * @brief  ��ȡ�����Ĳ�������������У��
  * @param  catPara���洢��ȡ�Ĳ���������ָ��
  * @param  startAddr��������ȡ����ʼ��ַ
  *          @arg FLASH_SAVE_ADDR_MAIN�����洢������ʼ��ַ
  *          @arg FLASH_SAVE_ADDR_BACKUP�����ݴ洢������ʼ��ַ
  * @retval ������ȡ״̬
  *          @arg LOAD_PARA_CRC_ERROR
  *          @arg LOAD_PARA_TOO_LONG_ERROR
  *          @arg LOAD_PARA_POINTER_NULL
  *          @arg LOAD_PARA_SUCCESS
  */
u8 loadParaAndCheck(u8 * catPara,u32 startAddr){
	/*�ȶ�������*/
	u32 catParaChLen = getCatParaChLenInFlash(startAddr);
	#if PRINT_UART_LOG
	printf("Para Array Length : %d\r\n",catParaChLen);
	#endif
	if(catParaChLen>=PARA_CAT_CH_MAX_LENGTH){
//		#if PRINT_UART_LOG
//		printf("Load Para Too Long Error!\r\n");
//		#endif
		return LOAD_PARA_TOO_LONG_ERROR;
	}
	memset(catPara,0,PARA_CAT_CH_MAX_LENGTH);//ȫ����ֵ0
	STMFLASH_ReadBytes(startAddr,catPara,catParaChLen);//��ȡmain������������
	/*CRCУ��*/
	#if PRINT_UART_LOG
	printf("Checking CRC...\r\n");
	#endif
	CRC_TYPE crc = CalCrc(0, (c8 *)catPara,catParaChLen- sizeof(FLASH_LABLE_TYPE) - FLASH_CRC_BYTE_LENGTH,0x8005);
	/*����õ�crcת��Ϊ�ַ��������ȹ̶�����λ��0*/
	u8 crcStr[FLASH_CRC_BYTE_LENGTH+1];memset(crcStr,'0',FLASH_CRC_BYTE_LENGTH);crcStr[FLASH_CRC_BYTE_LENGTH]=0;//��ʼ��"00000\0"
	u8 * crcStrItoa = itoa(crc);
	memcpy((char *)crcStr,crcStrItoa,strlen((c8*)crcStrItoa));//ת��Ϊ�ַ���
	/*��ȡ��flash�е�crc�ַ���*/
	u8 crcStrFlash[FLASH_CRC_BYTE_LENGTH+1]={0};memcpy(crcStrFlash,catPara+catParaChLen-sizeof(FLASH_LABLE_TYPE)-FLASH_CRC_BYTE_LENGTH,FLASH_CRC_BYTE_LENGTH);
	#if PRINT_UART_LOG
	printf("CRC Computed : %s \r\nCRC in Flash : %s\r\n",crcStr,crcStrFlash);
	#endif
	if(strcmp((c8*)crcStr,(c8*)crcStrFlash) != 0){//�Ա�
		#if PRINT_UART_LOG
		printf("CRC Check Error!\r\n");
		#endif
		return LOAD_PARA_CRC_ERROR;
	}else{
		#if PRINT_UART_LOG
		printf("CRC Check Correct!\r\n");
		#endif
	}
	#if PRINT_UART_LOG
	printf("Paras Unzipping...\r\n");
	#endif
	/*�ָ�����*/
	u32 paraStartAddr =(u32) (catPara + FLASH_HEAD_LENGTH_BYTES);//���ݴ洢��ʼ��λ�ã��������е��±�
	/**SSID*/
	u32 splitAddr = (u32) strchr((c8 *)(paraStartAddr),FLASH_LABEL_SPLIT);//�õ��ָ����ĵ�ַ
	memcpy((u8 *)RSI_JOIN_SSID,(u8 *)paraStartAddr,splitAddr - paraStartAddr);*(RSI_JOIN_SSID+splitAddr - paraStartAddr) = 0;//����һ��0
	paraStartAddr = splitAddr+1;//��ʼ��һ������
	#if PRINT_UART_LOG
	printf("RSI_JOIN_SSID : %s\r\n",RSI_JOIN_SSID);
	#endif
	/*PSK*/
	splitAddr = (u32) strchr((c8 *)(paraStartAddr),FLASH_LABEL_SPLIT);//�õ��ָ����ĵ�ַ
	memcpy((u8 *)RSI_PSK,(u8 *)paraStartAddr,splitAddr - paraStartAddr);*(RSI_PSK+splitAddr - paraStartAddr) = 0;//����һ��0
	paraStartAddr = splitAddr+1;//��ʼ��һ������
	#if PRINT_UART_LOG
	printf("RSI_PSK : %s\r\n",RSI_PSK);
	#endif
	/*nodeId*/
	splitAddr = (u32) strchr((c8 *)(paraStartAddr),FLASH_LABEL_SPLIT);//�õ��ָ����ĵ�ַ
	u8 nodeIdStrTemp[NODE_ID_2STR_MAX_LEN+1]={0};memcpy(nodeIdStrTemp,(u8 *)paraStartAddr,splitAddr - paraStartAddr); //������strTemp�ռ�
	nodeId =(u8)atoi((c8 *)nodeIdStrTemp);
	paraStartAddr = splitAddr+1;//��ʼ��һ������
	#if PRINT_UART_LOG
	printf("nodeId : %d\r\n",nodeId);
	#endif	
	/*Server Ip  ��destIp_txrx��*/
	u8 ipStrTemp[IPV4_1GROUP_STR_LENGTH+1]={0};//����һ���飬��"192\0"
	for(u8 i = 0;i<4;i++){
		splitAddr = (u32) strchr((c8 *)(paraStartAddr),FLASH_LABEL_SPLIT);//�õ��ָ����ĵ�ַ
		if(splitAddr == NULL){//��ֹ�����Ҳ����ָ���������
			destIp_txrx[0]=DESTIP_TXRX_GROUP1;destIp_txrx[1]=DESTIP_TXRX_GROUP2;
			destIp_txrx[2]=DESTIP_TXRX_GROUP3;destIp_txrx[3]=DESTIP_TXRX_GROUP4;
			return LOAD_PARA_POINTER_NULL;
		}
		memset(ipStrTemp,0,IPV4_1GROUP_STR_LENGTH+1);//����
		memcpy(ipStrTemp,(u8 *)paraStartAddr,splitAddr - paraStartAddr); //������strTemp�ռ�
		destIp_txrx[i] =(u8)atoi((c8 *)ipStrTemp);
		paraStartAddr = splitAddr+1;//��ʼ��һ��
	}
	#if PRINT_UART_LOG
	printf("destIp_txrx : %d.%d.%d.%d\r\n",destIp_txrx[0],destIp_txrx[1],destIp_txrx[2],destIp_txrx[3]);
	#endif
	return LOAD_PARA_SUCCESS;
}


/**
  * @brief  ����loadParaAndCheck�õ����洢�����߱��ݴ洢��������
  * @param  None
  * @retval None
  */
void loadParafromMainOrBackupFlash(void){
	#if PRINT_UART_LOG
	printf("Load Paras Start ...\r\n");
	printf("Loading Main Flash Paras...\r\n");
	#endif
	if(loadParaAndCheck(catPara,FLASH_SAVE_ADDR_MAIN) != LOAD_PARA_SUCCESS){//���ز������ɹ�
		#if PRINT_UART_LOG
		printf("Loading Main Flash Paras Unsuccessfully!\r\n");
		printf("Loading Backup Flash Paras...\r\n");
		#endif
		if(loadParaAndCheck(catPara,FLASH_SAVE_ADDR_BACKUP) != LOAD_PARA_SUCCESS){
			#if PRINT_UART_LOG
			printf("Loading Backup Flash Paras Unsuccessfully!\r\n");
			#endif			
		}else{
			#if PRINT_UART_LOG
			printf("Loading Main Flash Paras Successfully!\r\n");
			#endif				
		}
	}else{
		#if PRINT_UART_LOG
		printf("Loading Main Flash Paras Successfully!\r\n");
		#endif	
	}
}

/**
  * @brief  д���֣�4�ֽڣ����ݣ��ı䣨INC���ӣ�д��ַWriteAddr
  * @param  pBuff��Ҫд������ݵ�ָ��
  * @param  WriteAddr��Ҫд���Flash��ַ
  * @param  WordLen�������֣�32λ����
  * @retval д��״̬
  *          @arg WRITE_PARA_FAILED
  *          @arg WRITE_PARA_SUCCESS
  */
u8 writeFlashWordsIncAddr(u32 * pBuff,u32 * WriteAddr,u16 WordLen){
	for(int i =0;i<WordLen;i+=1){
		if(FLASH_ProgramByte(*WriteAddr,*(pBuff + i)) != FLASH_COMPLETE){//д������		 
			return WRITE_PARA_FAILED;	//д���쳣
		}
		*WriteAddr += 4;//4byteΪ��λ
	}
	return WRITE_PARA_SUCCESS;
}

/**
  * @brief  д���֣�4�ֽڣ����ݣ����ı䣨INC���ӣ�д��ַWriteAddr
  * @param  pBuff��Ҫд������ݵ�ָ��
  * @param  WriteAddr��Ҫд���Flash��ַ
  * @param  WordLen�������֣�32λ����
  * @retval д��״̬
  *          @arg WRITE_PARA_FAILED
  *          @arg WRITE_PARA_SUCCESS
  */
u8 writeFlashWords(u32 * pBuff,u32 WriteAddr,u16 WordLen){
	u32 writeAddrNow = WriteAddr;
	for(int i =0;i<WordLen;i+=1){
		if(FLASH_ProgramByte(writeAddrNow,*(pBuff + i)) != FLASH_COMPLETE){//д������		 
			return WRITE_PARA_FAILED;	//д���쳣
		}
		writeAddrNow += 4;//4byteΪ��λ
	}
	return WRITE_PARA_SUCCESS;
}

/**
  * @brief  д���ֽ����ݣ��ı䣨INC���ӣ�д��ַWriteAddr
  * @param  pBuff��Ҫд������ݵ�ָ��
  * @param  WriteAddr��Ҫд���Flash��ַ
  * @param  WordLen�������ֽڣ�8λ����
  * @retval д��״̬
  *          @arg WRITE_PARA_FAILED
  *          @arg WRITE_PARA_SUCCESS
  */
u8 writeFlashByteIncAddr(u8 * pBuff,u32 * WriteAddr,u16 u8Len){
	for(int i =0;i<u8Len;i+=1){
		if(FLASH_ProgramByte(*WriteAddr,*(pBuff + i))!=FLASH_COMPLETE){ //д������		
			return WRITE_PARA_FAILED;	//д���쳣
		}
		*WriteAddr += 1;//1byteΪ��λ
	}
	return WRITE_PARA_SUCCESS;
}


/**
  * @brief  д���ֽ����ݣ����ı䣨INC���ӣ�д��ַWriteAddr
  * @param  pBuff��Ҫд������ݵ�ָ��
  * @param  WriteAddr��Ҫд���Flash��ַ
  * @param  WordLen�������ֽڣ�8λ����
  * @retval д��״̬
  *          @arg WRITE_PARA_FAILED
  *          @arg WRITE_PARA_SUCCESS
  */
u8 writeFlashByte(u8 * pBuff,u32 WriteAddr,u16 u8Len){
	u32 writeAddrNow = WriteAddr;
	for(int i =0;i<u8Len;i+=1){
		if(FLASH_ProgramByte(writeAddrNow,*(pBuff + i))!=FLASH_COMPLETE)//д������
		{
			return WRITE_PARA_FAILED;	//д���쳣
		}
		writeAddrNow += 1;//1byteΪ��λ
	}
	return WRITE_PARA_SUCCESS;
}

/**
  * @brief  ��ȡ����������������Ŀռ䣬�������ݳ��ȡ��ָ��������ݡ�������
  * @note   ���ݵ�ǰ�Ĳ��������ȡ�����̶�
  * @param  None
  * @retval �洢�������ֽڳ���
  */
u32 getParaLen(void){
	u16 catChLen = 0;
	/*���ֽڳ�*/
	catChLen += FLASH_HEAD_LENGTH_BYTES;
	/*SSID*/
	catChLen += strlen((c8*)RSI_JOIN_SSID);
	/*�ָ���*/
	catChLen += sizeof(FLASH_LABLE_TYPE); 
	/*PSK*/
	catChLen += strlen((c8*)RSI_PSK);
	/*�ָ���*/
	catChLen += sizeof(FLASH_LABLE_TYPE);
	/*nodeId��ת��Ϊ�ַ�����ĳ��ȣ���23->"23"����Ϊ����2*/
	catChLen += strlen((c8 *)itoa(nodeId)); 
	/*�ָ���*/
	catChLen += sizeof(FLASH_LABLE_TYPE);
	/*IP*/
	catChLen += strlen((c8 *)itoa(destIp_txrx[0]));//IP��1��
	catChLen += sizeof(FLASH_LABLE_TYPE);
	catChLen += strlen((c8 *)itoa(destIp_txrx[1]));//IP��2��
	catChLen += sizeof(FLASH_LABLE_TYPE); 
	catChLen += strlen((c8 *)itoa(destIp_txrx[2]));//IP��3��
	catChLen += sizeof(FLASH_LABLE_TYPE);
	catChLen += strlen((c8 *)itoa(destIp_txrx[3]));//IP��4��
	catChLen += sizeof(FLASH_LABLE_TYPE);
	/*CRCУ��*/
	catChLen += FLASH_CRC_BYTE_LENGTH;//����һ��CRCУ��

	/*������*/
	catChLen += sizeof(FLASH_LABLE_TYPE); //��������ռ��1byte
//#if PRINT_UART_LOG	
//	printf("strlen(RSI_JOIN_SSID) : %d\r\n",strlen((c8*)RSI_JOIN_SSID));
//	printf("FLASH_LABEL_SPLIT : %d\r\n",sizeof(FLASH_LABLE_TYPE));
//	printf("strlen(RSI_PSK) : %d\r\n",strlen((c8*)RSI_PSK));
//	printf("CRC_TYPE : %d\r\n",FLASH_CRC_BYTE_LENGTH);
//	printf("FLASH_LABEL_END : %d\r\n",sizeof(FLASH_LABLE_TYPE));
//#endif
	return 	catChLen;
}

/**
  * @brief  ������Ҫ��������ݶ���������,�м��÷ָ����ֿ�
  * @param  None
  * @retval ���ݵĳ��ȣ�����������'\0'
  *         0����ʾ����ʧ��
  */
u32 getCatPara(void){
	/*��ȡҪ����Ĳ������ֽڳ���*/
	u32 catParaLen = getParaLen();
	#if PRINT_UART_LOG
	printf("catParaLen = %d\r\n",catParaLen);
	#endif	
	u32 i=0;
	//��chLen
	for(;i<FLASH_HEAD_LENGTH_BYTES;i++){
		*(catPara+i) = (u8)(catParaLen>>(8*i));
	}
	//��ssid
	memcpy(catPara+i,RSI_JOIN_SSID,strlen((c8*)RSI_JOIN_SSID));i += strlen((c8*)RSI_JOIN_SSID);
	*(catPara+i) = FLASH_LABEL_SPLIT;i += sizeof(FLASH_LABLE_TYPE);//�ָ���1byte
	//��PSK
	memcpy(catPara+i,RSI_PSK,strlen((c8*)RSI_PSK));i += strlen((c8*)RSI_PSK);
	*(catPara+i) = FLASH_LABEL_SPLIT;i += sizeof(FLASH_LABLE_TYPE);//�ָ���1byte
	//��nodeId
	u8 nodeIdStr[NODE_ID_2STR_MAX_LEN+1] = {0};strcpy((char *)nodeIdStr,(c8 *)itoa(nodeId));//ת��ΪStr
	memcpy((char *)(catPara+i),(c8 *)nodeIdStr,strlen((c8*)nodeIdStr));i += strlen((c8*)nodeIdStr);//����
	*(catPara+i) = FLASH_LABEL_SPLIT;i += sizeof(FLASH_LABLE_TYPE);//�ָ���1byte
	//��IP�������ĸ�byte"IP1\nIP2\nIP3\nIP4"��IPV4_STR_LENGTH = 15,�����м�ķָ��
	//1.����,ע������itoaָ��һ����ַ�����Բ�����4��ָ��ͬʱָ�򣬻����ָ������һ��ip��
	u8 ip1[IPV4_1GROUP_STR_LENGTH+1]={0};strcpy((char *)ip1,(c8 *)itoa(destIp_txrx[0]));
	u8 ip2[IPV4_1GROUP_STR_LENGTH+1]={0};strcpy((char *)ip2,(c8 *)itoa(destIp_txrx[1]));
	u8 ip3[IPV4_1GROUP_STR_LENGTH+1]={0};strcpy((char *)ip3,(c8 *)itoa(destIp_txrx[2]));
	u8 ip4[IPV4_1GROUP_STR_LENGTH+1]={0};strcpy((char *)ip4,(c8 *)itoa(destIp_txrx[3]));
	//2.д������
	memcpy((char *)(catPara+i),ip1,strlen((c8 *)ip1));i += strlen((c8 *)ip1);
	*(catPara+i) = FLASH_LABEL_SPLIT;i += sizeof(FLASH_LABLE_TYPE);//�ָ���1byte
	memcpy((char *)(catPara+i),ip2,strlen((c8 *)ip2));i += strlen((c8 *)ip2);
	*(catPara+i) = FLASH_LABEL_SPLIT;i += sizeof(FLASH_LABLE_TYPE);//�ָ���1byte	
	memcpy((char *)(catPara+i),ip3,strlen((c8 *)ip3));i += strlen((c8 *)ip3);
	*(catPara+i) = FLASH_LABEL_SPLIT;i += sizeof(FLASH_LABLE_TYPE);//�ָ���1byte
	memcpy((char *)(catPara+i),ip4,strlen((c8 *)ip4));i += strlen((c8 *)ip4);
	*(catPara+i) = FLASH_LABEL_SPLIT;i += sizeof(FLASH_LABLE_TYPE);//�ָ���1byte
	//��crc
	CRC_TYPE crc = CalCrc(0, (c8 *)catPara,catParaLen- FLASH_CRC_BYTE_LENGTH - sizeof(FLASH_LABLE_TYPE),0x8005);//�õ�crc,�ѽ������ź�crcλ��ȥ��
	#if PRINT_UART_LOG
	printf("GET CRC VAL : %d\r\n",crc);
	#endif
	u8 * crcStr = itoa(crc);
	u8 crcStrKeepZero[FLASH_CRC_BYTE_LENGTH+1] = {0};
	memset(crcStrKeepZero,'0',FLASH_CRC_BYTE_LENGTH);//'0'...'0''\0'
	memcpy(crcStrKeepZero,crcStr,strlen((c8 *)crcStr));//���������ں����'\0'
	memcpy(catPara+i,crcStrKeepZero,strlen((c8*)crcStrKeepZero));i += strlen((c8*)crcStrKeepZero);//crcStr
	#if PRINT_UART_LOG
	printf("GET CRC STR VAL : %s\r\n",crcStrKeepZero);
	#endif
//	for(u16 i_this;i_this<sizeof(CRC_TYPE);i++,i_this++){//��crc16
//		*(catPara+i) = (u8)(crc>>(8*i_this));
//	}
	*(catPara+i) = FLASH_LABEL_END;i += sizeof(FLASH_LABLE_TYPE);//��β
	*(catPara+i) = '\0';//����ҪҲ���ԣ�calloc��ʼ����ʱ���Ѿ�ȫ����ʼ��Ϊ0
	#if PRINT_UART_LOG
	printf("catPara : %s len : %d\r\n",catPara+FLASH_HEAD_LENGTH_BYTES,strlen((c8*)(catPara+FLASH_HEAD_LENGTH_BYTES)));
	#endif
	
	if(catParaLen-FLASH_HEAD_LENGTH_BYTES == strlen((c8 *) (catPara + FLASH_HEAD_LENGTH_BYTES))){
	#if PRINT_UART_LOG
		printf("LEN IS CORRECT!\r\n");
	#endif
	}else{
	#if PRINT_UART_LOG
		printf("LEN %d(Real : %d) IS NOT CORRECT!\r\n",strlen((c8 *) (catPara + FLASH_HEAD_LENGTH_BYTES)),catParaLen-FLASH_HEAD_LENGTH_BYTES);
	#endif
	}
	return catParaLen;
}

/**
  * @brief  д�������в��������ݵ����ͱ�������
  * @param  None
  * @retval None
  */
void writeSectorPara(void){
	u32 catParaLen = getCatPara();
//	if(catParaLen){//���������0��˵���������ݵ��������ʧ���ˡ���д��ֱ�ӷ���	
//		printf("\r\nCALLOC Failed!\r\nStop Writing Flash!\r\n");	
//		return;
//	}
	FLASH_Status status =FLASH_EraseSector(FLASH_SAVE_SECTOR_MAIN,VoltageRange_3);//�������������洢������������
	if(status != FLASH_COMPLETE){//δ�ɹ��������ݣ�����ʧ��
	#if PRINT_UART_LOG
		printf("Main flash Erased Unsuccessfully!\r\n");
	#endif
	}else{
	#if PRINT_UART_LOG
		printf("Main flash Erased Successfully!\r\n");
	#endif
	}
	if(writeFlashByte(catPara,FLASH_SAVE_ADDR_MAIN,catParaLen) == WRITE_PARA_FAILED){ //ʧ��
	#if PRINT_UART_LOG
		printf("Main flash Writen Unsuccessfully!\r\n");
	#endif
	}else{
	#if PRINT_UART_LOG
		printf("Main flash Writen Successfully!\r\n\r\n");
	#endif
	}
	status = FLASH_EraseSector(FLASH_SAVE_SECTOR_BACKUP,VoltageRange_3);//�������������洢������������
	if(status != FLASH_COMPLETE){//δ�ɹ��������ݣ�����ʧ��
	#if PRINT_UART_LOG
		printf("Backup flash Erased Unsuccessfully!\r\n");
	#endif
	}else{
	#if PRINT_UART_LOG
		printf("Backup flash Erased Successfully!\r\n");
	#endif
	}
	if(writeFlashByte(catPara,FLASH_SAVE_ADDR_BACKUP,catParaLen) == WRITE_PARA_FAILED){
	#if PRINT_UART_LOG
		printf("Backup flash Writen Unsuccessfully!\r\n");
	#endif
	}else{
	#if PRINT_UART_LOG
		printf("Backup flash Writen Successfully!\r\n");
	#endif
	}
}

/**
  * @brief  ��ĳ����ַ��ֵ0
  * @param  None
  * @retval None
  */
void putZero(u8 * pChar,u16 i){
	*(pChar+i) = 0;
}


/**
  * @brief  ��ȡsplitCmd������Ҫ���ص�״̬
  * @param  pCmd���洢����CMD���ַ������ĵ�ַ���ַ���
  * @param  pValue���洢��Ϣ����Ҫ���õĲ����ȣ��ĵ�ַ���ַ�����ʽ������Ҳ��ʾΪ�ַ�������Ҫת�������֣�
  * @retval ����/��Ϣ���
  *         @arg CMD_VALUE_SPLIT_ERROR
  *         @arg CMD_VALUE_SPLIT_OK
  *         @arg CMD_SPLIT_OK
  */
u8 getSplitCmdFunReturn(u8* pCmd,u8 * pValue){
	if(*pValue != 0  &&  *pCmd != 0){//cmd��val�����ǿյ�
		return CMD_VALUE_SPLIT_OK;
	}else if(*pCmd != 0){ //���cmd���ǿյ�
		return CMD_SPLIT_OK;
	}else{//�������ǿյķ���error����ʾû��ָ����߷ָ�ָ��ʧ��
		return CMD_VALUE_SPLIT_ERROR;
	}
}

/**
  * @brief  ר��Ϊ����splitCmd�������������
  * @param  pQueue���洢���������Ϣ���Ĵ洢�ռ䣨���У���ַ
  * @param  pCmd���洢����ĵ�ַ
  * @param  pValue���洢��Ϣ�ĵ�ַ
  * @param  i_cmd��pCmd��Ҫ����ĵ�ַ��pCmd+i_cmd��
  * @param  i_val��pValue��Ҫ����ĵ�ַ��pValue+i_val��
  * @retval None
  */
static void clearPara4SplitCmd(volatile CMD_QUEUE * pQueue,u8 * pCmd,u8 * pValue,u16 i_cmd,u16 i_val){
		uart_queue_clear(pQueue);
		putZero(pValue,i_val);
		putZero(pCmd,i_cmd);	
}

/**
  * @brief  �Ӵ洢�������Ϣ�Ķ����У��ָ������(cmd)����Ϣ(value)
  * @param  pQueue���洢���������Ϣ���Ĵ洢�ռ䣨���У���ַ
  * @param  pCmd���洢����ĵ�ַ
  * @param  pValue���洢��Ϣ�ĵ�ַ
  * @retval CMD��VAL�ָ����
  *         @arg CMD_VALUE_SPLIT_ERROR
  *         @arg CMD_VALUE_SPLIT_OK
  *         @arg CMD_SPLIT_OK
  *         @arg NONE_CMD_VALUE_MSG
  */
u8 splitCmd(volatile CMD_QUEUE * pQueue,u8 * pCmd,u8 * pValue)
{
	u16 i_cmd=0,i_val=0;
	u8 SplitFlag = CMD_VALUE_NOT_SPLIT_STA;
	u8 uart_data;
	u16 length = uart_queue_length(pQueue); //�õ�����	
	for(u16 forNum = 0;pQueue->CmdCompleteFlag==CMD_COMPLETED;)//һ��cmd�����
	{		
		if(forNum >= length){  //���������ݸ���
			clearPara4SplitCmd(pQueue,pCmd,pValue,i_cmd,i_val);
			#if PRINT_UART_LOG
				printf("CMD Has No \\r\\n!\r\n");
			#endif			
			return CMD_VALUE_SPLIT_ERROR;
		}
		uart_data = uart_queue_pop(pQueue);
		forNum++;
		
		if(uart_data == CMD_VALUE_SPLIT_CHAR && SplitFlag != CMD_VALUE_SPLITED_STA)//�ָ������˶���ֻ����һ��Ҳ����֧������"SET    111"���������ݣ�����Ѻ��漸���ո����ָ���
		{
			SplitFlag = CMD_VALUE_SPLITED_STA;
		}
		else //���Ƿָ���
		{
//			printf("%c",uart_data);
			if(uart_data != '\r')
			{
				if(uart_data == '\n') //ǰ��û��'\r'ֱ�ӳ�����\n
				{
					clearPara4SplitCmd(pQueue,pCmd,pValue,i_cmd,i_val);
					#if PRINT_UART_LOG
						printf("CMD Has No \\r!\r\n");
					#endif
					return CMD_VALUE_SPLIT_ERROR;
				}		
				if(SplitFlag == CMD_VALUE_NOT_SPLIT_STA) //��û�е��ָ�����Ҳ���ǻ��ڻ�ȡcmd
				{
					*(pCmd+i_cmd) = uart_data;
					i_cmd++;
				}
				else
				{
					*(pValue+i_val) = uart_data;
					i_val++;
				}

			}
			else if(uart_data == '\r' && uart_queue_pop(pQueue) == '\n')  //\r\n   ��������
			{
				clearPara4SplitCmd(pQueue,pCmd,pValue,i_cmd,i_val);
				return getSplitCmdFunReturn(pCmd,pValue);//����ֹͣʱ����Ҫ�ж���cmd����cmd+val
			}	
			else if(uart_data == '\r' && uart_queue_pop(pQueue) != '\n') //������"\r" "��\n"�����
			{
				clearPara4SplitCmd(pQueue,pCmd,pValue,i_cmd,i_val);
				#if PRINT_UART_LOG
					printf("CMD Has No \\n!\r\n");
				#endif
				return CMD_VALUE_SPLIT_ERROR;
			}
		}
	}
	return NONE_CMD_VALUE_MSG;
	
}


/**
  * @brief  ����������MCU
  * @note   ��Ҫ���ʹ�ã�����Ϊstatic
  * @param  None
  * @retval None
  */
static void SystemReset(void){
	__DSB();                                                     /* Ensure all outstanding memory accesses included
															  buffered write are completed before reset */
	SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      |
				(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
				SCB_AIRCR_SYSRESETREQ_Msk);                   /* Keep priority group unchanged */
	__DSB();                                                     /* Ensure completion of memory access */
	while(1);  
}

/**
  * @brief  ��ȡ����/APģʽ�����ĵ�
  * @note   ÿ��������Ҫ����
  * @note   ��Ҫ��Դ��ڣ�APģʽ����ָ�֧��
  * @param  None
  * @retval None
  */
void getHelp(void){
	#if PRINT_UART_LOG
		printf("\r\n============  Help Document  ============ \r\n\r\n");
		//CMD + VAL
		printf("- SET_RSI_JOIN_SSID    : Set RSI_JOIN_SSID\r\n  E.G. SET_RSI_JOIN_SSID SORL_WIFI\r\n\r\n");
		printf("- SET_RSI_PSK          : Set RSI_PSK\r\n  E.G. SET_RSI_PSK 123456\r\n\r\n");
	    printf("- SET_NODE_ID          : Set Id of the Node\r\n  E.G. SET_NODE_ID 1\r\n\r\n");
		printf("- SET_SERVER_IP        : Set IP of the Server\r\n  E.G. SET_SERVER_IP 115.159.154.160\r\n\r\n");
		//CMD
		printf("- HELP                 : Print Help Document\r\n  E.G. HELP\r\n\r\n");
		printf("- GET_PARA             : Print Parameters List\r\n  E.G. GET_PARA\r\n\r\n");
		printf("- SAVE_ALL_PARA        : Save All Parameters in Flash\r\n  E.G. SAVE_ALL_PARA\r\n\r\n");
		printf("- RESET_SYSTEM         : Reboot MCU\r\n  E.G. RESET_SYSTEM\r\n\r\n");
	#endif
}

/**
  * @brief  �������������
  * @note   ÿ��������Ҫ����
  * @param  None
  * @retval None
  */
void getPara(void){
	#if PRINT_UART_LOG
		printf("\r\n============  Parameter List  ============ \r\n\r\n");
		printf("RSI_JOIN_SSID     :      %s\r\n",RSI_JOIN_SSID);
		printf("RSI_PSK           :      %s\r\n",RSI_PSK);
		printf("nodeId            :      %d\r\n",nodeId);
		printf("destIp_txrx       :      %d.%d.%d.%d\r\n",destIp_txrx[0],destIp_txrx[1],destIp_txrx[2],destIp_txrx[3]);
		printf("localhost         :      %d.%d.%d.%d\r\n",localhost[0],localhost[1],localhost[2],localhost[3]);
		printf("test_name         :      %s\r\n",adc_queue.test_name);
		if(RSI_WIFI_OPER_MODE == RSI_WIFI_CLIENT_MODE_VAL){
			printf("Module Mode       :      CLIENT\r\n");
		}else if(RSI_WIFI_OPER_MODE == RSI_WIFI_AP_MODE_VAL){
			printf("Module Mode       :      AP\r\n");
		}
		if(RSI_IP_CFG_MODE == RSI_DHCP_IP_MODE){
			printf("Module Ip Mode    :      DHCP\r\n");
		}else if(RSI_IP_CFG_MODE == RSI_WIFI_AP_MODE_VAL){
			printf("Module Ip Mode    :      STATIC\r\n");
		}
		if(RSI_BAND == RSI_DUAL_BAND){
			printf("Frequency Band    :      DUAL BAND(2.4/5GHz)\r\n");
		}else if(RSI_BAND == RSI_BAND_5GHZ){
			printf("Frequency Band    :      5GHz\r\n");
		}else if(RSI_BAND == RSI_BAND_2P4GHZ){
			printf("Frequency Band    :      2.4GHz\r\n");
		}		
				
	#endif
}

/**
  * @brief  �����ѷָ�Ĵ�����
  * @param  cmd������ָ��
  * @retval None
  */
void handleCmd(c8 * cmd){
#if PRINT_UART_LOG
	printf("Received CMD:\r\n  %s\r\n",cmd);
#endif
	if(strcmp(cmd,CMD_HELP) == 0){//�����ļ�
		getHelp();
	}else if(strcmp(cmd,CMD_GET_PARA) == 0){
#if PRINT_UART_LOG
		getPara();
#endif		
	}else if(strcmp(cmd,CMD_SAVE_ALL_PARA) == 0){//�����������ݵ�flash
#if PRINT_UART_LOG
		printf("Saving All Paras\r\n");
#endif
		FLASH_Unlock();//���� 
		FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
		writeSectorPara();
		FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
		FLASH_Lock();//����
#if PRINT_UART_LOG
		printf("Saving Process End\r\n");
#endif
	}else if(strcmp(cmd,CMD_RESET_SYSTEM) == 0){//��������wifi
#if PRINT_UART_LOG
		printf("Reset...\r\n");
#endif
        SystemReset();                                      /* wait until reset */
	}
}

/**
  * @brief  �����ַ��������
  * @param  val��������ֵ���ַ�����
  * @param  obj��Ҫ���õĶ���ָ�루�ַ�����
  * @param  objName��Ҫ���õĶ�����ַ�������
  * @retval None
  */
void setStrVal(c8 * val,c8 * obj,c8 * objName){
#if PRINT_UART_LOG	
	printf("Setting %s...\r\n",objName);
#endif
	memcpy((u8 *)obj,val,strlen(val));
	memset((u8 *)(obj+strlen(val)),'\0',1);//����渳ֵ0
#if PRINT_UART_LOG
	printf("Now %s = \"%s\"\r\n",objName,obj);
	printf("Setted %s OK\r\n",objName);
	printf("To Save Flash.Send \"%s\"\r\n",CMD_SAVE_ALL_PARA);	
#endif
}


/**
  * @brief  �����ַ��������
  * @param  val��������ֵ���ַ���������Ҫ���ַ���ת��Ϊ���֣���'15'��Ҫת��Ϊ(u8)15
  * @param  obj��Ҫ���õĶ���ָ�루8λ�޷����������ͣ�
  * @param  objName��Ҫ���õĶ�����ַ�������
  * @retval None
  */
void setU8Val(u8 * val,u8 * obj,c8 * objName){//����string value
#if PRINT_UART_LOG	
	printf("Setting %s...\r\n",objName);
#endif
	u8 valU8 = atoi((c8*)val);
	memcpy((u8 *)obj,&valU8,sizeof(u8));
#if PRINT_UART_LOG
	printf("Now %s = %d\r\n",objName,*obj);
	printf("Setted %s OK\r\n",objName);
	printf("To Save Flash.Send \"%s\"\r\n",CMD_SAVE_ALL_PARA);	
#endif
}

/**
  * @brief  �����ַ��������
  * @param  val��IP������ֵ��ַ����Ҫ���ַ���ת��Ϊu8���飬ÿ��u8������IP_GROUP_SPLIT����
  * @param  obj��Ҫ���õĶ���ָ�루u8���飩
  * @param  objName��Ҫ���õĶ�����ַ�������
  * @retval None
  */
void setIpVal(u8 * val,u8 * obj,c8 * objName){
#if PRINT_UART_LOG	
	printf("Setting %s...\r\n",objName);
#endif
	u32 startAddr = (u32)(val);//��ʼ��ַ
	u32 splitAddr;//�ָ�����ķָ�����ַ
	u8 ip_n[IPV4_1GROUP_STR_LENGTH+1] = {0};//�洢IP��һ��С��
	for(u8 i = 0;i<3;i++){//ǰ����
		splitAddr = (u32)strchr((c8 *)startAddr,IP_GROUP_SPLIT);//�õ���һ���ָ�����ַ
		memcpy(ip_n,(u8 *)startAddr,splitAddr - startAddr);destIp_txrx[i] = (u8)atoi((c8 *)ip_n);
		memset(ip_n,0,IPV4_1GROUP_STR_LENGTH+1);//����	
		startAddr = splitAddr+1;//��ʼ��һ��
	}
	//���ĸ�
	strcpy((char *)ip_n,(c8*)startAddr);destIp_txrx[3] = (u8)atoi((c8 *)ip_n);
#if PRINT_UART_LOG
	printf("Now %s = %d.%d.%d.%d\r\n",objName,destIp_txrx[0],destIp_txrx[1],destIp_txrx[2],destIp_txrx[3]);
	printf("Setted %s OK\r\n",objName);
	printf("To Save Flash.Send \"%s\"\r\n",CMD_SAVE_ALL_PARA);	
#endif
}

/**
  * @brief  �����ѷָ���������Ϣ
  * @param  cmd������ָ��
  * @param  val����Ϣָ��
  * @retval None
  */
void handleCmdVal(c8 * cmd,c8 * val){
#if PRINT_UART_LOG
	printf("RECEIVED\r\n  CMD : %s \r\n  VAL : %s\r\n",cmd,val);
#endif
	if(!strcmp(cmd,CMD_SET_JOIN_SSID)){  //����ssid
		setStrVal(val,(c8*)RSI_JOIN_SSID,RSI_JOIN_SSID_STRNAME);
	}else if(!strcmp(cmd,CMD_SET_PSK)){  //����psk
		setStrVal(val,(c8*)RSI_PSK,RSI_PSK_STRNAME);
	}else if(!strcmp(cmd,CMD_SET_NODE_ID)){  //���ýڵ�id
		setU8Val((u8 *)val,&nodeId,NODE_ID_STRNAME);
	}else if(!strcmp(cmd,CMD_SET_SERVER_IP)){//���÷�����IP
		setIpVal((u8 * )val,(u8 * )destIp_txrx,SERVER_IP_STRNAME);
	}
	

}

/**
  * @brief  �����������Ϣ�ָ����
  * @note   Ŀǰ�����κδ���
  * @param  None
  * @retval 0
  */
u8 handleSplitError(){
	//TODO
	return 0;
}

/**
  * @brief  ����δ�ָ���������Ϣ������������
  * @param  pQueue���洢�������Ϣ�Ķ���
  * @retval None
  */
void dealCmdMsg(volatile CMD_QUEUE * pQueue){
	u8 cmd[USART_REC_CMD_LEN];
	u8 val[USART_REC_VAL_LEN];
	u8 SplitSta = splitCmd(&CMD_RX_BUF,cmd,val);
	switch(SplitSta){
		case CMD_VALUE_SPLIT_OK:   //cmd   value����
			handleCmdVal((c8 *)cmd,(c8 *)val);
			break;
		case CMD_SPLIT_OK:  //ֱ���ܵ���cmd
			handleCmd((c8 *)cmd);
			break;
		case CMD_VALUE_SPLIT_ERROR: //����split����
			handleSplitError();
		#if PRINT_UART_LOG
			printf("Received CMD ERROR\r\n");
		#endif
			break;
		case NONE_CMD_VALUE_MSG:   //û�н��յ�����
//		#if PRINT_UART_LOG
//			printf("None CMD\r\n");
//		#endif
			break;
		default:
			
			break;
	}
}

/**
  * @brief  �������ã���flash��д����Ϣ
  * @note   �����Ƚ�����ʱ���ܵĳ�ʼ��(delay_init)
  * @param  None
  * @retval None
  */
void setFactory(void){
	FLASH_Unlock();//���� 
	FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���	
	writeSectorPara();	
	FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����	
}




