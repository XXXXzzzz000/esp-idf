# GATT Client Multi-Connection Example Walkthrough

## Introduction
本文档介绍了ESP32的多连接BLE GATT客户端示例.在这个实现中,作为GATT客户端的单个ESP32同时连接到三个不同的GATT服务器.该设置说明了ESP32设备的使用情况,以便从不同的BLE传感器接收数据.除了连接多个外设,ESP32的BLE + Wi-Fi功能的独特组合使其成为物联网网关的绝佳选择.

这个例子的工作流程类似于** GATT客户端示例演练**,如图1所示.然而,在多连接实现中,GATT客户端搜索三个特定的服务器名称,一旦发现它们打开一个接一个地连接到所有三个人.在代码中,每个连接分别由一个应用程序配置文件处理.

为了演示这个例子,需要四个ESP32设备,其中:

* 一个人将被用作与`gattc_multi_connect`演示闪现的GATT客户端,
* 其余部分作为GATT服务器,运行在ESP-IDF蓝牙示例文件夹的`gatt_server`演示中.

！[多连接GATT客户端流程图](图片/多连接GATT客户端Flowchart.png)

## Includes
多连接示例的主源文件是`gattc_multi_connect.c`.有关详细信息,请参阅** GATT客户端示例演练**中的** **包含**.

## Main Entry Point
请参阅** GATT客户端示例演练**中的**主入口点**.

## Implementation Procedure
GATT客户实施包括以下步骤:

* 系统初始化,
* 扫描配置,
* 扫描附近的设备,
* 连接到感兴趣的设备,
* 注册通知.

### Initializing
请参阅** GATT客户端示例演练**中的**主入口点**.

#### Application Profiles
应用程序配置文件是分组功能的一种方式它们的设计使每个应用程序配置文件都连接到一个对等设备,这样,通过为每个应用程序配置文件分配一个应用程序配置文件,相同的ESP32可以连接到多个设备,如图2所示.每个应用程序配置文件创建一个GATT接口连接到其他设备.应用程序配置文件由一个ID号码定义,在这个例子中有三个配置文件:

```
#define PROFILE_NUM 3
#define PROFILE_A_APP_ID 0
#define PROFILE_B_APP_ID 1
#define PROFILE_C_APP_ID 2
```
！[ESP32 GATT多连接客户端应用程序配置文件](image / ESP32 GATT多连接客户端应用程序配置文件)

`esp_ble_gattc_app_register()`功能用于注册每个应用程序配置文件到BLE堆栈.注册操作会生成一个在注册事件中作为参数返回的GATT接口.此外,每个应用程序配置文件还由一个结构定义,该结构可用于在堆栈传播新数据时保持应用程序的状态并更新其参数.

代码中的应用程序配置文件是`gattc_profile_inst`结构的实例.有关详细信息,请参阅** GATT客户端示例演练**中的部分**应用程序配置文件**.

### Scanning

#### Setting Scan Parameters
请参阅** GATT客户端示例演练**中的设置扫描参数**.

#### Starting to Scan
请参阅** GATT客户端示例演练**中的开始扫描**.

#### Getting Scan Results
请参阅** GATT客户端示例演练**中的获取扫描结果**部分.

#### Name Comparison

* 首先,从广告数据中提取设备的名称并存储在`adv_name`变量中:

   ```
   adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
							ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
   ```

* 然后,将找到的设备名称与客户端要连接的服务器名称进行比较.服务器名称在`remote_device_name`数组中定义:

   ```
static const char remote_device_name[3][20] = {"ESP_GATTS_DEMO_1",     "ESP_GATTS_DEMO_2", “ESP_GATTS_DEMO_3"};
   ```

   名称比较如下:

   ```
if (strlen(remote_device_name[0]) == adv_name_len && strncmp((char *)adv_name, remote_device_name[0], adv_name_len) == 0) {
                    if (find_device_1 == false) {
                        find_device_1 = true;
                        ESP_LOGI(GATTC_TAG, "Searched device %s", remote_device_name[0]);
                        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, scan_result->scan_rst.bda, 6);
                    }
                    break;
                }
                else if (strlen(remote_device_name[1]) == adv_name_len && strncmp((char *)adv_name, remote_device_name[1], adv_name_len) == 0) {
                    if (find_device_2 == false) {
                        find_device_2 = true;
                        ESP_LOGI(GATTC_TAG, "Searched device %s", remote_device_name[1]);
                        memcpy(gl_profile_tab[PROFILE_B_APP_ID].remote_bda, scan_result->scan_rst.bda, 6);
                    }
                }
                else if (strlen(remote_device_name[2]) == adv_name_len && strncmp((char *)adv_name, remote_device_name[2], adv_name_len) == 0) {
                    if (find_device_3 == false) {
                        find_device_3 = true;
                        ESP_LOGI(GATTC_TAG, "Searched device %s", remote_device_name[2]);
                        memcpy(gl_profile_tab[PROFILE_C_APP_ID].remote_bda, scan_result->scan_rst.bda, 6);
                    }
                    break;
                }                                    
   ```
   
* 如果找到的任何设备名称对应于远程设备名称,则设置`find_device_X`标志并将远程设备的地址存储在`gl_profile_tab`表中.当所有的标志设置,客户端停止扫描,并连接到远程设备.

### Connecting to Remote Devices

#### Connecting to the First Remote Device
一旦找到所有设备,客户端将停止扫描:

```
if (find_device_1 && find_device_2 && find_device_3 && stop_scan == false)
                {
                    stop_scan = true;
                    esp_ble_gap_stop_scanning();
                }
```

扫描停止触发一个`ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT`事件,用于打开到第一个远程设备的连接.第二个和第三个设备在客户端搜索服务时获得连接,在第一个设备上获取特征并注册通知.此工作流程旨在测试每个远程设备之间的通信是否正常工作,然后尝试连接到下一个设备或发生错误时跳到下一个设备.

```
case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Scan stop failed");
            break;
        }
        ESP_LOGI(GATTC_TAG, "Stop scan successfully");
        if (!stop_scan){
            ESP_LOGE(GATTC_TAG, "Did not find all devices");
        }
        if (find_device_1){
            esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, true);
        }
        break;
```

* 使用`esp_ble_gattc_open()`函数打开连接,该函数将GATT接口,远程设备地址和布尔值设置为true以进行直接连接,或将false设置为后台自动连接.要断开物理连接,使用GAP API函数`esp_ble_gap_disconnect()`.

  当连接到第一个设备时,会生成一个`ESP_GATTC_CONNECT_EVT`事件,并将其转发给所有配置文件.它还会触发一个ESP_GATTC_OPEN_EVT事件,该事件仅被转发给配置文件A事件处理程序或`gattc_profile_a_event_handler()`函数.该事件检查连接是否成功打开,如果不成功,设备将被忽略,并且客户端尝试打开到第二个设备的连接:
  
  ```
  case ESP_GATTC_OPEN_EVT:
        if (p_data->open.status != ESP_GATT_OK){
            //open failed, ignore the first device, connect the second device
            ESP_LOGE(GATTC_TAG, "connect device failed, status %d", p_data->open.status);
            if (find_device_2){
                esp_ble_gattc_open(gl_profile_tab[PROFILE_B_APP_ID].gattc_if, gl_profile_tab[PROFILE_B_APP_ID].remote_bda, true);
            }
            break;
        }
 ```
 
   如果连接成功,则客户端保存连接ID,打印远程设备信息,并将MTU大小配置为200字节.

   ```
gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->open.conn_id;
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_OPEN_EVT conn_id %d, if %d, status %d, mtu %d", p_data->open.conn_id, gattc_if, p_data->open.status, p_data->open.mtu);
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_config_mtu (gattc_if, p_data->open.conn_id, 200);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
   ```

* 配置MTU大小后,会生成一个`ESP_GATTC_CFG_MTU_EVT`.此事件用于在远程设备上搜索可用的已知服务.搜索是通过使用`esp_ble_gattc_search_service()`函数和由以下定义的服务ID来执行的:

   ```
static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
};
   ```

处理程序然后搜索服务:

   ```
case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG,"Config mtu failed");
        }
        ESP_LOGI(GATTC_TAG, "Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &remote_filter_service_uuid);
        break;
   ```
   
   如果找到该服务,则会触发一个`ESP_GATTC_SEARCH_RES_EVT`事件,允许将`get_service_1标志`设置为true.这个标志用来打印信息,以后得到客户感兴趣的特征.

* 一旦搜索完所有服务,就会生成一个`ESP_GATTC_SEARCH_CMPL_EVT`事件,用于获取刚发现的服务的特征.这是通过`esp_ble_gattc_get_characteristic()`函数完成的:
 
   ```
 case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
            break;
        }
        if (get_service_1){
            esp_ble_gattc_get_characteristic(gattc_if, p_data->search_cmpl.conn_id, &remote_service_id, NULL);
        }
        break;
   ```
  `esp_ble_gattc_get_characteristic()`函数将GATT接口,连接ID和远程服务ID作为参数.另外,传递一个NULL值来表示我们想要从第一个开始的所有特性.如果客户对某个特定的特征感兴趣,则可以通过该字段中的特征ID来指定该特征.

   发现特征时触发`ESP_GATTC_GET_CHAR_EVT`事件.此事件用于打印有关特征的信息.

* 如果特征ID与由`REMOTE_NOTIFY_CHAR_UUID`定义的特征ID相同,则客户机注册该特征值的通知.

* 最后,使用相同的`esp_ble_gattc_get_characteristic()`功能请求下一个特征,这次,最后一个参数被设置为当前特征.这触发另一个`ESP_GATTC_GET_CHAR_EVT`,并且重复该过程直到获得所有特征.

   ```
case ESP_GATTC_GET_CHAR_EVT:
        if (p_data->get_char.status != ESP_GATT_OK) {
            break;
        }
        ESP_LOGI(GATTC_TAG, "GET CHAR: conn_id = %x, status %d", p_data->get_char.conn_id, p_data->get_char.status);
        ESP_LOGI(GATTC_TAG, "GET CHAR: srvc_id = %04x, char_id = %04x", p_data->get_char.srvc_id.id.uuid.uuid.uuid16, p_data->get_char.char_id.uuid.uuid.uuid16);

        if (p_data->get_char.char_id.uuid.uuid.uuid16 == REMOTE_NOTIFY_CHAR_UUID) {
            ESP_LOGI(GATTC_TAG, "register notify");
            esp_ble_gattc_register_for_notify(gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, &remote_service_id, &p_data->get_char.char_id);
        }

        esp_ble_gattc_get_characteristic(gattc_if, p_data->get_char.conn_id, &remote_service_id, &p_data->get_char.char_id);
        break;
   ```

在这一点上,客户已经从远程设备获得了所有的特征,并且已经订阅了感兴趣特征的通知.每次客户端注册通知时,都会触发`ESP_GATTC_REG_FOR_NOTIFY_EVT`事件.在这个例子中,这个事件被设置为使用esp_ble_gattc_write_char_descr()函数写入远程设备客户机配置特性(CCC).反过来,这个函数被用来写入特征描述符.蓝牙规范定义了许多特征描述符,但是,在本例中,感兴趣的描述符是处理启用通知的描述符,即客户端配置描述符.

#### Connecting to the Next Remote Device
* 为了将这个描述符作为参数传递,我们首先将它定义为:

   ```
static esp_gatt_id_t notify_descr_id = {
    .uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
    },
    .inst_id = 0,
};
   ```

   其中`ESP_GATT_UUID_CHAR_CLIENT_CONFIG`被定义为用于标识CCC的UUID:
   
   ```
   #define ESP_GATT_UUID_CHAR_CLIENT_CONFIG            0x2902          /*  Client Characteristic Configuration */
   ```

   要写入的值是`1`以启用通知.还传递参数`ESP_GATT_WRITE_TYPE_RSP`以请求服务器响应写入请求以及`ESP_GATT_AUTH_REQ_NONE`参数以指示写入请求不需要授权:
   
   ```
   case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "reg notify failed, error status =%x", p_data->reg_for_notify.status);
            break;
        }
        uint16_t notify_en = 1;
        ESP_LOGI(GATTC_TAG, "REG FOR NOTIFY: status %d, srvc_id = %04x, char_id = %04x",
                p_data->reg_for_notify.status,
                p_data->reg_for_notify.srvc_id.id.uuid.uuid.uuid16,
                p_data->reg_for_notify.char_id.uuid.uuid.uuid16);

        esp_ble_gattc_write_char_descr(gattc_if,
                                       gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                       &remote_service_id,
                                       &p_data->reg_for_notify.char_id,
                                       &notify_descr_id,
                                       sizeof(notify_en),
                                       (uint8_t *)&notify_en,
                                       ESP_GATT_WRITE_TYPE_RSP,
                                       ESP_GATT_AUTH_REQ_NONE);
        break;
    }
    
   ```
* 一旦启用通知,远程设备将发送一个通知,触发客户端上的`ESP_GATTC_NOTIFY_EVT`事件.使用`esp_ble_gattc_write_char()`函数处理这个事件来回写特性:

   ```
case ESP_GATTC_NOTIFY_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, Receive notify value:");
        esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);
        //write  back
        esp_ble_gattc_write_char(gattc_if,
                                gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                &remote_service_id,
                                &p_data->notify.char_id,
                                p_data->notify.value_len,
                                p_data->notify.value,
                                ESP_GATT_WRITE_TYPE_RSP,
                                ESP_GATT_AUTH_REQ_NONE);
        break;
   ```

* 如果写入过程被确认,则远程设备已成功连接,通信建立无误.写入过程立即生成一个`ESP_GATTC_WRITE_CHAR_EVT`事件,在这个例子中,该事件用于打印信息并连接到第二个远程设备:

   ```
case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
        }else{
            ESP_LOGI(GATTC_TAG, "write char success");
        }
        //connect the second device
        if (find_device_2){
            esp_ble_gattc_open(gl_profile_tab[PROFILE_B_APP_ID].gattc_if, gl_profile_tab[PROFILE_B_APP_ID].remote_bda, true);
        }
        break;
   ```
* 这会触发由配置文件B事件处理程序处理的打开事件.该处理程序按照相同的步骤搜索服务,获取特征,注册通知并作为第一个设备写入特征.第二个设备的序列也以`ESP_GATTC_WRITE_CHAR_EVT`事件结束,该事件又被用于连接到第三个设备:

   ```
 case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Write char failed, error status = %x", p_data->write.status);
        }else{
            ESP_LOGI(GATTC_TAG, "Write char success");
        }
        //connect the third device
        if (find_device_3){
            esp_ble_gattc_open(gl_profile_tab[PROFILE_C_APP_ID].gattc_if, gl_profile_tab[PROFILE_C_APP_ID].remote_bda, true);
        }
        break;
   ```

* 第三设备也以与第一和第二设备相同的形式执行相同的配置和通信步骤.成功完成后,所有三个远程设备都可以同时正确连接,并且无误地接收通知.

## Conclusion
在这个例子中,我们已经查看了多连接GATT客户端的示例代码.客户端连接到三个远程BLE外设并搜索感兴趣的服务.如果找到了这些服务,那么这些服务的特征就会被发现和订阅.到远程设备的连接按顺序完成,从第一个连接开始,确保远程设备已成功连接,并在尝试连接下一个设备之前进行通知.这个例子展示了将ESP32作为一个可以同时读取多个BLE传感器的中央设备的实用方法.
