# 1������

### 1������Ƶ��

wifi_main.c--->

retval = rsi_band(RSI_BAND); 

### 2��ѡ������

wifi_main.c--->

retval = rsi_select_antenna(1,8,8);//1���ⲿUFL���ߣ�8:2.4GHz���棬8:5GHz����

# 2�������ͼ

* ������

Initialization��ʼ��     ---->   ���������շ�txrx��socket��ͬ��sync��socket   ------>     �������Ծ�������PC������       ----->

����������ַ        ----->      ����ͬ��ʱ�ӣ�Time_Sync_Flag=1     ------->     ������������Ϣ     ----->      

��һ�ο��ܷ���һ��ֻ����ͬ��ʱ�����Ϣ��Time_Sync_Flag=0        ---->       ��������������Ϣ

* ��ʱ���ж�

1��1ms�ɼ�һ������

2���������ʱ���豸��ÿSYNC_INTERVAL_TIME*1ms����һ��ʱ��ͬ���ź�

# 2���汾�޸����ݣ�



### 2018-8-27

�ع�������Ľ������������while��

��Ϊ��send����ʱ��Ҳ��spi��������ж��з�����spi����ᱻ��ϣ����ֿ��������⡣


<del>
1��opensocket��ReadPKT����ʹ�ã�8-24ɾ���ˣ�

2��Ϊ�˱������ж��У�Order_anal()�����е�checkPKT��IP�����������Ϣ����check��

ԭ�����߼����⣺OpenSocket����Order_anal�����С�OpenSocket���͹ر�ԭsocket����Ϣ----���ȴ�������Ϣ ----��  ��Ϣ�����������޷�����жϣ�OpenSocket���ж��еȴ���
----�� Opensocket���ڵȴ���Ϣ��ֱ���ȴ�ʱ�䵽  ----��  ��һ�ν����жϣ�Order_anal���������ݣ�����pending��1   ----��   OpenSocketʼ��û�н��յ�������Ϣ

���ڵ��߼���Order_anal�У����յ�����IP�Ͷ˿ڸ�������󣬲����йرվ�socket����socket��������λflag�����������жϣ������ж�Ҳ����check�����棩Ҫ����IP�Ͷ˿��ˡ�  ----��   ��while�и�������IP�Ͷ˿�    ----��flag�����

3����rsi_spi_send�м�����ʱ���޶�

���time++����50000�Σ�����������ֹ������

</del>

### 2018-8-24

1�������ʱ���յ�ͬ���ź���˸���ź�

2��ɾ��opensocket��readPKT�����������⣬���Ժ�������޸ģ�����ɾ����

��Ϊ���ж��е��������ݽ��գ���check��

3������GET_TEST

���ڷ��ر���ID����λ��

### 2018-8-13
1������豸����λ������Ļظ�

��[0xa2,����,�豸id]����ʽ

2����ͬ�豸��ͬͨ���ĵ�����ѹ�ɼ�����

����λ���������ʽʵ��

### 2018-8-11
1�����ݲɼ�����ֻ���ڿ�ʼ���������յ����ٿ�ʼ��

��ʱ���ж��е��޸ġ�

2������ʱ��ʱ��LED2��˸��

### 2018-8-9
1��wifi_send_package�е�moduleSocketҪ��Ϊdest

����ָ����¥Զ��ip��Զ�̵Ķ˿ڡ�

2�Send_Sync_Time�޸�

�±�������˿���������ֱ��

���ø��ˣ���ΪSTM32��С��ģʽ��Сλ�����ڵ͵�ַ����λ�����ڸߵ�ַ

3��destSocket_sync��moduleSocket_sync�ĳ�һ��

4��led1��led2=0


### 2018-8-7

1���޸Ĵ���LUDP socket����

OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);

2�����������ʱ�Ӻʹ�ʱ�ӵ�#if

��IAM_MASTER_CLOCK����

3����Ӿ�����PC�˷���ͬ��ʱ�������

4��receive_udp_package�����޸�

��������ʱ�Ӻʹ�ʱ���豸���������������ͬ�ķ�Ӧ

5���ж϶����Ƿ���ڵ���UDP_SEND_SIZE - 10

if(queue_length(adc_queue) >= UDP_SEND_SIZE - 10 )//�Ҿ���>=�ȽϺã���һû���ü�����==UDP_SEND_SIZE - 10�������

6���ڶ�ʱ���жϷ������м�����ʱ��ͬ������

Send_Sync_Time()

7����order_anay�д����ͷ��������ӵ�socket

case GET_REMOTE_IP_PORT:            //������ַ
	memcpy(destIp_txrx,&arr[1],4);
	memcpy(&destSocket_txrx,&arr[5],2);
	rsi_socket_close(socketDescriptor_txrx, moduleSocket_txrx);//�رյ�ԭ����socket
	OpenLudpSocket(destIp_txrx,destSocket_txrx,moduleSocket_txrx,&socketDescriptor_txrx);
	break;
	





