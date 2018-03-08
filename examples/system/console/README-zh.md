# Console example

这个例子说明了`console`组件的使用来创建一个交互式shell。

## Configuring UART and VFS

``initialize_console``函数配置了与控制台操作相关的UART和VFS的某些方面：

-默认情况下`stdin`和`stdout`是缓冲流。 这意味着写入`stdout`的文本不会立即发送到UART。 这不是控制台的理想行为，因此使用`setvbuf`函数禁用`stdin`和`stdout`的缓冲。

- 行结尾被配置为与通用串行监视器程序（如SDK中包含的``screen``，``minicom``和``idf_monitor.py``）的预期/生成匹配。 这些命令的默认行为是：
    - When 'enter' key is pressed on the keyboard, `CR` (0x13) code is sent to the serial device.
    - To move the cursor to the beginning of the next line, serial device needs to send `CR LF` (0x13 0x10) sequence.

- UART驱动程序已初始化，并且VFS被配置为使用UART驱动程序的中断驱动程序读取和写入功能。

## Line editing

该示例的主要源文件说明了如何使用`linenoise`库，包括行完成，提示和历史记录。

## Commands

几个命令使用`esp_console_cmd_register`函数进行注册。 请参阅`cmd_wifi.c`和`cmd_system.c`文件中的`register_wifi`和`register_system`函数。

## Command handling

Main loop inside `app_main` function illustrates how to use `linenoise` and `esp_console_run` to implement read/eval loop.

## Argument parsing

Several commands implemented in `cmd_wifi.c` and `cmd_system.c` use Argtable3 library to parse and check the arguments.

## Command history

Each time a new command line is obtained from `linenoise`, it is written into history and the history is saved into a file in flash memory. On reset, history is initialized from that file.

# Example output

Here is an sample session with the console example. GPIO15 is connected to GND to remove boot log output.

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
[esp32]> help
help 
  Print the list of registered commands

free 
  Get the total size of heap memory available

restart 
  Restart the program

deep_sleep  [-t <t>] [--io=<n>] [--io_level=<0|1>]
  Enter deep sleep mode. Two wakeup modes are supported: timer and GPIO. If no
  wakeup option is specified, will sleep indefinitely.
  -t, --time=<t>  Wake up time, ms
      --io=<n>  If specified, wakeup using GPIO with given number
  --io_level=<0|1>  GPIO level to trigger wakeup

join  [--timeout=<t>] <ssid> [<pass>]
  Join WiFi AP as a station
  --timeout=<t>  Connection timeout, ms
        <ssid>  SSID of AP
        <pass>  PSK of AP

[esp32]> free
257200
[esp32]> deep_sleep -t 1000
I (146929) deep_sleep: Enabling timer wakeup, timeout=1000000us
I (619) heap_init: Initializing. RAM available for dynamic allocation:
I (620) heap_init: At 3FFAE2A0 len 00001D60 (7 KiB): DRAM
I (626) heap_init: At 3FFB7EA0 len 00028160 (160 KiB): DRAM
I (645) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (664) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (684) heap_init: At 40093EA8 len 0000C158 (48 KiB): IRAM

This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
[esp32]> join --timeout 10000 test_ap test_password
I (182639) connect: Connecting to 'test_ap'
I (184619) connect: Connected
[esp32]> free
212328
[esp32]> restart
I (205639) restart: Restarting
I (616) heap_init: Initializing. RAM available for dynamic allocation:
I (617) heap_init: At 3FFAE2A0 len 00001D60 (7 KiB): DRAM
I (623) heap_init: At 3FFB7EA0 len 00028160 (160 KiB): DRAM
I (642) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (661) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (681) heap_init: At 40093EA8 len 0000C158 (48 KiB): IRAM

This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
[esp32]> 

```

---

See the README.md file in the upper level 'examples' directory for more information about examples.
