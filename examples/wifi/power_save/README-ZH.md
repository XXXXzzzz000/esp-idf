# Wifi Power Save Example

这个例子展示了如何使用wifi的省电模式.

省电模式只能在sta模式下使用.

* 没有省电:这是默认模式. esp32将以全力运作.

* 调制解调器睡眠:如果您将节能模式设置为调制解调器睡眠,在连接至AP后10秒后,esp32将定期唤醒并休眠(关闭RF和PHY).

* 其他:尚未支持.
