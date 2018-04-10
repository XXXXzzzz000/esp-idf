#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiny_uart.h"


// extern const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB];

// extern void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
// extern void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
/* Attributes State Machine */
enum {
    IDX_SVC,
    IDX_CHAR_MOTOR,
    IDX_CHAR_VAL_MOTOR,
    HRS_IDX_NB,
};

extern void my_gatt_init(void );