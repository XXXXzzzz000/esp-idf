# ESP-IDF SPP(Serial Port Profile) GATT客户端演示
[SPP](https://baike.baidu.com/item/%E8%93%9D%E7%89%99%E4%B8%B2%E5%8F%A3)

## 1.概述

  在蓝牙经典(BR / EDR)系统中,串行端口配置文件(SPP)是由蓝牙特别兴趣小组(SIG)定义的采用的配置文件,用于仿真通过蓝牙无线连接的串行端口连接.对于BLE系统,BLE上采用的SPP配置文件未定义,因此,串行端口的仿真必须作为供应商特定的自定义配置文件实施.

  该参考设计包含两个演示程序,即在各自的端点上运行的ble spp服务器和ble spp客户端.这些设备彼此无线连接和交换数据.此功能通过无线方式创建虚拟串行链路.每个字节输入都可以由服务器和客户端发送和接收. spp服务器是作为[ble_spp_server](../ble_spp_server)演示实现的,而spp客户端是作为[ble_spp_client](../ble_spp_client)演示实现的. Espressif设计了BLE SPP应用程序以使用UART传输层,但是您可以将此设计与其他串行协议(如SPI)配合使用.

## 2.数据包结构

  Uart接收到数据后,数据将被发送到Uart任务.然后,在UART_DATA事件中,可以检索原始数据.每次最大长度为120个字节.
  如果使用两个ESP32芯片运行ble spp演示程序,则在建立ble连接后,MTU大小将被交换为200个字节,因此每个数据包都可以直接发送.
  如果只运行ble_spp_server演示,并且通过电话连接,则MTU大小可能小于123字节.在这种情况下,数据将被拆分成片段并轮流发送.
  在每个数据包中,我们添加4个字节来表示这是一个分段数据包.如果这是一个分段包,前两个字节包含“##”,第三个字节是包的总数,第四个字节是这个包的当前编号.
  手机应用程序需要检查数据包的结构,如果它想与ble_spp_server演示进行通信.

## 3.软件说明

  该应用程序在[spp_client_demo.c](../ble_spp_client/main/spp_client_demo.c)和[spp_server_demo.c](../ble_spp_server/main/ble_spp_server_demo.c).中实现.

### 3.1初始化

  服务器和客户端都将首先初始化uart和ble.服务器演示将使用属性服务器中的标准GATT和GAP服务来设置串行端口服务.客户端演示将通过无线广播来扫描广播,以查找spp服务器.

### 3.2事件处理

  spp服务器有两个BLE事件的主要事件处理函数:

* static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t * param);
* static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t * param);

  spp客户端具有两个BLE事件的主要事件处理功能:

* esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t * param);
* void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t * param);

  这些是SPP应用程序使用的一些队列和任务:

  队列:

* spp_uart_queue - 从Uart收到的Uart数据信息
* cmd_cmd_queue - 从客户端收到的命令
* cmd_heartbeat_queue - 收到心跳(如果支持)

  任务:

* uart_task - 处理Uart
  * spp_cmd_task - 处理命令消息,命令和处理由客户定义
  * spp_heartbeat_task - 如果心跳被支持,任务将发送一个心跳包给远程设备

### 3.3无线发送数据

  客户端将发送WriteNoRsp数据包到服务器.服务器端通过通知发送数据.当Uart接收到数据时,Uart任务将它放入缓冲区.如果数据大小大于(MTU大小 - 3),数据将被拆分成数据包并依次发送.

### 3.4无线接收数据

  服务器将在 ESP_GATTS_WRITE_EVT 事件中接收此数据,并通过 `uart_wrire_bytes` 函数将数据发送到Uart终端.例如:

    case ESP_GATTS_WRITE_EVT:
            ...
        if(res == SPP_IDX_SPP_DATA_RECV_VAL){
            uart_write_bytes(UART_NUM_0, (char *)(p_data->write.value), p_data->write.len);
        }
            ...
    break;

### 3.5 GATT服务器属性表

  charactertistic|UUID|Permissions
  :-:|:-:|:-:
  SPP_DATA_RECV_CHAR|0xABF1|READ&WRITE_NR
  SPP_DATA_NOTIFY_CHAR|0xABF2|READ&NOTIFY
  SPP_COMMAND_CHAR|0xABF3|READ&WRITE_NR
  SPP_STATUS_CHAR|0xABF4|READ & NOTIFY
  SPP_HEARTBEAT_CHAR|0xABF5|READ&WRITE_NR&NOTIFY

## 4.如何运行这些演示

  编译并下载每个应用程序到ESP32. spp cilent将自动连接到spp服务器,进行服务搜索,交换MTU大小和注册通知.
  如果您将数据输入到Uart终端,它将被打印在远程设备Uart终端上.