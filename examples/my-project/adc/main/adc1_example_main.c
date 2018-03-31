/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"


#define USE_MQ_136
// #define USE_MQ_135

#define DEFAULT_VREF    1100        //使用adc2_vref_to_gpio（）来获得更好的估计
#define NO_OF_SAMPLES   128          //多重采样
static const char *TAG = "ADC_TEST";
static esp_adc_cal_characteristics_t *adc_chars;
// static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_channel_t channel = ADC_CHANNEL_5;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;
static const adc_channel_t channel2 = ADC_CHANNEL_5; //MQ136 GPIO32
static void check_efuse()
{
    //检查TP是否被烧入eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //检查Vref是否被烧入eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void app_main()
{
    //检查两点或Vref是否被烧入eFuse
    check_efuse();

    //配置ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, atten);
    adc1_config_channel_atten(channel2, atten);

    // ADC特性描述
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    //adc1 衰减ADC_ATTEN_DB_11 adc位宽12 参考电压DEFAULT_VREF adc特性指针
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

#if 0
    //连续采样 ADC1
    while (1) {
        uint32_t adc_reading = 0;
        uint32_t adc_reading2 = 0;

        //多重采样,求其均值
        for (int i = 0; i < NO_OF_SAMPLES; i++) {

            adc_reading += adc1_get_raw((adc1_channel_t)channel);
            adc_reading2 += adc1_get_raw((adc1_channel_t)channel2);


        }
        adc_reading /= NO_OF_SAMPLES;
        adc_reading2 /= NO_OF_SAMPLES;
        //转换 adc_reading to voltage in mV
        uint32_t voltage = 2 * esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        uint32_t voltage2 = 2 * esp_adc_cal_raw_to_voltage(adc_reading2, adc_chars);

        ESP_LOGI(TAG, "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        ESP_LOGI(TAG, "Raw2: %d\tVoltage2: %dmV\n", adc_reading2, voltage2);

        float mq135_r = (float)((5.0  / (voltage / 1000.0)) - 1.0);
        float mq136_r = (float)((5.0 / (voltage2 / 1000.0)) - 1.0);

        ESP_LOGI(TAG, "mq135_r: %f k\n", mq135_r);
        ESP_LOGI(TAG, "mq136_r: %f k\n", mq136_r);

        ESP_LOGI(TAG, "\n\n\n\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

#endif

    //连续采样 ADC1
    while (1) {
        uint32_t adc_reading = 0;
        uint32_t adc_reading2 = 0;
#if defined(USE_MQ_135)
        //多重采样,求其均值
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }
        adc_reading /= NO_OF_SAMPLES;
        uint32_t voltage = 2 * esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        ESP_LOGI(TAG, "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        float mq135_r = (float)((5.0  / (voltage / 1000.0)) - 1.0);
        ESP_LOGI(TAG, "mq135_r: %f k\n", mq135_r);
#endif
#if defined(USE_MQ_136)

        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading2 += adc1_get_raw((adc1_channel_t)channel2);
        }

        adc_reading2 /= NO_OF_SAMPLES;
        //转换 adc_reading to voltage in mV
        uint32_t voltage2 = 2 * esp_adc_cal_raw_to_voltage(adc_reading2, adc_chars);

        ESP_LOGI(TAG, "Raw2: %d\tVoltage2: %dmV\n", adc_reading2, voltage2);

        float mq136_r = (float)((5.0 / (voltage2 / 1000.0)) - 1.0);

        ESP_LOGI(TAG, "mq136_r: %f k\n", mq136_r);
#endif
        ESP_LOGI(TAG, "\n\n\n\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


