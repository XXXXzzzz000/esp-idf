#pragma once

typedef enum air_sensor_t
{
    MQ_135 = 1,
    MQ_136 = 2
} air_sensor_t;
// typedef struct
// {
//     CALIBRATION,COLLECT
// }air_sensor_state_t;

extern xTaskHandle xAirAdcHandle;
//adc获取任务
void air_adc_get_task(void *parm);
//注册adc命令
void register_adc();
