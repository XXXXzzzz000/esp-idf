service table 类型
struct esp_gatts_attr_db_t {
    esp_attr_control_t      attr_control;                   /*!< The attribute control type */
    esp_attr_desc_t         att_desc;                       /*!< The attribute type */
} ;

struct esp_attr_control_t {
#define ESP_GATT_RSP_BY_APP             0
#define ESP_GATT_AUTO_RSP               1
    /**
     * @brief if auto_rsp set to ESP_GATT_RSP_BY_APP, means the response of Write/Read operation will by replied by application.
              if auto_rsp set to ESP_GATT_AUTO_RSP, means the response of Write/Read operation will be replied by GATT stack automatically.
     */
    uint8_t auto_rsp;
} ;


struct esp_attr_desc_t {
    uint16_t uuid_length;              /*!< UUID 长度 */
    uint8_t  *uuid_p;                  /*!< UUID 值 */
    uint16_t perm;                     /*!< 属性权限 */
    uint16_t max_length;               /*!< element 最大长度*/
    uint16_t length;                   /*!< element 当前长度*/
    uint8_t  *value;                   /*!< Element 值数组*/
} ;
uuid除了标识设备,还起到标识一类设备的作用


uuid

/**@{
 * All "ESP_GATT_UUID_xxx" is attribute types
 */
#define ESP_GATT_UUID_IMMEDIATE_ALERT_SVC           0x1802          /*  Immediate alert Service*/
#define ESP_GATT_UUID_LINK_LOSS_SVC                 0x1803          /*  Link Loss Service*/
#define ESP_GATT_UUID_TX_POWER_SVC                  0x1804          /*  TX Power Service*/
#define ESP_GATT_UUID_CURRENT_TIME_SVC              0x1805          /*  Current Time Service Service*/
#define ESP_GATT_UUID_REF_TIME_UPDATE_SVC           0x1806          /*  Reference Time Update Service*/
#define ESP_GATT_UUID_NEXT_DST_CHANGE_SVC           0x1807          /*  Next DST Change Service*/
#define ESP_GATT_UUID_GLUCOSE_SVC                   0x1808          /*  Glucose Service*/
#define ESP_GATT_UUID_HEALTH_THERMOM_SVC            0x1809          /*  Health Thermometer Service*/
#define ESP_GATT_UUID_DEVICE_INFO_SVC               0x180A          /*  Device Information Service*/
#define ESP_GATT_UUID_HEART_RATE_SVC                0x180D          /*  Heart Rate Service*/
#define ESP_GATT_UUID_PHONE_ALERT_STATUS_SVC        0x180E          /* Phone Alert Status Service*/
#define ESP_GATT_UUID_BATTERY_SERVICE_SVC           0x180F          /* Battery Service*/
#define ESP_GATT_UUID_BLOOD_PRESSURE_SVC            0x1810          /* Blood Pressure Service*/
#define ESP_GATT_UUID_ALERT_NTF_SVC                 0x1811          /* Alert Notification Service*/
#define ESP_GATT_UUID_HID_SVC                       0x1812          /* HID Service*/
#define ESP_GATT_UUID_SCAN_PARAMETERS_SVC           0x1813          /* Scan Parameters Service*/
#define ESP_GATT_UUID_RUNNING_SPEED_CADENCE_SVC     0x1814          /* Running Speed and Cadence Service*/
#define ESP_GATT_UUID_CYCLING_SPEED_CADENCE_SVC     0x1816          /* Cycling Speed and Cadence Service*/
#define ESP_GATT_UUID_CYCLING_POWER_SVC             0x1818          /* Cycling Power Service*/
#define ESP_GATT_UUID_LOCATION_AND_NAVIGATION_SVC   0x1819          /* Location and Navigation Service*/
#define ESP_GATT_UUID_USER_DATA_SVC                 0x181C          /* User Data Service*/
#define ESP_GATT_UUID_WEIGHT_SCALE_SVC              0x181D          /* Weight Scale Service*/

#define ESP_GATT_UUID_PRI_SERVICE                   0x2800
#define ESP_GATT_UUID_SEC_SERVICE                   0x2801
#define ESP_GATT_UUID_INCLUDE_SERVICE               0x2802
#define ESP_GATT_UUID_CHAR_DECLARE                  0x2803          /*  Characteristic Declaration*/

#define ESP_GATT_UUID_CHAR_EXT_PROP                 0x2900          /*  Characteristic Extended Properties */
#define ESP_GATT_UUID_CHAR_DESCRIPTION              0x2901          /*  Characteristic User Description*/
#define ESP_GATT_UUID_CHAR_CLIENT_CONFIG            0x2902          /*  Client Characteristic Configuration */
#define ESP_GATT_UUID_CHAR_SRVR_CONFIG              0x2903          /*  Server Characteristic Configuration */
#define ESP_GATT_UUID_CHAR_PRESENT_FORMAT           0x2904          /*  Characteristic Presentation Format*/
#define ESP_GATT_UUID_CHAR_AGG_FORMAT               0x2905          /*  Characteristic Aggregate Format*/
#define ESP_GATT_UUID_CHAR_VALID_RANGE              0x2906          /*  Characteristic Valid Range */
#define ESP_GATT_UUID_EXT_RPT_REF_DESCR             0x2907
#define ESP_GATT_UUID_RPT_REF_DESCR                 0x2908

/* GAP Profile Attributes */
#define ESP_GATT_UUID_GAP_DEVICE_NAME               0x2A00
#define ESP_GATT_UUID_GAP_ICON                      0x2A01
#define ESP_GATT_UUID_GAP_PREF_CONN_PARAM           0x2A04
#define ESP_GATT_UUID_GAP_CENTRAL_ADDR_RESOL        0x2AA6

/* Attribute Profile Attribute UUID */
#define ESP_GATT_UUID_GATT_SRV_CHGD                 0x2A05

/* Link ESP_Loss Service */
#define ESP_GATT_UUID_ALERT_LEVEL                   0x2A06          /* Alert Level */
#define ESP_GATT_UUID_TX_POWER_LEVEL                0x2A07          /* TX power level */

/* Current Time Service */
#define ESP_GATT_UUID_CURRENT_TIME                  0x2A2B          /* Current Time */
#define ESP_GATT_UUID_LOCAL_TIME_INFO               0x2A0F          /* Local time info */
#define ESP_GATT_UUID_REF_TIME_INFO                 0x2A14          /* reference time information */

/* Network availability Profile */
#define ESP_GATT_UUID_NW_STATUS                     0x2A18          /* network availability status */
#define ESP_GATT_UUID_NW_TRIGGER                    0x2A1A          /* Network availability trigger */

/* Phone alert */
#define ESP_GATT_UUID_ALERT_STATUS                  0x2A3F          /* alert status */
#define ESP_GATT_UUID_RINGER_CP                     0x2A40          /* ringer control point */
#define ESP_GATT_UUID_RINGER_SETTING                0x2A41          /* ringer setting */

/* Glucose Service */
#define ESP_GATT_UUID_GM_MEASUREMENT                0x2A18
#define ESP_GATT_UUID_GM_CONTEXT                    0x2A34
#define ESP_GATT_UUID_GM_CONTROL_POINT              0x2A52
#define ESP_GATT_UUID_GM_FEATURE                    0x2A51

/* device information characteristic */
#define ESP_GATT_UUID_SYSTEM_ID                     0x2A23
#define ESP_GATT_UUID_MODEL_NUMBER_STR              0x2A24
#define ESP_GATT_UUID_SERIAL_NUMBER_STR             0x2A25
#define ESP_GATT_UUID_FW_VERSION_STR                0x2A26
#define ESP_GATT_UUID_HW_VERSION_STR                0x2A27
#define ESP_GATT_UUID_SW_VERSION_STR                0x2A28
#define ESP_GATT_UUID_MANU_NAME                     0x2A29
#define ESP_GATT_UUID_IEEE_DATA                     0x2A2A
#define ESP_GATT_UUID_PNP_ID                        0x2A50

/* HID characteristics */
#define ESP_GATT_UUID_HID_INFORMATION               0x2A4A
#define ESP_GATT_UUID_HID_REPORT_MAP                0x2A4B
#define ESP_GATT_UUID_HID_CONTROL_POINT             0x2A4C
#define ESP_GATT_UUID_HID_REPORT                    0x2A4D
#define ESP_GATT_UUID_HID_PROTO_MODE                0x2A4E
#define ESP_GATT_UUID_HID_BT_KB_INPUT               0x2A22
#define ESP_GATT_UUID_HID_BT_KB_OUTPUT              0x2A32
#define ESP_GATT_UUID_HID_BT_MOUSE_INPUT            0x2A33

 /// Heart Rate Measurement
#define    ESP_GATT_HEART_RATE_MEAS                 0x2A37
/// Body Sensor Location
#define    ESP_GATT_BODY_SENSOR_LOCATION            0x2A38
/// Heart Rate Control Point
#define    ESP_GATT_HEART_RATE_CNTL_POINT           0x2A39

/* Battery Service characteristics */
#define ESP_GATT_UUID_BATTERY_LEVEL                 0x2A19

/* Sensor Service */
#define ESP_GATT_UUID_SC_CONTROL_POINT              0x2A55
#define ESP_GATT_UUID_SENSOR_LOCATION               0x2A5D

/* Runners speed and cadence service */
#define ESP_GATT_UUID_RSC_MEASUREMENT               0x2A53
#define ESP_GATT_UUID_RSC_FEATURE                   0x2A54

/* Cycling speed and cadence service */
#define ESP_GATT_UUID_CSC_MEASUREMENT               0x2A5B
#define ESP_GATT_UUID_CSC_FEATURE                   0x2A5C

/* Scan ESP_Parameter characteristics */
#define ESP_GATT_UUID_SCAN_INT_WINDOW               0x2A4F
#define ESP_GATT_UUID_SCAN_REFRESH                  0x2A31
/**
 * @}
 */
