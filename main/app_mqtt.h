#ifndef __APP_MQTT_H
#define __APP_MQTT_H
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_mqtt.h"

struct sub_option_t
{
    char *topic;
    uint32_t mask_bit; 
    uint8_t *pub_data;
};

void app_mqtt_init(void);
void app_mqtt_start(void);
void app_mqtt_publish(const char *topic, uint8_t *data, int data_len);
void app_mqtt_subscriber(const char *topic, esp_mqtt_subscribe_cb_t cb);

#endif
