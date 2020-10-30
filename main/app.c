/*
 * wifi_driver.c
 *
 *  Created on: 12 Oct 2019
 *      Author: Sadequr Rahman Rabby
 */

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_websocket_client.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "heartBeat.h"
#include "wifi_driver.h"
#include "esp_websocket_client.h"
#include "mqtt_manager.h"
#include "MessageQueue.h"

static const char *TAG = "app";


void wifiEventReciver(void * mMsg)
{
	uint8_t status = *((uint8_t*)mMsg);
	switch (status) {
		case WIFI_CONNECTED:
			ESP_LOGI(TAG, "wifi connected");
			mqtt_app_start();
			break;
		case WIFI_DISCONNECTED:
			ESP_LOGI(TAG, "wifi disconnected");
			break;
		default:
			break;
	}
}

void app_main(void) {
	nvs_flash_init();
	ESP_LOGI(TAG, "[APP] Startup..");
	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

//	esp_log_level_set("*", ESP_LOG_INFO);
//	esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
//	esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
//	esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
//	esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
//	esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
//	esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

	MessageQueue_Init(15);
	wifiDriver_init();
	MessageQueue_RegisterMsg(wifiDriver, wifiEventReciver);
	heartBeat_init();


}

