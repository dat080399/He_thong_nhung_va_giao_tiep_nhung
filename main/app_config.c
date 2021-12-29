#include <string.h>
#include <stdlib.h>
#include "stdbool.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "tcpip_adapter.h"

#include "app_config.h"
#include "app_wifi_sta.h"
#include "app_nvs_flash.h"
#include "app_smart_config.h"
#include "app_sc.h"
#include "app_mqtt.h"

extern bool wifi_reset;

static const int WIFI_STA_HTTP_DONE_BIT = BIT0;
static const int WIFI_STA_CONNECTED_BIT = BIT1;
static const int WIFI_STA_RECV_WIFI_INFO_BIT = BIT2;
static const int WIFI_STA_ESPTOUCH_DONE_BIT =  BIT3;

static EventGroupHandle_t wifi_event_group;
static bool provisioned =  false;

static const char *TAG = "app_config";

static wifi_config_t wifi_config;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    /* For accessing reason codes in case of disconnection */
    system_event_info_t *info = &event->event_info;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, WIFI_STA_CONNECTED_BIT);
        ESP_LOGI(TAG, "GOT IP/n");
        if(provisioned)
        app_mqtt_start();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);
        if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
            /*Switch to 802.11 bgn mode */
            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
        }
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_STA_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void wifi_init(bool *provisioned)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    /* Get WiFi Station configuration */
    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }

    if (strlen((const char*) wifi_cfg.sta.ssid)) {
        *provisioned = true;
        ESP_LOGI(TAG, "Found ssid %s",     (const char*) wifi_cfg.sta.ssid);
        ESP_LOGI(TAG, "Found password %s", (const char*) wifi_cfg.sta.password);
    }
    else{
        ESP_LOGI(TAG, "Not Found Wifi");
    }
}

void app_config_start(void)
{
    wifi_event_group = xEventGroupCreate();
    wifi_init(&provisioned);
    if(!provisioned || wifi_reset)
    {
        app_wifi_sta_init();
        smartconfig_start();
        xEventGroupWaitBits(wifi_event_group, WIFI_STA_CONNECTED_BIT , false, true, portMAX_DELAY);
        app_mqtt_start();
        provisioned = true;
    }
    else
    {
        ESP_LOGI(TAG, "Starting WiFi station");
        app_wifi_sta_init();
    }
}