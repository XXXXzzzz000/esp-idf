ESP-IDF GATT MULTIPLE CLIENT演示
========================

这是用户使用ESP_API创建GATT多连接客户端的演示。
gattc_multi_connect演示可以同时连接三个可变的从站。
修改gatt_server demo的名称为ESP_GATTS_DEMO_a，ESP_GATTS_DEMO_b和ESP_GATTS_DEMO_c，然后运行三个演示程序，
gattc_multi_connect演示将连接三个gatt_server演示，然后交换数据。
当然你也可以修改代码来连接更多的设备，我们默认最多连接4个设备，超过4个你需要修改menuconfig。

请查看[教程](tutorial/GATT_Client_Multi-Connection_Example_Walkthrough.md)以获取关于此示例的更多信息。