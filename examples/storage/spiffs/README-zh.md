# SPIFFS示例

这个例子演示了如何在ESP32中使用SPIFFS。示例执行以下步骤：

1.使用"all-in-one" `esp_vfs_spiffs_register`功能：
     - 初始化SPIFFS，
     - 使用SPIFFS库装载SPIFFS文件系统（如果无法装入文件系统，则使用格式），
     - 在VFS中注册SPIFFS文件系统，使用C标准库和POSIX功能。
2.使用`fopen`创建一个文件并使用`fprintf`写入。
3.重命名文件。在重命名之前，使用`stat`函数检查目标文件是否已经存在，并使用`unlink`函数将其删除。
4.打开重命名的文件进行读取，读回行，并将其打印到终端。

## 输出示例

这是一个控制台输出的示例。在这种情况下，`format_if_mount_failed`参数在源代码中设置为`true`。 SPIFFS未格式化，所以初始安装失败。然后格式化SPIFFS，然后重新安装。

```
I (195) example: Initializing SPIFFS
E (195) SPIFFS: mount failed, -10025. formatting...
I (4525) example: Opening file
I (4635) example: File written
I (4685) example: Renaming file
I (4735) example: Reading file
I (4735) example: Read from file: 'Hello World!'
I (4735) example: SPIFFS unmounted
```