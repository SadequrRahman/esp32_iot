/*
 * mqtt_manager.c
 *
 *  Created on: 23 Nov 2019
 *      Author: Sadequr Rahman
 */

#include "mqtt_manager.h"
#include "esp_log.h"

static const char *TAG = "mqtt";

esp_mqtt_client_handle_t client = NULL;

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}


 void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
    	.event_handle = mqtt_event_handler,
        .uri = "mqtt://mqtt.eclipse.org",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    //esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID , mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
