# Examples

这个目录包含了越来越多的esp-idf简单示例项目.这些旨在显示基本的esp-idf功能,并提供您可以用于您自己的项目.

# Using Examples

构建示例与构建任何其他项目相同:

* 按照顶级esp-idf README中的设置说明进行操作.

* 设置`IDF_PATH`环境变量指向esp-idf顶级目录的路径.
* 更改为您想要构建的示例的目录.
* `make menuconfig`来配置这个例子.大多数示例都需要通过此配置使用简单的WiFi SSID和密码.
* `make`来建立例子.
* 按照打印的说明进行闪光,或运行`make flash`.

# Copying Examples

每个示例都是独立项目.示例*不必位于esp-idf目录*中.您可以将示例目录复制到计算机的任何位置,以便制作可以修改和使用的副本.

`IDF_PATH`环境变量是将示例与`esp-idf`系统的其余部分相连接的唯一东西.

如果您正在寻找一个更简单的项目,请尝试[esp-idf-template](https://github.com/espressif/esp-idf-template).

# Contributing Examples

如果你有一个你认为我们想要的新例子,请考虑将它作为合并请求发送给我们.

请阅读概述贡献规则的esp-idf CONTRIBUTING.rst文件.

另外,这里有一些关于创建好例子的技巧:

* 一个很好的例子被记录下来,并且可以配置基本选项.
* 一个很好的例子不包含很多代码.如果示例中有很多通用代码,请考虑将该代码重构为独立组件,然后在示例中使用该组件的API.
* 示例中的名称(文件,函数,变量等)应与IDF的其他部分的名称区分开来(理想情况下,在名称中使用`example`).
* 示例中使用的函数和变量在可能的情况下应声明为静态.
* 例子应该展示一个独特的东西.避免多用途的“演示”示例,将其分解为多个示例.
* 示例必须根据Apache许可证2.0或(如果可能,最好举例)授权,您可以声明该示例为Public Domain / Creative Commons Zero.
