#ifndef _APP_MQTT_PUB_H__
#define _APP_MQTT_PUB_H__

#include "app_mqtt.h"
#include "freertos/queue.h"

QueueHandle_t xQueue_MQTT_PUB;
void app_mqtt_queue_send(struct sub_option_t topic);
void app_mqtt_task_pub(void *pvParameters );

#endif