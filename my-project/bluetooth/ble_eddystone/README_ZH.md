# ESP-IDF Eddystone演示

这个例子演示了Eddystone兼容的eddystone框架的BLE扫描，包括UID，URL。

Eddystone是一个来自Google的开放式信标协议规范，旨在改善“基于邻近性的体验”
同时支持Android和iOS智能设备平台。

通过<https://developers.google.com/beacons>和<https://github.com/google/eddystone>了解更多信息。

## esp_eddystone_protocol.h

这个头文件包含了一些与eddystone协议有关的定义。

## esp_eddystone_api.h＆esp_eddystone_api.c

这些文件包含eddystone帧包的解码和解码结果。

你只需要包含esp_eddystone_protocol.h，esp_eddystone_api.h和esp_eddystone_api.c进行开发。

## esp_eddystone_demo.c

这是使用esp_eddystone_protocol.h，esp_eddystone_api.h和esp_eddystone_api.c文件来解析eddystone数据包的示例/演示。