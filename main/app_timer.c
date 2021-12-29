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

#include "app_nvs_flash.h"
#include "app_timer.h"

TimerHandle_t xTimer_for_flash;
TimerHandle_t xTimer_for_reset;

void vTimer_cb_flash( TimerHandle_t xTimer )
{
    app_reset_flash();
}

void vTimer_cb_reset( TimerHandle_t xTimer )
{
    esp_restart();
}

void app_timer_for_flash_init(void)
{
    xTimer_for_flash = xTimerCreate("Timer for flash", 5000 / portTICK_PERIOD_MS, pdFALSE, (void * ) 0, vTimer_cb_flash);
    xTimerStart(xTimer_for_flash, 10);
}

void app_timer_for_reset_chip(void)
{
    xTimer_for_reset = xTimerCreate("Timer_for_reset_chip", 1500 / portTICK_PERIOD_MS, pdFALSE, (void * ) 0, vTimer_cb_reset);
    xTimerStart(xTimer_for_reset, 10);
}
