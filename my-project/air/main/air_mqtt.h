#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
// #include "onenet_helper.h"
#include "mqtt_client.h"
extern esp_mqtt_client_handle_t client;
//上传数据
void onenet_publish(esp_mqtt_client_handle_t client, char *name, float val);
//启动mqtt
void mqtt_app_start(void);
//初始化WiFi
void air_wifi_init(void);
