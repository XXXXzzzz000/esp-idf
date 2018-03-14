#pragma once

int cmd_storge_init();
int cmd_storge_write(air_sensor_t sensor_type, uint32_t adc_reading, uint32_t voltage, uint64_t time);
void register_storge();
int cmd_storge_read(int argc, char **argv);
int cmd_storge_uninit();
