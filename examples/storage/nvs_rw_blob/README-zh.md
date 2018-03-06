# 非易失性存储（NVS）读取和写入示例

演示如何使用NVS读取和写入单个整数值和blob（二进制大对象），以在ESP32模块重新启动之间保留它们。

  * value - 跟踪ESP32模块软启动和硬启动的次数。
  * blob - 包含一个包含模块运行时间的表。该表从NVS读取到动态分配的RAM。在每次手动触发的软重新启动时将新的运行时间添加到表中并写回​​到NVS。通过拉低GPIO0完成触发。

示例还显示了如何在读取/写入操作成功时实施诊断。

如果还没有完成，请考虑检查更简单的示例 *storage/nvs_rw_value*, 该示例已被用作准备此文件的起点。

NVS和API的详细功能描述在 [documentation](https://esp-idf.readthedocs.io/en/latest/api-reference/nvs_flash.html)中提供.

有关示例的更多信息，请参阅上层'examples'目录中的README.md文件。.
