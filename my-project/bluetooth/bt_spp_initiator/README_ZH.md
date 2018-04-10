ESP-IDF BT-SPP-ACCEPTOR演示
======================

SPP接受者角色演示

这是用户使用ESP_API创建SPP接受器的演示。

选项选择步骤：

     1. make menuconfig。
     2.进入menuconfig“组件配置”，选择“蓝牙”
     3.进入菜单蓝牙，选择“Classic Bluetooth”和“SPP Profile”
     4.选择你的选择。

然后在代码中将SPP_SHOW_MODE设置为SPP_SHOW_DATA或SPP_SHOW_SPEED（应该与bt_spp_initator相同）。

程序启动后，bt_spp_initator会连接并发送数据。