
#include "tiny_uart.h"



#define RX_SIZE  256

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)
#define TF_TAG "TintFrame"

static const char *TAG = "serial";

TinyFrame my_tf;
TinyFrame *demo_tf = &my_tf;

bool do_corrupt = false;


// helper func for testing
void dumpFrame(const uint8_t *buff, size_t len)
{
    size_t i;
    ESP_LOGI("tf_dump", "dumpFrame:\r\n");
    for (i = 0; i < len; i++) {
        ESP_LOGI("tf_dump", "%3u %02X", buff[i], buff[i]);
        if (buff[i] >= 0x20 && buff[i] < 127) {
            ESP_LOGI("tf_dump", " %c", buff[i]);
        } else {
            ESP_LOGI("tf_dump", " .");
        }
        ESP_LOGI("tf_dump", "\r\n");
    }
    ESP_LOGI("tf_dump", "--- end of frame ---\r\n\r\n");
}

void dumpFrameInfo(TF_Msg *msg)
{
    ESP_LOGI("tf_dump", "dumpFrameInfo:\r\n");
    ESP_LOGI("tf_dump", "Frame info\r\n"
             "  type: %02Xh\r\n"
             "  data: \"%.*s\"\r\n"
             "   len: %u\r\n"
             "    id: %Xh\r\n\r\n",
             msg->type, msg->len, msg->data, msg->len, msg->frame_id);
    ESP_LOG_BUFFER_HEX("raw data:",msg->data, msg->len);
}

void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len)
{
    // ESP_LOGI(TF_TAG, "--------------------\r\n");
    // ESP_LOGI(TF_TAG, "\033[32mTF_WriteImpl - sending frame:\033[0m\r\n");

    // ESP_LOGI("TF_WriteImpl", "len=%d", len);
    // // 打印发送的数据
    // ESP_LOG_BUFFER_CHAR("TF_WriteImpl:", buff, len);
    // uint8_t *xbuff = (uint8_t *)buff;
    // //校验和
    // if (do_corrupt) {
    //     ESP_LOGI("TF_WriteImpl", "(corrupting to test checksum checking...)\r\n");
    //     xbuff[8]++;
    // }
    uart_write_bytes(UART_NUM_1, (const char * )buff, len);
    // dumpFrame(xbuff, len);
    // 将它发回，就好像我们收到了它
    // TF_Accept(tf, xbuff, len);
}

/** 一个监听器函数的示例 */
TF_Result myListener(TinyFrame *tf, TF_Msg *msg)
{
    dumpFrameInfo(msg);
    return TF_STAY;
}

// TF_Result testIdListener(TinyFrame *tf, TF_Msg *msg)
// {
//     ESP_LOGI(TF_TAG, "OK - ID Listener triggered for msg!\r\n");
//     dumpFrameInfo(msg);
//     return TF_CLOSE;
// }



int sendData(const char *logName, const char *data)
{
    const int txBytes = uart_write_bytes(UART_NUM_1, data, 256);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

// static void tx_task()
// {
//     static const char *TX_TASK_TAG = "TX_TASK";
//     esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
//     while (1) {
//         char buf[256] = "{m:0}";
//         uart_write_bytes(UART_NUM_1, buf, 256);
//         // ESP_LOGI(logName, "Wrote %d bytes", txBytes);
//         vTaskDelay(2000 / portTICK_PERIOD_MS);
//         strcpy(buf, "{m:1}");
//         txBytes = uart_write_bytes(UART_NUM_1, buf, 256);
//     }
// }
static void rx_task()
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t data[RX_SIZE + 1];
    while (1) {
        memset(data, 0, RX_SIZE + 1);
        int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_SIZE, 10 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            TF_Accept(demo_tf, (const uint8_t *)&data, rxBytes);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);

    }
}


void tiny_init(void )
{
    ESP_LOGI(TAG,"tiny_init");
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_SIZE * 2, 0, 0, NULL, 0);

    xTaskCreate(rx_task, "uart_rx_task", 2048 * 2, NULL, configMAX_PRIORITIES, NULL);
    // xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
    
    //设置TinyFrame库
    TF_InitStatic(&my_tf, TF_MASTER);
    // 添加lintener
    TF_AddGenericListener(demo_tf, myListener);

}