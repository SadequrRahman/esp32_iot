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
#include "BleManager.h"



static const char *TAG = "app";



void wifiEventReciver(void * mMsg)
{
	wifiDriver_Msg_t *msg = (wifiDriver_Msg_t*)mMsg;
	switch (msg->eventId) {
		case WIFI_STATION_CONNECTED:
			ESP_LOGI(TAG, "wifi station is connected");
			break;
		case WIFI_CONNECTED:
			ESP_LOGI(TAG, "wifi gots ip: %s", ip4addr_ntoa(&msg->ipInfo.ip));
			mqtt_app_start();
			break;
		case WIFI_DISCONNECTED:
			ESP_LOGI(TAG, "wifi is disconnected");
			break;
		default:
			break;
	}
}

void app_main(void) {
	esp_err_t ret;

	/* Initialize NVS. */
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );


	MessageQueue_Init(15);
	MessageQueue_RegisterMsg(wifiDriver, wifiEventReciver);
	wifiDriver_init();
	heartBeat_init();
	BleManager_init("SensorNode");
	esp_ble_adv_data_t *advData = BleManager_getDefaultAdvData();
	esp_ble_adv_params_t *advParams = BleManager_getDefaultAdvertiseParam();
	esp_ble_adv_data_t *scanResp = BleManager_getDefaultAdvRespData();

	BleManager_setAdvParams(advParams);
	BleManager_setAdvRespData(scanResp);
	BleManager_setAdvData(advData);


}

