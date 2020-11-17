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

uint16_t SERVICE_UUID = 0x00FF;
uint16_t CHAR_UUID = 0xFF01;
uint8_t char1_str[] = {0x11,0x22,0x33};

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
	ble_service_t* service =  BleProfiles_createService((uint8_t*)&SERVICE_UUID, ESP_UUID_LEN_16, 4, true);
	BleProfiles_addService(profile, service);
	ble_char_t* ch = BleProfiles_createCharacteristic((uint8_t*)&CHAR_UUID, ESP_UUID_LEN_16, ESP_GATT_AUTO_RSP);
	BleProfiles_setCharacteristicPermission(ch, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE );
	BleProfiles_setCharacteristicProperty(ch, (ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY) );
	BleProfile_setCharacteristicValue(ch, char1_str, sizeof(char1_str), 0x40);
	BleProfiles_addCharacteristic(service, ch);


	BleDevice_activateProfiles();


}

