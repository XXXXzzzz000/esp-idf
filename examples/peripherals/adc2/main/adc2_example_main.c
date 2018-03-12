/* ADC2 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_system.h"
#include "esp_adc_cal.h"

#define DAC_EXAMPLE_CHANNEL     CONFIG_DAC_EXAMPLE_CHANNEL
#define ADC2_EXAMPLE_CHANNEL    CONFIG_ADC2_EXAMPLE_CHANNEL

void app_main(void)
{
    uint8_t output_data=0;
    int     read_raw;
    esp_err_t r;

    gpio_num_t adc_gpio_num, dac_gpio_num;

    assert( adc2_pad_get_io_num( ADC2_EXAMPLE_CHANNEL, &adc_gpio_num ) == ESP_OK );
    assert( dac_pad_get_io_num( DAC_EXAMPLE_CHANNEL, &dac_gpio_num ) == ESP_OK );

    printf("ADC channel %d @ GPIO %d, DAC channel %d @ GPIO %d.\n", ADC2_EXAMPLE_CHANNEL, adc_gpio_num,
                DAC_EXAMPLE_CHANNEL, dac_gpio_num );

    dac_output_enable( DAC_EXAMPLE_CHANNEL );

    //请确保在使用adc2之前执行init。
    printf("adc2_init...\n");
    adc2_config_channel_atten( ADC2_EXAMPLE_CHANNEL, ADC_ATTEN_0db );

    vTaskDelay(2 * portTICK_PERIOD_MS);

    printf("start conversion.\n");
    while(1) {
        dac_output_voltage( DAC_EXAMPLE_CHANNEL, output_data++ );
        r = adc2_get_raw( ADC2_EXAMPLE_CHANNEL, ADC_WIDTH_12Bit, &read_raw);
        if ( r == ESP_OK ) {
            printf("%d: %d\n", output_data, read_raw );
        } else if ( r == ESP_ERR_INVALID_STATE ) {
            printf("ADC2 not initialized yet.\n");
        } else if ( r == ESP_ERR_TIMEOUT ) {
            //在这个例子中不会发生这种情况。 但是，如果WiFi正在使用中，则可能会返回此类错误代码。
            printf("ADC2 is in use by Wi-Fi.\n");
        }
        vTaskDelay( 2 * portTICK_PERIOD_MS );
    }
}
