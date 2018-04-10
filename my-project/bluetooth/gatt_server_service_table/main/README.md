1. gap为蓝牙通用访问规范
```c
/// GAP BLE callback event type
typedef enum {
    ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT        = 0,       /*!< When advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT,             /*!< When scan response data set complete, the event comes */
    ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT,                /*!< When scan parameters set complete, the event comes */
    ESP_GAP_BLE_SCAN_RESULT_EVT,                            /*!< When one scan result ready, the event comes each time */
    ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT,              /*!< When raw advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,         /*!< When raw advertising data set complete, the event comes */
    ESP_GAP_BLE_ADV_START_COMPLETE_EVT,                     /*!< When start advertising complete, the event comes */
    ESP_GAP_BLE_SCAN_START_COMPLETE_EVT,                    /*!< When start scan complete, the event comes */
    ESP_GAP_BLE_AUTH_CMPL_EVT,                              /* Authentication complete indication. */
    ESP_GAP_BLE_KEY_EVT,                                    /* BLE  key event for peer device keys */
    ESP_GAP_BLE_SEC_REQ_EVT,                                /* BLE  security request */
    ESP_GAP_BLE_PASSKEY_NOTIF_EVT,                          /* passkey notification event */
    ESP_GAP_BLE_PASSKEY_REQ_EVT,                            /* passkey request event */
    ESP_GAP_BLE_OOB_REQ_EVT,                                /* OOB request event */
    ESP_GAP_BLE_LOCAL_IR_EVT,                               /* BLE local IR event */
    ESP_GAP_BLE_LOCAL_ER_EVT,                               /* BLE local ER event */
    ESP_GAP_BLE_NC_REQ_EVT,                                 /* Numeric Comparison request event */
    ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT,                      /*!< When stop adv complete, the event comes */
    ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT,                     /*!< When stop scan complete, the event comes */
    ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT,                   /*!< When set the static rand address complete, the event comes */
    ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT,                     /*!< When update connection parameters complete, the event comes */
    ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT,                /*!< When set pkt lenght complete, the event comes */
    ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT,             /*!< When  Enable/disable privacy on the local device complete, the event comes */
    ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT,               /*!< When remove the bond device complete, the event comes */
    ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT,                /*!< When clear the bond device clear complete, the event comes */
    ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT,                  /*!< When get the bond device list complete, the event comes */
    ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT,                     /*!< When read the rssi complete, the event comes */
    ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT,              /*!< When add or remove whitelist complete, the event comes */
    ESP_GAP_BLE_EVT_MAX,
} esp_gap_ble_cb_event_t;
```
2. gatt
```c
/// GATT Server callback function events
typedef enum {
    ESP_GATTS_REG_EVT                = 0,       /*!< When register application id, the event comes */
    ESP_GATTS_READ_EVT               = 1,       /*!< When gatt client request read operation, the event comes */
    ESP_GATTS_WRITE_EVT              = 2,       /*!< When gatt client request write operation, the event comes */
    ESP_GATTS_EXEC_WRITE_EVT         = 3,       /*!< When gatt client request execute write, the event comes */
    ESP_GATTS_MTU_EVT                = 4,       /*!< When set mtu complete, the event comes */
    ESP_GATTS_CONF_EVT               = 5,       /*!< When receive confirm, the event comes */
    ESP_GATTS_UNREG_EVT              = 6,       /*!< When unregister application id, the event comes */
    ESP_GATTS_CREATE_EVT             = 7,       /*!< When create service complete, the event comes */
    ESP_GATTS_ADD_INCL_SRVC_EVT      = 8,       /*!< When add included service complete, the event comes */
    ESP_GATTS_ADD_CHAR_EVT           = 9,       /*!< When add characteristic complete, the event comes */
    ESP_GATTS_ADD_CHAR_DESCR_EVT     = 10,      /*!< When add descriptor complete, the event comes */
    ESP_GATTS_DELETE_EVT             = 11,      /*!< When delete service complete, the event comes */
    ESP_GATTS_START_EVT              = 12,      /*!< When start service complete, the event comes */
    ESP_GATTS_STOP_EVT               = 13,      /*!< When stop service complete, the event comes */
    ESP_GATTS_CONNECT_EVT            = 14,      /*!< When gatt client connect, the event comes */
    ESP_GATTS_DISCONNECT_EVT         = 15,      /*!< When gatt client disconnect, the event comes */
    ESP_GATTS_OPEN_EVT               = 16,      /*!< When connect to peer, the event comes */
    ESP_GATTS_CANCEL_OPEN_EVT        = 17,      /*!< When disconnect from peer, the event comes */
    ESP_GATTS_CLOSE_EVT              = 18,      /*!< When gatt server close, the event comes */
    ESP_GATTS_LISTEN_EVT             = 19,      /*!< When gatt listen to be connected the event comes */
    ESP_GATTS_CONGEST_EVT            = 20,      /*!< When congest happen, the event comes */
    /* following is extra event */
    ESP_GATTS_RESPONSE_EVT           = 21,      /*!< When gatt send response complete, the event comes */
    ESP_GATTS_CREAT_ATTR_TAB_EVT     = 22,
    ESP_GATTS_SET_ATTR_VAL_EVT       = 23,
} esp_gatts_cb_event_t;
```