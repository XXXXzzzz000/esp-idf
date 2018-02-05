# attribute和characteristic

```c
typedef enum {
    ESP_GATTS_REG_EVT                = 0,
    ESP_GATTS_READ_EVT               = 1,
    ESP_GATTS_WRITE_EVT              = 2,
    ESP_GATTS_EXEC_WRITE_EVT         = 3,
    ESP_GATTS_MTU_EVT                = 4,
    ESP_GATTS_CONF_EVT               = 5,
    ESP_GATTS_UNREG_EVT              = 6,
    ESP_GATTS_CREATE_EVT             = 7,
    ESP_GATTS_ADD_INCL_SRVC_EVT      = 8,
    ESP_GATTS_ADD_CHAR_EVT           = 9,
    ESP_GATTS_ADD_CHAR_DESCR_EVT     = 10,
    ESP_GATTS_DELETE_EVT             = 11,
    ESP_GATTS_START_EVT              = 12,
    ESP_GATTS_STOP_EVT               = 13,
    ESP_GATTS_CONNECT_EVT            = 14,
    ESP_GATTS_DISCONNECT_EVT         = 15,
    ESP_GATTS_OPEN_EVT               = 16,
    ESP_GATTS_CANCEL_OPEN_EVT        = 17,
    ESP_GATTS_CLOSE_EVT              = 18,
    ESP_GATTS_LISTEN_EVT             = 19,
    ESP_GATTS_CONGEST_EVT            = 20,
    /* following is extra event */
    ESP_GATTS_RESPONSE_EVT           = 21,
    ESP_GATTS_CREAT_ATTR_TAB_EVT     = 22,
    ESP_GATTS_SET_ATTR_VAL_EVT       = 23,
} esp_gatts_cb_event_t;
```