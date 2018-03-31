
#include "onenet_helper.h"

#define MQTT_HELPER_TEST

#if defined(MQTT_HELPER_TEST)
static const  char *TAG = "ONENET_HELPER";
#endif

int Mqtt_PackDataPointByBinary(esp_mqtt_client_handle_t client, uint16_t pkt_id, const char *dsid,
                               const char *desc, int64_t ts, const char *bin, uint32_t size,
                               int qos, int retain, int own)
{
    char dp_type = kTypeBin & 0xFF;
    uint32_t ds_info_len = 0;
    char *ds_info_str = NULL;
    cJSON *ds_info = cJSON_CreateObject();
    uint32_t bin_offset = 0;
    char time_buff[20];
    int ret = MQTTERR_NOERROR;

    cJSON_AddStringToObject(ds_info, "ds_id", dsid);
    if (ts <= 0) {
        time(&ts);
    }
    strftime(time_buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&ts));

    cJSON_AddStringToObject(ds_info, "at", time_buff);
    cJSON_AddStringToObject(ds_info, "desc", desc);
    ds_info_str = cJSON_Print(ds_info);
    ds_info_len = strlen(ds_info_str);
#ifdef _debug
    printf("save data type 2(binary),length:%d,\njson:%s\n", ds_info_len, ds_info_str);
#endif

    //payload的总长度
    int32_t payload_size = 1 + 2 + ds_info_len + 4 + size;
    char payload[payload_size];
    //填充payload
    payload[0] = dp_type;
    payload[1] = (ds_info_len >> 8) & 0xFF;
    payload[2] = ds_info_len & 0xFF;
    memcpy(payload + 3, ds_info_str, ds_info_len);
    bin_offset = 1 + 2 + ds_info_len;
    payload[bin_offset] = (size >> 24) & 0xFF;
    payload[bin_offset + 1] = (size >> 16) & 0xFF;
    payload[bin_offset + 2] = (size >> 8) & 0xFF;
    payload[bin_offset + 3] = size & 0xFF;
    memcpy(payload + bin_offset + 4,
           bin, size);
#if defined(MQTT_HELPER_TEST)
    ret = 0;
    ESP_LOG_BUFFER_HEX(TAG, payload, payload_size);
#else
    ret = esp_mqtt_client_publish(client, MQTTSAVEDPTOPICNAME, payload, payload_size, qos, retain);
#endif
    free(ds_info_str);
    return ret;
}

/**
 * 封装字符串类型数据点（OneNet扩展）,支持数据类型type=1,3,4,5,6,7
 * @param buf 存储数据包的缓冲区对象
 * @param pkt_id 数据包ID，非0
 * @param time 格林威治时间，从1970-01-01T00:00:00.000开始的毫秒时间戳，
 *             为0或负数时，系统取默认时间
 * @param type 上传数据点的类型
 * @param str 数据的起始地址
 * @param size 数据的字节数
 * @param qos QoS等级
 * @param retain 非0时，服务器将该publish消息保存到topic下，并替换已有的publish消息
 * @param own 非0时，拷贝bin到缓冲区
 * @return 成功返回MQTTERR_NOERROR
 * @remark 当own为0时，bin必须在buf被销毁或重置前保持有效
 */

int Mqtt_PackDataPointByString(esp_mqtt_client_handle_t client, uint16_t pkt_id, int64_t ts,
                               int32_t type, const char *str, uint32_t size,
                               int qos, int retain, int own)
{
    char *payload = NULL;
    int32_t payload_size = 0;
    struct tm *t = NULL;
    int64_t now;
    time_t tt;
    int32_t offset = 0;
    int ret = 0;

    if (kTypeFullJson == type ||
            kTypeBin == type ||
            kTypeSimpleJsonWithoutTime == type ||
            kTypeSimpleJsonWithTime == type ||
            kTypeString == type) {
        //payload 总长
        payload_size = 1 + 2 + size;
        payload = (char *)malloc(payload_size);
        if (NULL == payload) {
            return MQTTERR_OUTOFMEMORY;
        }

        //填充payload
        payload[0] = type & 0xFF;
        payload[1] = (size >> 8) & 0xFF;
        payload[2] = size & 0xFF;
        memcpy(payload + 3, str, size);

        // ret = Mqtt_PackPublishPkt(buf, pkt_id, MQTTSAVEDPTOPICNAME, payload, payload_size, qos, retain, own);
#if defined(MQTT_HELPER_TEST)
        ret = 0;
        ESP_LOG_BUFFER_HEX(TAG, payload, payload_size);
#else
        ret = esp_mqtt_client_publish(client, MQTTSAVEDPTOPICNAME, payload, payload_size, qos, retain);
#endif
    } else if (kTypeStringWithTime == (type & 0x7F) ||
               kTypeFloat == (type & 0x7F)) {
        if (kTypeFloat == (type & 0x7F)) {
            payload_size = 1 + size;
        } else {
            payload_size = 1 + 2 + size;
        }
        if (type & 0x80) {
            payload_size += 6;
        }
        payload = (char *)malloc(payload_size);
        if (NULL == payload) {
            return MQTTERR_OUTOFMEMORY;
        }

        //填充payload
        payload[0] = type & 0xFF;
        if (ts <= 0) {
            time(&now);
        }
        tt = (time_t)now;
        t = gmtime(&tt);
        if (!t) {
            free(payload);
            return MQTTERR_INTERNAL;
        }
        if (type & 0x80) {
            payload[1] = (t->tm_year + 1900) % 100;
            payload[2] = (t->tm_mon + 1) & 0xFF;
            payload[3] = (t->tm_mday) & 0xFF;
            payload[4] = (t->tm_hour) & 0xFF;
            payload[5] = (t->tm_min) & 0xFF;
            payload[6] = (t->tm_sec) & 0xFF;
            offset = 6;
        } else {
            offset = 0;
        }
        if (kTypeStringWithTime == (type & 0x7F)) {
            payload[offset + 1] = (size >> 8) & 0xFF;
            payload[offset + 2] = size & 0xFF;
            offset += 2;
        }
        memcpy(payload + offset + 1, str, size);
        // ret = Mqtt_PackPublishPkt(buf, pkt_id, MQTTSAVEDPTOPICNAME, payload, payload_size, qos, retain, own);
#if defined(MQTT_HELPER_TEST)
        ret = 0;
        ESP_LOG_BUFFER_HEX(TAG, payload, payload_size);
#else
        ret = esp_mqtt_client_publish(client, MQTTSAVEDPTOPICNAME, payload, payload_size, qos, retain);
#endif
    } else {
        return MQTTERR_INVALID_PARAMETER;
    }

    free(payload);
    return ret;
}


