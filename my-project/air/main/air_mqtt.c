#include "air_mqtt.h"


static const char *TAG = "MQTT_SAMPLE";

static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;
esp_mqtt_client_handle_t client = NULL;

// static void print_event(esp_mqtt_event_handle_t event)
// {
//     ESP_LOGI(TAG, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
//     ESP_LOGI(TAG, "DATA=%.*s\r\n", event->data_len, event->data);
// }
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
    //连接完成事件
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // onenet_publish(client);
//两个订阅,等级不同
#if 0
        msg_id = esp_mqtt_client_subscribe(client, "/26942674/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/26942674/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        //取消第二个订阅
        msg_id = esp_mqtt_client_unsubscribe(client, "/26942674/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
#endif
        break;
    //断开连接事件
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");

        break;
    //订阅完成事件
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //订阅完成后发布信息
        // msg_id = esp_mqtt_client_publish(client, "/26942674/qos0", "data", 0, 0, 0);

        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    //取消订阅完成事件
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);

        break;
    //发布完成事件
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        // vTaskDelay(500);
        // onenet_publish(client);
        break;
    //数据事件
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");

        // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        // printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    //错误事件
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    }
    return ESP_OK;
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

void air_wifi_init(void)
{

    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]", CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Waiting for wifi");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}

void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .event_handle = mqtt_event_handler,

        //使用tcp方式传输
        .transport = MQTT_TRANSPORT_OVER_TCP,
        .host = "183.230.40.39",
        .port = 6002,

        //设备id      对应onenet中设备ID
        .client_id = "26943530",
        //用户名       对应onenet中产品产品ID
        .username = "124643",
        //密码        对应onenet中设备的鉴权信息（即唯一设备编号，SN）
        .password = "123456789",
        // .user_context = (void *)your_context
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void onenet_publish(esp_mqtt_client_handle_t client, char *name, float val)
{
    // uint32_t val;
    int msg_id;
    // val = esp_random() % 20 + 15;

    char buf[128];
    memset(buf, 0, sizeof(buf));
    sprintf(&buf[3], "{\"%s\":%f}", name, val);
    uint16_t len = strlen(&buf[3]);
    buf[0] = 0x03; //data_type_simple_json_without_time
    buf[1] = len >> 8;
    buf[2] = len & 0xFF;
    msg_id = esp_mqtt_client_publish(client, "$dp", buf, len + 3, 0, 0);
    // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}


