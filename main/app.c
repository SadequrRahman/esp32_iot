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
#include "BleDevice.h"



static const char *TAG = "app";

ble_profile_t* profile = (void*)0;
bleDevice_config_t *conf = (void*)0;



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

uint16_t UUID = 0xFF00;

void app_main(void) {
	esp_err_t ret;
	/* Initialize NVS. */
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );


	//MessageQueue_Init(15);
	//MessageQueue_RegisterMsg(wifiDriver, wifiEventReciver);
	//wifiDriver_init();
	heartBeat_init();
	conf = BleDevice_getDefaultConfig();
	BleDevice_init("OnTest", conf);
	// create default profile
	profile = BleProfiles_createProfile();
	BleDevice_addProfile(profile);
	ble_service_t* service =  BleProfiles_createService(&UUID, 2, 4, true);
	BleProfiles_addService(profile, service);
	BleDevice_activateProfiles();


}

