#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "esp_console.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

#include "nvs.h"
#include "nvs_flash.h"
#include <ctype.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "soc/rtc_cntl_reg.h"
#include "sdkconfig.h"


#include "air_adc.h"
#include "air_storge.h"
#include "cmd_decl.h"
