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

uint8_t SERVICE_UUID[] = {0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33};
uint8_t CHAR_UUID[] = {0x00, 0x00, 0x00, 0x02, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33};
uint8_t char1_str[] = "First Char";

uint8_t CHAR_UUID1[] = {0x00, 0x00, 0x00, 0x03, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33};
uint8_t char1_str1[] = "Second Char";


uint8_t DES_UUID[] = {0x00, 0x00, 0x00, 0x04, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33};
uint8_t desValue[] = "This is a test description";

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
	ble_service_t* service =  BleProfiles_createService((uint8_t*)&SERVICE_UUID, ESP_UUID_LEN_128, 0x06, true);
	BleProfiles_addService(profile, service);
	//
	ble_char_t* ch = BleProfiles_createCharacteristic((uint8_t*)&CHAR_UUID, ESP_UUID_LEN_128, ESP_GATT_AUTO_RSP);
	BleProfiles_setCharacteristicPermission(ch, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE );
	BleProfiles_setCharacteristicProperty(ch, (ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY) );
	BleProfile_setCharacteristicValue(ch, char1_str, sizeof(char1_str), 0x40);


	//
	ble_char_t* ch1 = BleProfiles_createCharacteristic((uint8_t*)&CHAR_UUID1, ESP_UUID_LEN_128, ESP_GATT_AUTO_RSP);
	BleProfiles_setCharacteristicPermission(ch1, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE );
	BleProfiles_setCharacteristicProperty(ch1, (ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY) );
	BleProfile_setCharacteristicValue(ch1, char1_str1, sizeof(char1_str1), 0x40);


	//
	ble_descrp_t* des = BleProfiles_createDescription((uint8_t*)&DES_UUID, ESP_UUID_LEN_128, ESP_GATT_AUTO_RSP);
	BleProfiles_setDescriptionPermission(des, ESP_GATT_PERM_READ );
	BleProfile_setDescriptionValue(des, desValue, sizeof(desValue), 0x40);

	BleProfiles_addCharacteristic(service, ch);
	BleProfiles_addCharacteristic(service, ch1);
	BleProfiles_addDescription(ch,des);
	BleDevice_activateProfiles();


}

