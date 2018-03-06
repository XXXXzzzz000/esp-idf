# 非易失性存储（NVS）读取和写入示例

演示如何使用NVS读取和写入单个整数值。

该值保存ESP32模块重新启动的次数。 由于它被写入到NVS，所以在重新启动之间保留该值。

示例还显示了如何检查读/写操作是否成功，或者某些值未在NVS中初始化。 诊断以纯文本形式提供，以帮助跟踪程序流程并捕获途中的任何问题。

检查另一个示例 *storage/nvs_rw_blob*，它显示如何读取和写入可变长度二进制数据（blob）。

NVS和API的详细功能描述在[documentation](https://esp-idf.readthedocs.io/en/latest/api-reference/nvs_flash.html)中提供。

有关示例的更多信息，请参阅上层'examples'目录中的README.md文件。
