ESP-IDF GATT SERVER创建属性表演示
===============================================

这是用户使用ESP_API创建GATT服务器属性表的演示。
这个表格很容易用来创建GATT服务器服务数据库，而不需要使用每个“属性创建”功能。
实际上，创建服务器服务和特性有两种方法。
一个是使用esp_gatts_create_service或esp_ble_gatts_add_char等。
另一种方法是使用esp_ble_gatts_create_attr_tab。
重要的事情：两种方式不能在同一个服务中使用，但可以在不同的服务中使用。

请查看[教程](tutorial/GATT_Server_Service_Table_Example_Walkthrough.md)以获取关于此示例的更多信息。