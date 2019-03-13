/**
  ******************************************************************************
  * @file    typetrans.c
  * @author  zju.nesc.iotlab �㽭��ѧNeSC������������ʵ����
  * @version V1.0
  * @date    13-March-2019
  * @brief   integer-string trans functions   ����-�ַ���ת������
  ******************************************************************************
  */

//-------------include---------------//
#include "typetrans.h"
#include "config.h"

/**
  * @brief  ��ת�ַ���
  * @param  s��Ҫ��ת���ַ���
  * @retval ת������ַ���
  */
u8 *reverse(char *s){
    char temp;
	/*pָ��s��ͷ��*/
    char *p = s;    
	/*qָ��s��β��*/
    char *q = s;
    while(*q)
    ++q;
    q--;
    
    /* �����ƶ�ָ�룬ֱ��p��q����*/
    while(q > p){
        temp = *p;
        *p++ = *q;
        *q-- = temp;
    }
    return (u8*)s;
}

/**
  * @brief  ����ת�ַ�������123->"123"
  * @param  n������
  * @retval ת������ַ���
  */
u8 *itoa(u32 n)
{
    int i = 0,isNegative = 0;
	/* ����Ϊstatic������������ȫ�ֱ���*/
    static char s[STR_MAX_LENGTH];      
	/* ����Ǹ�������תΪ����*/
    if((isNegative = n) < 0) 
    {
        n = -n;
    }
    do      /* �Ӹ�λ��ʼ��Ϊ�ַ���ֱ�����λ�����Ӧ�÷�ת*/
    {
        s[i++] = n%10 + '0';
        n = n/10;
    }while(n > 0);
    /* ����Ǹ��������ϸ���*/
    if(isNegative < 0)   
    {
        s[i++] = '-';
    }
    s[i] = '\0';    /* �������ַ���������*/
    return reverse(s);
}


