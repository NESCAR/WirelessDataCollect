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

# 3��ϵͳ����
## 3.1 ����/APģʽ����

```cpp
/** !���ò���������*/
#define CMD_SET_JOIN_SSID      "SET_RSI_JOIN_SSID"   /*!< ����SSID>*/
#define CMD_SET_PSK            "SET_RSI_PSK"         /*!< ����PSK>*/
#define CMD_SET_NODE_ID        "SET_NODE_ID"         /*!< ���ýڵ�id>*/
#define CMD_SET_SERVER_IP      "SET_SERVER_IP"       /*!< ����˽���Ʒ�����IP>*/
/** !������*/
#define CMD_HELP               "HELP"                /*!< �������>*/
#define CMD_GET_PARA           "GET_PARA"            /*!< ����������>*/
#define CMD_SAVE_ALL_PARA      "SAVE_ALL_PARA"       /*!< �������в���>*/
#define CMD_RESET_SYSTEM       "RESET_SYSTEM"        /*!< ϵͳ����>*/
```

## 3.2 ��λ��-�ڵ㽻��

```cpp
#define  GET_TIME_SYNC_PC           0xa1   /*!< ������λ����ͬ���ź�>*/
#define  RETURN_INFO                0xa2   /*!< ������Ϣ>*/
#define  GET_TEST_NAME              0xa3   /*!< ��ȡ��������>*/
#define  GET_TIME_SYNC_MAIN_CLOCK   0xa4   /*!< ����������ͬ���ź�>*/
#define  GET_WIFI_SEND_EN           0xa5   /*!< ���ݲɼ��ͷ���ʹ��>*/
#define  GET_WIFI_SEND_DISABLE      0xa6   /*!< ���ݲɼ��ͷ���ʧ��>*/
#define  GET_CHANNEL_MODEL          0xa7   /*!< ����ͨ���ĵ���/��ѹģʽ>*/
#define  GET_CAN_FILTER_ID          0xa8   /*!< CAN���ݲɼ���ID�б�����>*/
#define  GET_REMOTE_IP_PORT         0xa9   /*!< ���÷������ݶ���˽���Ʒ�������IP>*/
#define  PAGING                     0xaa   /*!< Ѱ���ź�>*/
```
