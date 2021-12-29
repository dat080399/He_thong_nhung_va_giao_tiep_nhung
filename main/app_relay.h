#ifndef _APP_RELAY_H__
#define _APP_RELAY_H__
#include <stdint.h>
#include "app_mqtt.h"

#define GPIO_OUTPUT_IO_0    5
void app_relay_config(void);
void app_relay_set_level(int level);
void app_mqtt_set_relay(uint32_t time_for_relay, struct sub_option_t *sub_option);
void app_mqtt_stop_timer(void);

#endif