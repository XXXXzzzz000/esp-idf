/* Wear levelling and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This sample shows how to store files inside a FAT filesystem.
   FAT filesystem is stored in a partition inside SPI flash, using the
   flash wear levelling library.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "esp_console.h"

#define AIR_LOG_PATH ("/spiflash/air_log.txt")

static const char *TAG = "cmd_storge";
// 损耗均衡库实例
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// 挂载路径 partition
const char *base_path = "/spiflash";

static int cmd_storge_init()
{
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE};
    //挂载flash
    esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, "storage", &mount_config, &s_wl_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount FATFS (0x%x)", err);
        return -1;
    }
    return 0;
}

static int cmd_storge_read()
{
    //读方式打开文件
    FILE *f = fopen(AIR_LOG_PATH, "rb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return -1;
    }
    char line[128];
    //读取内容
    fgets(line, sizeof(line), f);
    //关闭文件
    fclose(f);
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos)
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);
    return 0;
}

static int cmd_storge_write()
{
    FILE *f = fopen(AIR_LOG_PATH, "wb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return -1;
    }
    //写入esp-idf版本
    fprintf(f, "written using ESP-IDF %s\n", esp_get_idf_version());
    //关闭文件
    fclose(f);
    ESP_LOGI(TAG, "File written");
    return 0;
}
static int cmd_storge_uninit()
{
    //卸载flash
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount(base_path, s_wl_handle));
    return 0;
}

/**
 * 命令注册相关
 */

void register_storge()
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
            {NULL, NULL, NULL, NULL,NULL,}

        };

    for (esp_console_cmd_t *p = cmd_table; p->command!=NULL; p++)
    {
        ESP_ERROR_CHECK(esp_console_cmd_register(p));
    }
}

#if 0
static void cmd_storge_init_register()
{
    const esp_console_cmd_t cmd = {
        .command = "storge_init",
        .help = "Get the total size of heap memory available",
        .hint = NULL,
        .func = &cmd_storge_init,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
static void cmd_storge_read_register()
{
    const esp_console_cmd_t cmd = {
        .command = "storge_read",
        .help = "Get the total size of heap memory available",
        .hint = NULL,
        .func = &cmd_storge_read,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static void cmd_storge_write_register()
{
    const esp_console_cmd_t cmd = {
        .command = "storge_write",
        .help = "Get the total size of heap memory available",
        .hint = NULL,
        .func = &cmd_storge_write,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
static void cmd_storge_uninit_register()
{
    const esp_console_cmd_t cmd = {
        .command = "storge_uninit",
        .help = "Get the total size of heap memory available",
        .hint = NULL,
        .func = &cmd_storge_uninit,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
#endif
