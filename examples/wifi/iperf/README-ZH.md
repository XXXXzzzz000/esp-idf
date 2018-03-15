# Iperf Example

本示例实现了通用性能度量工具[iPerf](https://iperf.fr/)使用的协议.
可以在运行此示例的两个ESP32之间或单个ESP32和运行iPerf工具的计算机之间测量性能

演示测试台站TCP Tx性能的步骤:

1. 使用sdkconfig. defaults构建iperf示例,其中包含性能测试特定的配置

2. 以演示模式运行演示并加入目标AP
   登录密码

3. 在AP侧运行iperf作为服务器
   iperf -s -i 3

4. 在ESP32端运行iperf作为客户端
   iperf -c 192.168.10.42 -i 3 -t 60

控制台输出由站TCP TCP吞吐量测试打印,如下所示:

```
>esp32> sta aptest
>
>I (5325) iperf: sta connecting to 'aptest'
>
>esp32> I (6017) event: ip: 192.168.10.248, mask: 255.255.255.0, gw: 192.168.10.1
>
>esp32> iperf -s -i 3 -t 1000
>
>I (14958) iperf: mode=tcp-server sip=192.168.10.248:5001, dip=0.0.0.0:5001, interval=3, time=1000
>
>Interval Bandwidth
>
>esp32> accept: 192.168.10.42,62958
>
>0-   3 sec       8.43 Mbits/sec
>
>3-   6 sec       36.16 Mbits/sec
>
>6-   9 sec       36.22 Mbits/sec
>
>9-  12 sec       36.44 Mbits/sec
>
>12-  15 sec       36.25 Mbits/sec
>
>15-  18 sec       24.36 Mbits/sec
>
>18-  21 sec       27.79 Mbits/sec
```

测试台/软AP的TCP / UDP RX / TX吞吐量的步骤与台站TCP TX中的测试步骤相似.

有关示例的更多信息,请参阅上层\x26#39