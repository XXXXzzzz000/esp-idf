# Wifi SCAN Example

这个例子展示了如何使用ESP32的扫描.

我们有两种扫描方式,快速扫描和全部频道扫描:

* 快速扫描:在这种模式下,扫描完成后,匹配AP甚至没有扫描所有的通道,可以设置信号和自动模式的阈值,它会忽略阈值以下的AP.

* 全部频道扫描:全部频道扫描结束后,将存储完整匹配AP中的4个,可以根据rssi或authmode设置排序方式,扫描后选择最佳频道

并尝试连接.因为它需要malloc动态内存来存储匹配AP,并且大多数情况下是连接到更好的信号AP,所以它不需要记录所有匹配的AP.为了限制动态内存使用量,匹配次数限制为4.四个匹配允许具有相同SSID名称和所有可能的认证模式的AP  - 开放,WEP,WPA和WPA2.
