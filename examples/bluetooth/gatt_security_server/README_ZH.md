ESP-IDF GATT SECURITY服务端演示
========================

这是用户使用ESP BLE安全API与对等设备连接和通信的演示。
1.在init阶段，应该使用esp_ble_gap_set_security_param API为BLE堆栈设置将安全参数;
2.使用esp_ble_set_encryption API与对等设备启动加密，如果对等设备采取主动加密，则在收到ESP_GAP_BLE_SEC_REQ_EVT时，应使用esp_ble_gap_security_rsp API向对等设备发送响应。
3.加密完成后，会收到ESP_GAP_BLE_AUTH_CMPL_EVT事件。

请查看[教程](tutorial/GATT_Security_Client_Example_Walkthrough.md)以获取关于此示例的更多信息。