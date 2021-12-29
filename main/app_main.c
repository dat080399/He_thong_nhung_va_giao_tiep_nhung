#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "stdbool.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "app_nvs_flash.h"
#include "app_mqtt.h"
#include "app_config.h"
#include "app_relay.h"
#include "app_nvs_flash.h"
#include "app_timer.h"
#include "app_mqtt_pub.h"

static const char *TAG = "main app";
bool wifi_reset = false;
TaskHandle_t xMQTT_PUB;

void app_main()
{
    app_relay_config();
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    app_timer_for_flash_init();
    app_count_reset(&wifi_reset);
    xTaskCreate( app_mqtt_task_pub, "MQTT_PUB", 1024, NULL, 4, &xMQTT_PUB );
    app_mqtt_init();
    app_config_start();
}
