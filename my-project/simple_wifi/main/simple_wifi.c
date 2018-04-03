/* Simple WiFi Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <lwip/err.h>
#include <lwip/sys.h>
#include "lwip/sockets.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_MODE_AP CONFIG_ESP_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_MAX_STA_CONN CONFIG_MAX_STA_CONN

#define EXAMPLE_SERVER_IP CONFIG_SERVER_IP
#define EXAMPLE_SERVER_PORT CONFIG_SERVER_PORT
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "petato wifi";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR " join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR "leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

void wifi_init_sta()
{
    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS},
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

// int espx_last_socket_errno(int socket)
// {
//     int ret = 0;
//     u32_t optlen = sizeof(ret);
//     getsockopt(socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
//     return ret;
// }

void app_main()
{
    //初始化 NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //初始化wifi station
    wifi_init_sta();

    struct sockaddr_in server_addr;
    int sock_fd;
    char msg[] = "hello, you are connected!\n";
    //创建socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        ESP_LOGE(TAG, "failed to create sock_fd!\n");
    }
    //地址信息
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, EXAMPLE_SERVER_IP, &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(EXAMPLE_SERVER_PORT);
    //连接
    int rc = connect(sock, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in));
    if (rc < 0)
    {
        ESP_LOGE(TAG, "connect error\n");
    }
    char msg[] = "hello, you are connected!\n";
    while (1)
    {
        send(sock, (char *)msg, sizeof(msg), 0);
        vTaskDelay(500);
    }
}
