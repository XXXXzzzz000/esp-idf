


```c
typedef struct
{
    esp_attr_control_t      attr_control;                   /*!< 属性控制类型(手动回复还是自动回复) */
    esp_attr_desc_t         att_desc;                       /*!< 属性类型 */
} esp_gatts_attr_db_t;
```
`esp_attr_control_t`和`esp_attr_desc_t`
```c
/**
 * @brief attribute auto response flag
 */
typedef struct
{
#define ESP_GATT_RSP_BY_APP             0
#define ESP_GATT_AUTO_RSP               1
    /**
     * @brief ESP_GATT_RSP_BY_APP, 手动回复.
              ESP_GATT_AUTO_RSP, 自动回复.
     */
    uint8_t auto_rsp;
} esp_attr_control_t;

//具体请查阅 ESP蓝牙架构的3.2.3
/**
 * @brief 属性描述 (used to create database)
 */
 typedef struct
 {   
     uint16_t uuid_length;              /*!< UUID 长度 */
     uint8_t  *uuid_p;                  /*!< UUID 值 */
     uint16_t perm;                     /*!< 属性许可 */
     uint16_t max_length;               /*!< 当前属性值的最⼤⻓度*/
     uint16_t length;                   /*!< 当前属性的当前实际⻓度*/
     uint8_t  *value;                   /*!< 当前属性的属性值初始化值*/
 } esp_attr_desc_t;
```