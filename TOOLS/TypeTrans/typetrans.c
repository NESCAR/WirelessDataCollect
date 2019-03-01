#include "typetrans.h"
#include "config.h"
//��ת�ַ���
u8 *reverse(char *s)
{
    char temp;
    char *p = s;    //pָ��s��ͷ��
    char *q = s;    //qָ��s��β��
    while(*q)
    ++q;
    q--;
    
    //�����ƶ�ָ�룬ֱ��p��q����
    while(q > p)
    {
        temp = *p;
        *p++ = *q;
        *q-- = temp;
    }
    return (u8*)s;
}
//����ת�ַ���
u8 *itoa(u32 n)
{
    int i = 0,isNegative = 0;
    static char s[STR_MAX_LENGTH];      //����Ϊstatic������������ȫ�ֱ���
    if((isNegative = n) < 0) //����Ǹ�������תΪ����
    {
        n = -n;
    }
    do      //�Ӹ�λ��ʼ��Ϊ�ַ���ֱ�����λ�����Ӧ�÷�ת
    {
        s[i++] = n%10 + '0';
        n = n/10;
    }while(n > 0);
    
    if(isNegative < 0)   //����Ǹ��������ϸ���
    {
        s[i++] = '-';
    }
    s[i] = '\0';    //�������ַ���������
    return reverse(s);
}


