#ifndef __APP_NVS_FLASH_H
#define __APP_NVS_FLASH_H
#include <stdint.h>
#include "stdbool.h"
void app_flash_switch_mode_provision(void);
uint8_t app_flash_get_mode_provision(void);
void app_flash_get_device_info(char *clientid, char *userid, char *homeid);
void app_flash_set_homeid(char *homeid);
void app_flash_set_userid(char *userid);
void app_flash_erase_all(void);
void app_count_reset(bool *wifi_rst);
void app_wifi_nvs_config(void);
void app_reset_flash(void);
#endif