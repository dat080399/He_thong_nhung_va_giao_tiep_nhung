#include "app_mqtt.h"
#include "esp_log.h"
#include "string.h"
#include "app_relay.h"
#include "app_nvs_flash.h"
#include "app_timer.h"
#include "app_mqtt_pub.h"

static const char *TAG = "MQTTS_APP";

const char *topic_option_1 = "/v1.6/devices/tram-xang-tu-dong/option-1/lv";
const char *topic_option_2 = "/v1.6/devices/tram-xang-tu-dong/option-2/lv";
const char *topic_option_3 = "/v1.6/devices/tram-xang-tu-dong/option-3/lv";
const char *topic_stop     = "/v1.6/devices/tram-xang-tu-dong/stop/lv";
const char *topic_pub      = "/v1.6/devices/tram-xang-tu-dong";

uint8_t *pub_option_1 = (uint8_t*)"{\"option-1\":1}";
uint8_t *pub_option_2 = (uint8_t*)"{\"option-2\":1}";
uint8_t *pub_option_3 = (uint8_t*)"{\"option-3\":1}";
uint8_t *pub_option_stop = (uint8_t*)"{\"stop\":1}";

uint32_t option_is_runing;

struct sub_option_t user_option[5];

uint32_t time_for_relay[3] = {3000, 4500, 5000};

static esp_mqtt_conn_params_t mqtt_conn_param;

void subscribe_state_callback(const char *topic, void *payload, size_t payload_len, void *priv_data)
{
    char *temp;
    temp = (char *)(payload);

    for(int i = 0; i < 5; i++)
    {
        if(!strcmp(topic, user_option[i].topic))
        {
            ESP_LOGI(TAG,"data sub: %.*s", payload_len,(char *)payload);
            if(temp[0] == '0' && option_is_runing == 0)
            {
                ESP_LOGI(TAG, "TOPIC: %d", i);
                // app_relay_set_level(0);
                app_mqtt_set_relay(time_for_relay[i], &user_option[i]);
                option_is_runing |= user_option[i].mask_bit;
            }
            else if(temp[0] == '1')
            {
                if(option_is_runing & user_option[i].mask_bit)
                {
                    app_relay_set_level(0);
                    app_mqtt_stop_timer();
                    option_is_runing &= ~ user_option[i].mask_bit;
                    ESP_LOGI(TAG, "Option_Stop \n");
                }
            }
            else if (option_is_runing)
            {
                // pub to mqtt server
                // vTaskDelay(1000 / portTICK_PERIOD_MS);
                // app_mqtt_publish(topic_pub, user_option[i].pub_data, strlen((char*) user_option[i].pub_data));
                app_mqtt_queue_send(user_option[i]);
                ESP_LOGI(TAG, "Option_error \n");
            }
            return;
        }
    }

    if(!strcmp(topic, topic_stop))
    {
        ESP_LOGI(TAG, "0");
        ESP_LOGI(TAG,"data sub: %.*s", payload_len,(char *)payload);
    }
}

void sub_wifi_config(const char *topic, void *payload, size_t payload_len, void *priv_data)
{
    char *temp;
    temp = (char *)(payload);
    if(temp[0] == '0'&& option_is_runing == 0)
    {
        app_wifi_nvs_config();
        // app_mqtt_publish(topic_pub, pub_option_stop, strlen((char*) pub_option_stop));
        option_is_runing |= 1 << 0;
        app_timer_for_reset_chip();
    }
}

void app_mqtt_init(void)
{
    mqtt_conn_param.mqtt_host = "industrial.api.ubidots.com";
    mqtt_conn_param.user_name = "BBFF-8HrrV0XriGU345p6dywiYX2AX1olal";
    esp_mqtt_init(&mqtt_conn_param);
}

void sub_option_init(char* topic, uint32_t mask, uint8_t* pub_data)
{
    for(int i = 0; i < 5; i++)
    {
        if(user_option[i].topic == NULL)
        {
            user_option[i].topic = topic;
            user_option[i].mask_bit = mask;
            user_option[i].pub_data = pub_data;
            esp_mqtt_add_topic_to_subscribe_list(user_option[i].topic,subscribe_state_callback, MQTT_QOS1,NULL);
            return;
        }
    }
}

void app_mqtt_start(void)
{
    // esp_mqtt_add_topic_to_subscribe_list(topic_option_1,subscribe_state_callback, MQTT_QOS1,NULL);
    // esp_mqtt_add_topic_to_subscribe_list(topic_option_2,subscribe_state_callback, MQTT_QOS1,NULL);
    // esp_mqtt_add_topic_to_subscribe_list(topic_option_3,subscribe_state_callback, MQTT_QOS1,NULL);
    // esp_mqtt_add_topic_to_subscribe_list(topic_stop,subscribe_state_callback, MQTT_QOS1,NULL);
    
    sub_option_init(topic_option_1, 1 << 1, pub_option_1);
    sub_option_init(topic_option_2, 1 << 2, pub_option_2);
    sub_option_init(topic_option_3, 1 << 3, pub_option_3);
    // sub_option_init(topic_stop, 1 << 0, pub_option_stop);
    esp_mqtt_add_topic_to_subscribe_list(topic_stop,sub_wifi_config, MQTT_QOS1,NULL);
    app_mqtt_pub_register_topic_connected(topic_pub, pub_option_stop);
    esp_mqtt_connect();
}

void app_mqtt_publish(const char *topic, uint8_t *data, int data_len)
{
    esp_mqtt_publish(topic,data,data_len,MQTT_QOS1,NULL);
}

void app_mqtt_subscriber(const char *topic, esp_mqtt_subscribe_cb_t cb)
{
    esp_mqtt_subscribe(topic,cb,MQTT_QOS1,NULL);
}
