```shell
4: error: invalid operands to binary != (have 'esp_console_cmd_t {aka struct <anonymous>}' and 'int')
         if(cmd_table[i]!=0)
                        ^
/home/petato/esp/esp-idf/make/component_wrapper.mk:273: recipe for target 'cmd_storage.o' failed
make[1]: *** [cmd_storage.o] Error 1
/home/petato/esp/esp-idf/make/project.mk:450: recipe for target 'component-main-build' failed
make: *** [component-main-build] Error 2

```
将一个结构体数组中的结构体和一个int型变量做比较

```c
static void register_storge()
{
    esp_console_cmd_t cmd_table[] =
        {
            {
                .command = "storge_init",
                .help = "Get the total size of heap memory available",
                .hint = NULL,
                .func = &cmd_storge_init,
            },
            {
                .command = "storge_read",
                .help = "Get the total size of heap memory available",
                .hint = NULL,
                .func = &cmd_storge_read,
            },
            {
                .command = "storge_write",
                .help = "Get the total size of heap memory available",
                .hint = NULL,
                .func = &cmd_storge_write,
            },
            {
                .command = "storge_uninit",
                .help = "Get the total size of heap memory available",
                .hint = NULL,
                .func = &cmd_storge_uninit,
            },
            {NULL, NULL, NULL, NULL}

        };

    for (esp_console_cmd_t *p = cmd_table; p->command!=NULL; p++)
    {
        ESP_ERROR_CHECK(esp_console_cmd_register(p));
    }
}
```

解决思路:
1. 采用数组直接限定结构体的大小
缺点: 需要修改for循环和结构体数组的长度

2. 在数组最后添加特定值的结构体元素,在循环中判断是否为特定元素
缺点:判断可能不是简单值的判断(如特定值为string,那么每一个元素都要判断很多次).
