
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
#include "esp_timer.h"
static const char *TAG = "AIR_ADC";

#define DEFAULT_VREF 1100 //使用adc2_vref_to_gpio（）来获得更好的估计
#define NO_OF_SAMPLES 64  //多重采样
#define TASK_PERIOD 3000 //任务周期30s
xTaskHandle xAirAdcHandle = NULL;
static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;  //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_channel_t channel2 = ADC_CHANNEL_5; //GPIO32 具体查看ESP32技术参考手册
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;
static uint64_t adc_runtime = 0;
#if 0
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
#endif

//初始化adc
static void air_adc_init()
{

    //检查两点或Vref是否被烧入eFuse
    // check_efuse();

//配置ADC
    //位宽
    adc1_config_width(ADC_WIDTH_BIT_12);
    //通道衰减
    adc1_config_channel_atten(channel, atten);
    adc1_config_channel_atten(channel2, atten);

// ADC特性描述
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    //adc1 衰减ADC_ATTEN_DB_11 adc位宽12 参考电压DEFAULT_VREF adc特性指针
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    // print_char_val_type(val_type);
}
//获取自系统运行以来的秒数
uint64_t get_time_s()
{
    return esp_timer_get_time() / 1000000;
}
//获取空气传感器的
//读取adc值
static uint32_t air_adc_get_voltage()
{
    uint32_t adc_reading = 0;
    uint32_t adc_reading2 = 0;
//多重采样两个通道,求其均值
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        adc_reading += adc1_get_raw((adc1_channel_t)channel);
        adc_reading2 += adc1_get_raw((adc1_channel_t)channel2);
    }
    adc_reading /= NO_OF_SAMPLES;
    adc_reading2 /= NO_OF_SAMPLES;
//转换 adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    uint32_t voltage2 = esp_adc_cal_raw_to_voltage(adc_reading2, adc_chars);
    ESP_LOGI(TAG, "Raw: %d\tVoltage: %dmV\tTime:%lld\n", adc_reading, voltage, adc_runtime);
    ESP_LOGI(TAG, "Raw2: %d\tVoltage2: %dmV\tTime:%lld\n", adc_reading2, voltage2, adc_runtime);

//将读取的示数写入到文件内
    //实际时间=adc_runtime*30s
    cmd_storge_write(1, adc_reading, voltage, adc_runtime);
    cmd_storge_write(2, adc_reading2, voltage2, adc_runtime);
    return voltage;
}

//获取adc值的任务 30s运行一次
void air_adc_get_task(void *parm)
{
    static portTickType xLastWakeTime;
    const portTickType xFrequency = TASK_PERIOD;

    // 使用当前时间初始化变量xLastWakeTime
    xLastWakeTime = xTaskGetTickCount();
    air_adc_init();
    while (1)
    {
        //等待下一个周期
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        // 需要周期性执行代码放在这里
        air_adc_get_voltage();
        //adc运行时间增加
        adc_runtime++;
    }
}

/* console 命令 */
static void cmd_adc_suspend(int argc, char **argv)
{
    if (xAirAdcHandle == NULL)
    {
        ESP_LOGE(TAG, "air task handler is null");
        return;
    }
    vTaskSuspend(xAirAdcHandle);
}
static void cmd_adc_resume(int argc, char **argv)
{
    if (xAirAdcHandle == NULL)
    {
        ESP_LOGE(TAG, "air task handler is null");
        return;
    }
    xTaskToResume(xAirAdcHandle);
}

/** commonds register*/
void register_adc()
{
    esp_console_cmd_t cmd_table[] =
        {

            {
                .command = "adc_resume",
                .help = "",
                .hint = NULL,
                .func = &cmd_adc_resume,
            },
            {
                .command = "adc_suspend",
                .help = "",
                .hint = NULL,
                .func = &cmd_adc_suspend,
            },
            {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
            }

        };

    for (esp_console_cmd_t *p = cmd_table; p->command != NULL; p++)
    {
        ESP_ERROR_CHECK(esp_console_cmd_register(p));
    }
}
