#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"

#include "app_mqtt.h"
#include "app_relay.h"
#include "app_mqtt_pub.h"

TimerHandle_t xTimer_for_relay;

struct sub_option_t *current_option;

void vTimerCallback( TimerHandle_t xTimer )
{
    static char *topic_pub = "/v1.6/devices/tram-xang-tu-dong";
    app_relay_set_level(0);
    // push to server mqtt
    // app_mqtt_publish(topic_pub, current_option->pub_data, strlen((char*) current_option->pub_data));
    app_mqtt_queue_send(*current_option);
}

void app_relay_config(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_IO_0;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    xTimer_for_relay = xTimerCreate("Timer for relay", 1000, pdFALSE, (void * ) 0, vTimerCallback);
}

void app_relay_set_level(int level)
{
    gpio_set_level(GPIO_OUTPUT_IO_0, level);
}

void app_mqtt_set_relay(uint32_t time_for_relay, struct sub_option_t *sub_option)
{
    app_relay_set_level(1);
    xTimerChangePeriod( xTimer_for_relay, time_for_relay / portTICK_PERIOD_MS, 10 );
    current_option = sub_option;
}

void app_mqtt_stop_timer(void)
{
    xTimerStop( xTimer_for_relay, 10 );
}