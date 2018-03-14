#pragma once


typedef struct
{
    MQ_135 = 1, MQ_136 = 2
} air_sensor_t;
// typedef struct
// {
//     CALIBRATION,COLLECT
// }air_sensor_state_t;
extern xTaskHandle xAirAdcHandle=NULL;
void air_adc_get_task(void *parm);
void register_adc();

