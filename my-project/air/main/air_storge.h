#pragma once
//初始化
int cmd_storge_init();
//写入数据
int cmd_storge_write(air_sensor_t sensor_type, uint32_t adc_reading, uint32_t voltage, uint64_t time);
//注册命令
void register_storge();
//读取数据
int cmd_storge_read(int argc, char **argv);
//退出
int cmd_storge_uninit();
