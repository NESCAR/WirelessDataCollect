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
	





