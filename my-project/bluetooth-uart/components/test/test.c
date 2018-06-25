#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"

#include "TinyFrame.h"
#include "service_table.h"

void cmd_hc05_get_test()
{
    for (size_t i = 0; i < 20; i++) {
        TF_Msg msg;
        msg.type = 10;
        msg.data = NULL;
        msg.len = 0;
        TF_Send(demo_tf, &msg);
        vTaskDelay(2000 / portTICK_PERIOD_MS);

    }
}