ע��㣺

1��SPI��������WiFiģ��󣬻᷵����Ӧ��Read_PKT()����


������


1������Ƶ��

wifi_main.c--->

retval = rsi_band(RSI_BAND); 

2��ѡ������

wifi_main.c--->

retval = rsi_select_antenna(1,8,8);//1���ⲿUFL���ߣ�8:2.4GHz���棬8:5GHz����

3��UDP��opensocket�Ŀ���

wifi_main.c--->

OpenSocket("192.168.100.2",5002,5001,RSI_SOCKET_LUDP );�Ƿ���Ҫ

4��concurrentģʽ����APģʽ������







