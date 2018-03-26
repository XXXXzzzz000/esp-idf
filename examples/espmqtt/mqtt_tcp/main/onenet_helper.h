#pragma once




#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "cJSON.h"
#include "mqtt_client.h"

#define MQTTSAVEDPTOPICNAME "$dp"
#define PAYLOADWITHTIME(T) ((T & 0xFF) | 0x80)
#define PAYLOADWITHOUTTIME(T) (T & 0x7F)
/** MQTT错误码 */
enum MqttError {
    MQTTERR_NOERROR                  = 0,  /**< 成功，无错误*/
    MQTTERR_OUTOFMEMORY              = -1, /**< 内存不足 */
    MQTTERR_ENDOFFILE                = -2, /**< 读数据失败，已到文件结尾*/
    MQTTERR_IO                       = -3, /**< I/O错误 */
    MQTTERR_ILLEGAL_PKT              = -4, /**< 非法的数据包 */
    MQTTERR_ILLEGAL_CHARACTER        = -5, /**< 非法的字符 */
    MQTTERR_NOT_UTF8                 = -6, /**< 字符编码不是UTF-8 */
    MQTTERR_INVALID_PARAMETER        = -7, /**< 参数错误 */
    MQTTERR_PKT_TOO_LARGE            = -8, /**< 数据包过大 */
    MQTTERR_BUF_OVERFLOW             = -9, /**< 缓冲区溢出 */
    MQTTERR_EMPTY_CALLBACK           = -10,/**< 回调函数为空 */
    MQTTERR_INTERNAL                 = -11,/**< 系统内部错误 */
    MQTTERR_NOT_IN_SUBOBJECT         = -12,/**< 调用Mqtt_AppendDPFinishObject，但没有匹配的Mqtt_AppendDPStartObject */
    MQTTERR_INCOMPLETE_SUBOBJECT     = -13,/**< 调用Mqtt_PackDataPointFinish时，包含的子数据结构不完整 */
    MQTTERR_FAILED_SEND_RESPONSE     = -14 /**< 处理publish系列消息后，发送响应包失败 */
};
/* 上报数据点，消息支持的格式类型 */
enum MqttSaveDataType
{
    kTypeFullJson = 0x01,
    kTypeBin = 0x02,
    kTypeSimpleJsonWithoutTime = 0x03,
    kTypeSimpleJsonWithTime = 0x04,
    kTypeString = 0x05,
    kTypeStringWithTime = 0x06,
    kTypeFloat = 0x07
};


/**
 * 封装二进制类型数据点（OneNet扩展）,支持数据类型type=2
 * @param buf 存储数据包的缓冲区对象
 * @param pkt_id 数据包ID，非0
 * @param dsid 数据流ID
 * @param desc 数据点的描述信息
 * @param time 格林威治时间，从1970-01-01T00:00:00.000开始的毫秒时间戳，
 *             为0或负数时，系统取默认时间
 * @param bin 二进制数据的起始地址
 * @param size 二进制数据的字节数
 * @param qos QoS等级
 * @param retain 非0时，服务器将该publish消息保存到topic下，并替换已有的publish消息
 * @param own 非0时，拷贝bin到缓冲区
 * @return 成功返回MQTTERR_NOERROR
 * @remark 当own为0时，bin必须在buf被销毁或重置前保持有效
 */
int Mqtt_PackDataPointByBinary(esp_mqtt_client_handle_t client, uint16_t pkt_id, const char *dsid,
                               const char *desc, int64_t ts, const char *bin, uint32_t size,
                               int qos, int retain, int own);

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
                               int qos, int retain, int own);


