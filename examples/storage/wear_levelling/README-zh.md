# 耗损均衡例子

此示例演示如何使用损耗均衡库和FATFS库将文件存储在SPI flash 内的分区中。示例执行以下步骤：

1. 使用"all-in-one" `esp_vfs_fat_spiflash_mount`函数：
    - 在SPI flash 中找到一个分区，
    - 使用此分区初始化损耗均衡库
    - 使用FATFS库挂载FAT文件系统（如果无法挂载文件系统，则格式化文件系统），
    - 在VFS中注册FAT文件系统，支持使用C标准库和POSIX功能。
2. 使用`fopen`创建一个文件并使用`fprintf`写入。
3. 打开文件进行阅读，读回行，并将其打印到终端。

##输出示例

这是一个典型的控制台输出示例。

```
I (280) example: Mounting FAT filesystem
W (440) vfs_fat_spiflash: f_mount failed (13)
I (440) vfs_fat_spiflash: Formatting FATFS partition, allocation unit size=4096
I (660) vfs_fat_spiflash: Mounting again
I (660) example: Opening file
I (910) example: File written
I (910) example: Reading file
I (920) example: Read from file: 'written using ESP-IDF v3.1-dev-171-gf9ad17eee-dirty'
I (920) example: Unmounting FAT filesystem
I (1000) example: Done
```
