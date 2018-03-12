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
#include "air_storge.h"

#define AIR_LOG_PATH ("/spiflash/air_log.txt")
// 挂载路径 partition
const char *base_path = "/spiflash";

static const char *TAG = "air_storge";
// 损耗均衡库实例
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

/** commonds */
int cmd_storge_init()
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

int cmd_storge_read()
{
    //打开文件
    FILE *f = fopen(AIR_LOG_PATH, "rb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return -1;
    }
    char line[128];
    while (!feof(f))
    {
        //读取内容
        fgets(line, sizeof(line), f);
        // strip newline
        char *pos = strchr(line, '\n');
        if (pos)
        {
            *pos = '\0';
        }
        ESP_LOGI(TAG, "Read from file: '%s'", line);
    }
    //关闭文件
    fclose(f);

    return 0;
}

int cmd_storge_write(uint32_t adc_reading, uint32_t voltage)
{
    //打开文件
    FILE *f = fopen(AIR_LOG_PATH, "ab");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return -1;
    }
    //写入adc数据 格式raw,voltage\n
    fprintf(f, "%d,%d\n", adc_reading, voltage);
    //关闭文件
    fclose(f);
    ESP_LOGI(TAG, "File written");
    return 0;
}

int cmd_storge_write_test()
{
    //打开文件
    FILE *f = fopen(AIR_LOG_PATH, "ab");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return -1;
    }
    //写入adc数据 格式raw,voltage\n
    fprintf(f, "%d,%d\n", 0, 1);
    //关闭文件
    fclose(f);
    ESP_LOGI(TAG, "File written");
    return 0;
}

int cmd_storge_uninit()
{
    //卸载flash
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount(base_path, s_wl_handle));
    return 0;
}

/** commonds register*/
void register_storge()
{
    esp_console_cmd_t cmd_table[] =
        {

            {
                .command = "storge_uninit",
                .help = "",
                .hint = NULL,
                .func = &cmd_storge_uninit,
            },
            {
                .command = "storge_write",
                .help = "",
                .hint = NULL,
                .func = &cmd_storge_write_test,
            },
            {
                .command = "storge_init",
                .help = "",
                .hint = NULL,
                .func = &cmd_storge_init,
            },
            {
                .command = "storge_read",
                .help = "",
                .hint = NULL,
                .func = &cmd_storge_read,
            },
            {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
            }

        };

    for (esp_console_cmd_t *p = cmd_table; p->command != NULL; p++)
    {
        ESP_ERROR_CHECK(esp_console_cmd_register(p));
    }
}
