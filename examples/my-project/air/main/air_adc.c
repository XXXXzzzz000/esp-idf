
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "air_adc.h"
#include "esp_log.h"
#include "air_storge.h"
static const char *TAG = "AIR_ADC";

#define DEFAULT_VREF 1100 //使用adc2_vref_to_gpio（）来获得更好的估计
#define NO_OF_SAMPLES 64  //多重采样

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6; //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;
//检查adc的参考电压值
static void check_efuse()
{
    //检查TP是否被烧入eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
    {
        ESP_LOGI(TAG, "eFuse Two Point: Supported\n");
    }
    else
    {
        ESP_LOGI(TAG, "eFuse Two Point: NOT supported\n");
    }

    //检查Vref是否被烧入eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
    {
        ESP_LOGI(TAG, "eFuse Vref: Supported\n");
    }
    else
    {
        ESP_LOGI(TAG, "eFuse Vref: NOT supported\n");
    }
}
//打印adc检查值
static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        ESP_LOGI(TAG, "Characterized using Two Point Value\n");
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        ESP_LOGI(TAG, "Characterized using eFuse Vref\n");
    }
    else
    {
        ESP_LOGI(TAG, "Characterized using Default Vref\n");
    }
}
//初始化adc
static void air_adc_init()
{

    //检查两点或Vref是否被烧入eFuse
    check_efuse();

    //配置ADC
    if (unit == ADC_UNIT_1)
    {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    }
    else
    {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    // ADC特性描述
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    //adc1 衰减ADC_ATTEN_DB_11 adc位宽12 参考电压DEFAULT_VREF adc特性指针
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}
//读取adc值
static uint32_t air_adc_get_voltage()
{
    uint32_t adc_reading = 0;
    //多重采样,求其均值
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        if (unit == ADC_UNIT_1)
        {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }
        else
        {
            int raw;
            adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
            adc_reading += raw;
        }
    }
    adc_reading /= NO_OF_SAMPLES;
    //转换 adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    ESP_LOGI(TAG, "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
    //将读取的示数写入到文件内
    cmd_storge_write(adc_reading, voltage);
    return voltage;
}

//获取adc值的任务
void air_adc_get_task(void *parm)
{
    air_adc_init();
    while (1)
    {
        air_adc_get_voltage();
        vTaskDelay(300);
    }
}


