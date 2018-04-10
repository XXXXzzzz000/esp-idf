#pragma once 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"

#include "TinyFrame.h"

typedef unsigned char *pu8;
extern TinyFrame *demo_tf;

extern void tiny_init(void );
