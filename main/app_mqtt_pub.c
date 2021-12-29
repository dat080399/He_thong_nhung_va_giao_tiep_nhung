#include "app_mqtt_pub.h"
#include "app_mqtt.h"
#include "string.h"

static char *topic_pub      = "/v1.6/devices/tram-xang-tu-dong";

void app_mqtt_queue_init(void)
{
    xQueue_MQTT_PUB = xQueueCreate( 20, sizeof(struct sub_option_t ) );
}

void app_mqtt_queue_send(struct sub_option_t topic)
{
    xQueueSend( xQueue_MQTT_PUB, &topic, portMAX_DELAY);
}

void app_mqtt_task_pub(void *pvParameters )
{
    app_mqtt_queue_init();
    struct sub_option_t data_to_pub;
    while(1)
    {
        // void* data_to_pub;
        xQueueReceive(xQueue_MQTT_PUB, &data_to_pub,  portMAX_DELAY);
        app_mqtt_publish(topic_pub, ((struct sub_option_t)data_to_pub).pub_data, strlen((char*) (((struct sub_option_t)data_to_pub).pub_data)));
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}