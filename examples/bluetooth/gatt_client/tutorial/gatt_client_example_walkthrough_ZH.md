# Gatt客户端示例教程

## 介绍

在这个例子中，将带你阅读ESP32的GATT客户端示例代码。 这个例子实现了一个蓝牙低功耗（BLE）通用属性（GATT）客户端扫描附近的外围服务器，并连接到一个预定义的服务。 然后，客户搜索可用特征并订阅已知特征以接收通知或指示。 该示例可以注册应用程序配置文件并初始化一系列事件，这些事件可用于配置通用访问配置文件（GAP）参数以及处理诸如扫描，连接到外设以及读取和写入特性等事件。

# Includes

此示例位于ESP-IDF examples目录下的 `bluetooth/gatt_client/main` 文件夹中。 位于主文件夹中的`gattc_demo.c`文件包含了我们需要了解的所有功能。 包含在`gattc_demo.c`中的头文件有：

```c
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "controller.h"

#include "bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
```

FreeRTOS和下层系统组件需要这些`includes`功能，包括日志记录功能以及将数据存储在非易失性闪存中的库。 我们对`“bt.h”`, `“esp_bt_main.h”`, `"esp_gap_ble_api.h"`和`“esp_gattc_api.h”`感兴趣，它们实现了这个例子所需的BLE API。

* `bt.h`：从主机侧配置BT控制器和VHCI。
* `esp_bt_main.h`：初始化并启用Bluedroid堆栈。
* `esp_gap_ble_api.h`：实现GAP配置，例如广播和连接参数。
* `esp_gattc_api.h`：实现GATT客户端配置，例如连接外设和搜索服务。

## 主程序入口

程序的入口点是app_main（）函数：

```c
void app_main()
{
    // 初始化 NVS.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s initialize controller failed, error code = %x\n", __func__, ret);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed, error code = %x\n", __func__, ret);
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed, error code = %x\n", __func__, ret);
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable bluetooth failed, error code = %x\n", __func__, ret);
        return;
    }

    //将回调函数注册到gap模块
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gap register failed, error code = %x\n", __func__, ret);
        return;
    }

    //将回调函数注册到gatcc模块
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if(ret){
        ESP_LOGE(GATTC_TAG, "%s gattc register failed, error code = %x\n", __func__, ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gattc app register failed, error code = %x\n", __func__, ret);
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

}
```

主函数通过初始化非易失性存储库来启动。 该库允许将键值对保存在闪存中，并被某些组件（如Wi-Fi库）用于保存SSID和密码：

```c
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
```

## BT控制器和堆栈初始化

主函数还创建了一个类型为`esp_bt_controller_config_t`的BT控制器配置结构初始化BT控制器，并通过`BT_CONTROLLER_INIT_CONFIG_DEFAULT（）`宏生成的默认设置。 BT控制器在控制器侧实现主机控制器接口（HCI），链路层（LL）和物理层（PHY）。 BT控制器对于用户应用是不可见的，并且BLE堆栈在底层处理。 控制器配置包括设置BT控制器堆栈大小，优先级和HCI波特率。 通过创建的设置，BT控制器被初始化并通过`esp_bt_controller_init（）`函数初始化并使能：

```c
esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
```

接下来，在双模模式（BLE + BT Classic）中启用控制器。

```c
ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
```

支持的蓝牙模式有四种：

1.“ESP_BT_MODE_IDLE”：蓝牙未运行
2.“ESP_BT_MODE_BLE”：BLE模式
3.“ESP_BT_MODE_CLASSIC_BT”：BT经典模式
4.“ESP_BT_MODE_BTDM”：双模（BLE + BT Classic）

在BT控制器初始化之后，Bluedroid stack(android 蓝牙协议栈)包括BT Classic和BLE的通用定义和API，通过使用以下API来初始化和使能：

```c
ret = esp_bluedroid_init();
ret = esp_bluedroid_enable();
```
主函数在注册GAP和GATT事件处理程序以及应用程序配置文件并设置支持的最大MTU大小后结束。

```c
    //将回调函数注册到gap模块
    ret = esp_ble_gap_register_callback(esp_gap_cb);

    //将回调函数注册到gattc模块
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }
```

GAP和GATT事件处理程序是用于捕获由BLE堆栈生成的事件并执行函数来配置应用程序参数的函数。 而且，事件处理程序也用来处理来自中央的读写事件。 GAP事件处理程序负责扫描并连接到服务器，GATT处理程序管理客户端连接到服务器后发生的事件，例如搜索服务以及写入和读取数据。 GAP和GATT事件处理程序使用以下方法进行注册：

```c
esp_ble_gap_register_callback();
esp_ble_gattc_register_callback();
```
函数 `esp_gap_cb()` 和 `esp_gattc_cb()` 处理由BLE堆栈生成的所有事件。

## Application Profiles

应用程序配置文件是一种将针对一个或多个服务器应用程序设计的功能进行分组的方法。 例如，您可以将应用程序配置文件连接到心率传感器，另一个连接到温度传感器。 每个应用程序配置文件创建一个GATT接口连接到其他设备。 代码中的应用程序配置文件是定义为`gattc_profile_inst`结构的实例：

```c
struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
};
```

应用程序简档结构包含：

* `gattc_cb`：GATT客户端回调函数
* `gattc_if`：此配置文件的GATT客户端接口号
* `app_id`：应用程序配置文件ID号
* `conn_id`：连接ID
* `service_start_handle`：服务启动句柄
* `service_end_handle`：服务结束句柄
* `char_handle`：Char句柄
* `remote_bda`：连接到此客户端的远程设备地址。

在这个例子中有一个应用程序配置文件，其ID被定义为：

```c
#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0
```
应用程序配置文件存储在`gl_profile_tab`数组中，初始化为：

```c
/* 一个基于gatt的配置文件一个app_id和一个gattc_if，这个数组将存储由ESP_GATTS_REG_EVT返回的gattc_if */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* gatt_if默认值ESP_GATT_IF_NONE */
    },
};
```
//FIXME:gattc_profile_a_event_handler 重复两个内容,可能是错的
应用程序配置文件数组的初始化包括为每个配置文件定义回调函数。 它们分别是 `gattc_profile_a_event_handler()` 和 `gattc_profile_a_event_handler()` 。 另外，GATT接口被初始化为`ESP_GATT_IF_NONE`的默认值。 稍后，在注册应用程序配置文件时，BLE堆栈将返回一个GATT接口实例以用于该应用程序配置文件。

配置文件注册会触发一个`ESP_GATTC_REG_EVT`事件，由`esp_gattc_cb（）`事件处理程序处理。 处理程序接受事件返回的GATT接口并将其存储在配置文件表中：

```c
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    ESP_LOGI(GATTC_TAG, "EVT %d, gattc if %d", event, gattc_if);

    /* 如果事件是注册事件，则为每个配置文件存储gattc_if */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }
…
```

最后，回调函数为`gl_profile_tab`表中的每个配置文件调用相应的事件处理程序。

```c
…
/* 如果gattc_if等于配置文件A，则调用配置文件A cb处理程序，所以在这里调用每个配置文件的回调 */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE，不指定某个gatt_if，需要调用每个配置文件的cb函数 */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}
```
## 设置扫描参数

GATT客户端通常扫描附近的服务器，并尝试连接到它们。 但是，为了执行扫描，首先需要设置配置参数。 这是在注册应用程序配置文件后完成的，因为注册一旦完成就会触发 `ESP_GATTC_REG_EVT` 事件。 当这个事件第一次被触发时，GATT事件处理程序捕获它并为配置文件A分配一个GATT接口，然后事件被转发到配置文件A的GATT事件处理程序。在这个事件处理程序中，事件用来调用 `esp_ble_gap_set_scan_params()` 函数，它将`ble_scan_params`结构实例作为参数。 这个结构被定义为：

```c
/// Ble 扫描参数
typedef struct {
    esp_ble_scan_type_t     scan_type;              /*!< 扫描类型 */
    esp_ble_addr_type_t     own_addr_type;          /*!< 所有者地址类型 */
    esp_ble_scan_filter_t   scan_filter_policy;     /*!< 扫描过滤器策略 */
    uint16_t                scan_interval;          /*!< 扫描间隔。 范围：0x0004-0x4000
                                                         默认值：0x0010（10毫秒）
                                                         时间= N * 0.625毫秒
                                                         时间范围：2.5毫秒到10.24秒*/
    uint16_t                scan_window;            /*!< 扫描窗口。 LE扫描的持续时间。 
                                                         LE_Scan_Window应小于或等于LE_Scan_Interval
                                                         范围：0x0004到0x4000
                                                         默认值：0x0010（10 ms）
                                                         时间= N * 0.625毫秒
                                                         时间范围：2.5毫秒到10240毫秒*/
} esp_ble_scan_params_t;
```










它被初始化为：

```c
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};
```

BLE扫描参数配置为使扫描类型处于活动状态（包括读取扫描响应），它是公共类型的，允许读取任何广播设备并具有100ms（1.25ms * 0x50）的扫描间隔和 60 ms（1.25 ms * 0x30）的扫描窗口。

扫描值使用`esp_ble_gap_set_scan_params()` 函数设置：

```c
case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "REG_EVT");
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
```

## 开始扫描

一旦扫描参数被设置，一个`ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT`事件被触发，该事件由GAP事件处理程序`esp_gap_cb()`处理。 此事件用于开始扫描附近的GATT服务器：

```c
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //持续时间的单位是秒
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
        break;
    }
```

使用 `esp_ble_gap_start_scanning()` 函数开始扫描，该函数采用一个代表连续扫描持续时间的参数（以秒为单位）。 一旦扫描周期结束，将触发 `ESP_GAP_SEARCH_INQ_CMPL_EVT` 事件。

## 获取扫描结果

扫描结果一到达，就会显示 `ESP_GAP_BLE_SCAN_RESULT_EVT` 事件，其中包括以下参数：

```c
    /**
     * @brief ESP_GAP_BLE_SCAN_RESULT_EVT
     */
    struct ble_scan_result_evt_param {
        esp_gap_search_evt_t search_evt;            /*!< 搜索事件类型 */
        esp_bd_addr_t bda;                          /*!< 已搜寻的蓝牙装置地址 */
        esp_bt_dev_type_t dev_type;                 /*!< 设备类型 */
        esp_ble_addr_type_t ble_addr_type;          /*!< Ble设备地址类型 */
        esp_ble_evt_type_t ble_evt_type;            /*!< Ble扫描结果事件类型 */
        int rssi;                                   /*!< 搜索设备的RSSI */
        uint8_t  ble_adv[ESP_BLE_ADV_DATA_LEN_MAX + ESP_BLE_SCAN_RSP_DATA_LEN_MAX];     										/*!< 收到EIR */
        int flag;                                   /*!< 广播数据标志位 */
        int num_resps;                              /*!< 扫描结果编号 */
        uint8_t adv_data_len;                       /*!< adv数据长度 */
        uint8_t scan_rsp_len;                       /*!< 扫描响应长度 */
    } scan_rst;                                     /*!< ESP_GAP_BLE_SCAN_RESULT_EVT的事件参数 */
```



这个事件还包括一个子事件列表，如下所示：

```c
/// Sub Event of ESP_GAP_BLE_SCAN_RESULT_EVT
typedef enum {
    ESP_GAP_SEARCH_INQ_RES_EVT             = 0,      /*!< 对等设备的查询结果。 */
    ESP_GAP_SEARCH_INQ_CMPL_EVT            = 1,      /*!< 查询完成。 */
    ESP_GAP_SEARCH_DISC_RES_EVT            = 2,      /*!< 发现对等设备的结果。 */
    ESP_GAP_SEARCH_DISC_BLE_RES_EVT        = 3,      /*!< 在对等设备上查找基于
                                                          BLE GATT的服务的结果. */
    ESP_GAP_SEARCH_DISC_CMPL_EVT           = 4,      /*!< 发现完成。 */
    ESP_GAP_SEARCH_DI_DISC_CMPL_EVT        = 5,      /*!< 发现完成。 */
    ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT  = 6,      /*!< 搜索已取消 */
} esp_gap_search_evt_t;
```



我们对 `ESP_GAP_SEARCH_INQ_RES_EVT` 事件感兴趣，每次找到新设备时都会调用这个事件。 我们也对 `ESP_GAP_SEARCH_INQ_CMPL_EVT` 感兴趣，当扫描完成时间被触发，可以用来重新启动扫描过程：
```c
      case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
            ESP_LOGI(GATTC_TAG, "searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            ESP_LOGI(GATTC_TAG, "searched Device Name Len %d", adv_name_len);
            esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
            ESP_LOGI(GATTC_TAG, "\n");
            if (adv_name != NULL) {
                if (strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
                    ESP_LOGI(GATTC_TAG, "searched device %s\n", remote_device_name);
                    if (connect == false) {
                        connect = true;
                        ESP_LOGI(GATTC_TAG, "connect to the remote device.");
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                    }
                }
            }
            break;
```

首先解析设备名称并与 `remote_device_name` 中定义的名称进行比较。 如果它等于我们感兴趣的GATT服务器的设备名称，则停止扫描。


## 连接到GATT服务器

每当我们收到 `ESP_GAP_SEARCH_INQ_RES_EVT` 事件的结果，代码首先打印远程设备的地址：

```c
case ESP_GAP_SEARCH_INQ_RES_EVT:
            esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
```

客户端然后打印广播的数据长度和扫描响应长度：

```c
ESP_LOGI(GATTC_TAG, "searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
```

为了获得设备名称，我们使用`esp_ble_resolve_adv_data()` 函数，它将获取存储在`scan_result->scan_rst.ble_adv`中的广播数据，广播数据的类型和长度，以便从广播分组帧中提取值。 然后打印设备名称。

```c
adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
							ESP_BLE_AD_TYPE_NAME_CMPL,					
							&adv_name_len);
ESP_LOGI(GATTC_TAG, "searched Device Name Len %d", adv_name_len);
esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
```

最后，如果远程设备名称与上面定义的相同，则本地设备将停止扫描，并尝试使用 `esp_ble_gattc_open()` 函数打开与远程设备的连接。该功能采用Application Profile GATT接口，远程服务器地址和布尔值作为参数。布尔值用于指示连接是直接完成还是在后台完成（自动连接），此时必须将此布尔值设置为true以建立连接。请注意，客户端打开到服务器的虚拟连接。虚拟连接返回一个连接ID。虚拟连接是Application Profile和远程服务器之间的连接。由于许多Application Profile可以在一个ESP32上运行，因此可能会有多个虚拟连接对同一个远程服务器打开。还有物理连接是客户端和服务器之间实际的BLE链接。因此，如果物理连接与 `esp_ble_gap_disconnect()` 函数断开连接，则所有其他虚拟连接也将关闭。在这个例子中，每个Application Profile通过 `esp_ble_gattc_open()` 函数创建一个到同一个服务器的虚拟连接，所以当关闭函数被调用的时候，只有那个来自Application Profile的连接被关闭，而如果调用了gap断开连接函数，所有连接都将被关闭(一个是应用层的虚拟连接断开,一个是底层的物理连接断开)。此外，连接事件会转发到所有profiles，因为它涉及物理连接，而打开的事件只转发到创建虚拟连接的profile。

## 配置MTU(最大传输单元)大小

ATT_MTU被定义为在客户端和服务器之间发送的任何数据包的最大大小。 当客户端连接到服务器时，它通知服务器MTU请求和响应协议数据单元(PDU)的数据大小。 这是在连接打开之后完成的。 打开连接后，会触发一个 `ESP_GATTC_CONNECT_EVT` 事件：

```c
     case ESP_GATTC_CONNECT_EVT:
        //p_data->connect.status always be ESP_GATT_OK
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_CONNECT_EVT conn_id %d, if %d, status %d", conn_id, gattc_if, p_data->connect.status);
        conn_id = p_data->connect.conn_id;
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
```

连接ID和远程设备（服务器）的地址存储在Application Profile表中并打印到控制台：

```c
conn_id = p_data->connect.conn_id;
gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, 
		sizeof(esp_bd_addr_t));
ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
esp_log_buffer_hex(GATTC_TAG, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, 
		sizeof(esp_bd_addr_t));
```

蓝牙4.0连接的典型MTU大小为23个字节。 客户端可以使用`esp_ble_gattc_send_mtu_req()` 函数来改变MUT的大小，该函数采用GATT接口和连接ID。 所请求的MTU的大小由 `esp_ble_gatt_set_local_mtu()`定义。 服务器可以接受或拒绝请求。 ESP32支持高达517字节的MTU大小，由 `esp_gattc_api.h`中的 `ESP_GATT_MAX_MTU_SIZE` 定义。 在这个例子中，MTU大小被设置为500字节。 如果配置失败，将返回错误信息：

```c
esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, conn_id);
if (mtu_ret){
	ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
}
break;
```

连接打开还会触发一个`ESP_GATTC_OPEN_EVT`，用于检查连接的打开是否成功，否则打印错误并退出。

```c
case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "open failed, status %d", p_data->open.status);
            break;
        }
ESP_LOGI(GATTC_TAG, "open success");
```

当MTU被改变时，触发一个 `ESP_GATTC_CFG_MTU_EVT`，在这个例子中用来打印新的MTU大小。

```c
case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG,"config mtu failed, error status = %x", param->cfg_mtu.status);
        }
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
…
```

## 发现服务

MTU配置事件也用于客户端连接服务器时发现其可用的服务。 使用 `esp_ble_gattc_search_service()` 发现可用服务。 该函数的参数是GATT接口，应用程序配置文件连接ID和客户端感兴趣的服务应用程序的UUID(Universally Unique Identifier 通用唯一识别码)。要查询的服务定义为：

```c
static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
};
```
`REMOTE_SERVICE_UUID`的定义:

```c
#define REMOTE_SERVICE_UUID        0x00FF
```
然后发现服务如下：

```c
esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &remote_filter_service_uuid);
        break;
```

如果找到服务,将返回 `ESP_GATTC_SEARCH_RES_EVT` 。每个搜索到的服务都将触发该事件(该事件将打印搜索到的服务的信息,信息取决于uuid的大小)

```c
 case ESP_GATTC_SEARCH_RES_EVT: {
        esp_gatt_srvc_id_t *srvc_id = &p_data->search_res.srvc_id;
        conn_id = p_data->search_res.conn_id;
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_16 && srvc_id->id.uuid.uuid.uuid16 == 
REMOTE_SERVICE_UUID) {
        get_server = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
        ESP_LOGI(GATTC_TAG, "UUID16: %x", srvc_id->id.uuid.uuid.uuid16);
        }
        break;
```

在客户端找到正在查找的服务的情况下，标志get_server被设置为true，并且start handle value和end handle value，稍后将用于获取该服务的所有特征。 在返回所有服务结果后，搜索完成并触发 `ESP_GATTC_SEARCH_CMPL_EVT` 事件。

## 获取特征

这个例子实现了从预定义的服务获取特征数据。 我们从一个UUID=0x00FF的服务中获取一个UUID=0xFF01的特征：

```c
#define REMOTE_NOTIFY_CHAR_UUID    0xFF01
```
服务结构 `esp_gatt_srvc_id_t` 定义如下：

```c
/**
 * @brief Gatt id, include uuid and instance id
 */
typedef struct {
    esp_bt_uuid_t   uuid;                   /*!< UUID */
    uint8_t         inst_id;                /*!< Instance id */
} __attribute__((packed)) esp_gatt_id_t;
```

在这个例子中，我们定义了我们想从中获得特性的服务：

```c
static esp_gatt_srvc_id_t remote_service_id = {
    .id = {
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
        },
        .inst_id = 0,
    },
    .is_primary = true,
};
```
//FIXME:有点不对
一旦定义好了，我们就可以使用 `esp_ble_gattc_get_characteristic()` 函数从服务中获得特性，该函数将会在 搜索服务完成事件 `ESP_GATTC_SEARCH_CMPL_EVT` 中调用。

```c
    
case ESP_GATTC_SEARCH_CMPL_EVT:
if (p_data->search_cmpl.status != ESP_GATT_OK){
    ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
    break;
}
conn_id = p_data->search_cmpl.conn_id;
if (get_server){
    uint16_t count = 0;
    esp_gatt_status_t status = esp_ble_gattc_get_attr_count(gattc_if,
                                                            p_data->search_cmpl.conn_id, ESP_GATT_DB_CHARACTERISTIC,
                                                            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                            INVALID_HANDLE,
                                                            &count);
    if (status != ESP_GATT_OK){
        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
    }

    if (count > 0){
        char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
        if (!char_elem_result){
            ESP_LOGE(GATTC_TAG, "gattc no mem");
        }else{
            status = esp_ble_gattc_get_char_by_uuid(gattc_if,
                                                    p_data->search_cmpl.conn_id,
                                                    gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                    gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                    remote_filter_char_uuid,
                                                    char_elem_result,
                                                    &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
            }

            /*  每个服务在'ESP_GATTS_DEMO'演示中只有一个字符，所以我们首先使用'char_elem_result' */
            if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                gl_profile_tab[PROFILE_A_APP_ID].char_handle =
                    char_elem_result[0].char_handle;
                esp_ble_gattc_register_for_notify(gattc_if,
                                                  gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                                  char_elem_result[0].char_handle);
            }
        }
        /* 释放 char_elem_result */
        free(char_elem_result);
    }else{
        ESP_LOGE(GATTC_TAG, "no char found");
    }
}
break;
```



















































`esp_ble_gattc_get_attr_count()` 获得gattc缓存中给定服务或特性的属性数量。 `esp_ble_gattc_get_attr_count()` 函数的参数是GATT接口，连接ID，`esp_gatt_db_attr_type_t`中定义的属性类型，属性起始句柄，属性结束句柄，特征句柄（该参数仅在类型为设置为`ESP_GATT_DB_DESCRIPTOR`时有效。），并输出在给定属性类型的gattc缓存中找到的属性编号。然后我们分配一个缓冲区来保存`esp_ble_gattc_get_char_by_uuid()` 函数的字符信息。该函数在gattc缓存中查找具有给定特征UUID的特征。它只是从本地缓存中获得特性，而不是远程设备。在服务器中，可能有多个字符共享相同的UUID。然而，在我们的gatt_server演示中，每个char都有一个唯一的UUID，这就是为什么我们只使用指向服务特性的指针-`char_elem_result`中的第一个字符。 Count最初存储了客户端想要查找的特性的数量，并且将用`esp_ble_gattc_get_char_by_uuid`在gattc缓存中实际找到的特性的数量进行更新。

## 注册通知

每当特征值改变时，客户端可以通过注册接收来自服务器的通知。 在这个例子中，我们想要注册使用0xff01的UUID标识的特性的通知。 得到所有的特性后，我们检查接收到的特性的属性，然后使用`esp_ble_gattc_register_for_notify（）`函数来注册通知。 函数参数是GATT接口，远程服务器的地址以及我们要注册通知的句柄。

```c
…
/*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                    if(count > 0 && (char_elem_result[0].properties & 
                                                   ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                        gl_profile_tab[PROFILE_A_APP_ID].char_handle = 
                                       char_elem_result[0].char_handle;
                        esp_ble_gattc_register_for_notify (gattc_if, 
                                         gl_profile_tab[PROFILE_A_APP_ID].remote_bda,       
                                         char_elem_result[0].char_handle);
                                }
…
```

This procedure registers notifications to the BLE stack, and triggers an `ESP_GATTC_REG_FOR_NOTIFY_EVT`. This event is used to write to the server Client Configuration Descriptor:

```c
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_REG_FOR_NOTIFY_EVT");
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "REG FOR NOTIFY failed: error status = %d", 
                                             p_data->reg_for_notify.status);
        }else{
            uint16_t count = 0;
            uint16_t notify_en = 1;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                               gl_profile_tab[PROFILE_A_APP_ID].conn_id,                                                                          
                                               ESP_GATT_DB_DESCRIPTOR,
                                gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,                                                         
                                gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0){
                descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * count);
                if (!descr_elem_result){
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                }else{
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,                                                              
                                          gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                     p_data->reg_for_notify.handle,                                                             
                                                                 notify_descr_uuid,
                                                                 descr_elem_result,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle   
                                                                            error");
                    }

                    /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                    if (count > 0 && descr_elem_result[0].uuid.len == ESP_UUID_LEN_16 &&   
descr_elem_result[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ret_status = esp_ble_gattc_write_char_descr( gattc_if,                                                      
                                      gl_profile_tab[PROFILE_A_APP_ID].conn_id,                                                                 
                                                  descr_elem_result[0].handle,
                                                             sizeof(notify_en),
                                                             (Uint8 *)&notify_en,                                                                     
                                                          ESP_GATT_WRITE_TYPE_RSP,                                                                    
                                                           ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                    }

                    /* free descr_elem_result */
                    free(descr_elem_result);
                }
            }
            else{
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }

        }
        break;
    }
```

The event is used to first print the notification register status and the service and characteristic UUIDs of the just registered notifications. The client then writes to the Client Configuration Descriptor by using the `esp_ble_gattc_write_char_descr()` function. There are many characteristic descriptors defined in the Bluetooth specification. However, in this case we are interested in writing to the descriptor that deals with enabling notifications, which is the Client Configuration descriptor. In order to pass this descriptor as parameter, we first define it as:

```c
static esp_gatt_id_t notify_descr_id = {
    .uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
    },
    .inst_id = 0,
};
```
Where `ESP_GATT_UUID_CHAR_CLIENT_CONFIG` is defined with the UUID to identify the Characteristic Client Configuration:

```c
#define ESP_GATT_UUID_CHAR_CLIENT_CONFIG            0x2902          /*  Client Characteristic Configuration */
```
The value to write is “1” to enable notifications. We also pass `ESP_GATT_WRITE_TYPE_RSP` to request that the server responds to the request of enabling notifications and `ESP_GATT_AUTH_REQ_NONE` to indicate that the Write request does not need authorization.



## Conclusion

We have reviewed the GATT Client example code for the ESP32. This example scans for nearby devices and searches for services and characteristics of servers of interest. When the server of interest is found, a connection is made with that server and a search for services is performed. Finally, the client looks for a specific characteristic in the services found, if found, gets the characteristic value and registers for notifications to that characteristic. This is done by registering one Application Profile and following a sequence of events to configure the GAP and GATT parameters required.

