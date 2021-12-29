#ifndef _APP_SMART_CONFIG_H__
#define _APP_SMART_CONFIG_H__

#include "esp_smartconfig.h"
#include "smartconfig_ack.h"

typedef void (* smart_config_callback_t)(smartconfig_status_t status, void *pdata);

void app_smart_config_start(void);
void smart_config_set_cb(smart_config_callback_t cb);

#endif