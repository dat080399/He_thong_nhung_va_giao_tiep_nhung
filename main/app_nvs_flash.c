#include "nvs.h"
#include "nvs_flash.h"
#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "esp_err.h"

#include "app_nvs_flash.h"

nvs_handle nvs_handle_user;
static uint8_t mode = 0;

#define NAME   "__nvs_user"
#define KEY   "count_reset"
#define KEY_HOMEID  "homeid"
#define KEY_USERID    "userid"

void app_count_reset(bool *wifi_rst)
{
    uint8_t count_reset;
    nvs_open(NAME, NVS_READWRITE, &nvs_handle_user);
    nvs_get_u8(nvs_handle_user,KEY,&count_reset);
    printf("get mode = %d\n", count_reset);
    if(count_reset == 3)
    {
        count_reset = 0;
        *wifi_rst = true;
    }
    else
        count_reset ++;
    nvs_set_u8(nvs_handle_user,KEY,count_reset);
}

void app_reset_flash(void)
{
    uint8_t count_reset = 0;
    nvs_set_u8(nvs_handle_user,KEY,count_reset);
}

void app_wifi_nvs_config(void)
{
    uint8_t count_reset = 3;
    nvs_set_u8(nvs_handle_user,KEY,count_reset);
}

void app_flash_erase_all(void)
{
    ESP_ERROR_CHECK(nvs_flash_erase());
}
