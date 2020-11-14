/*
 * BleDevice.c
 *
 *  Created on: 14 Nov 2020
 *      Author: Rabby
 */


#include "BleDevice.h"

static const char *TAG = "BleDevice";

bleDevice_handler_t *mDeviceHandler = (void*)0;

//
static void gap_event_handler(esp_gap_ble_cb_event_t event,
		esp_ble_gap_cb_param_t *param);
static void gatts_event_handler(esp_gatts_cb_event_t event,
		esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

void BleDevice_init(char *pName)
{
	if(!mDeviceHandler){   // singletone pattern
		mDeviceHandler = (bleDevice_handler_t*) malloc(sizeof(bleDevice_handler_t));
		if(!mDeviceHandler) // if malloc fail then return from that function without init
			return;
		mDeviceHandler->mName = pName;
		mDeviceHandler->profileCount = 0;
		mDeviceHandler->pList = custom_list_new();
		// start init the ESP specific
		ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
		esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
		esp_err_t ret;
		ret = esp_bt_controller_init(&bt_cfg);
		if (ret) {
				ESP_LOGE(TAG, "%s enable controller failed: %s", __func__,
						esp_err_to_name(ret));
				return;
			}
			ESP_LOGI(TAG, "Bluetooth initializing done.");
			ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
			if (ret) {
				ESP_LOGE(TAG, "%s enable controller failed: %s", __func__,
						esp_err_to_name(ret));
				return;
			}
			ESP_LOGI(TAG, "Bluetooth mode set to Low energy only");
			ret = esp_bluedroid_init();
			if (ret) {
				ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__,
						esp_err_to_name(ret));
				return;
			}
			ESP_LOGI(TAG, "bluedroid initialized");
			ret = esp_bluedroid_enable();
			if (ret) {
				ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__,
						esp_err_to_name(ret));
				return;
			}
			ESP_LOGI(TAG, "bluedroid enabled");
			ret = esp_ble_gatts_register_callback(gatts_event_handler);
			if (ret) {
				ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
				return;
			}
			ESP_LOGI(TAG, "GATT event handler registered");
			ret = esp_ble_gap_register_callback(gap_event_handler);
			if (ret) {
				ESP_LOGE(TAG, "gap register error, error code = %x", ret);
				return;
			}
			ESP_LOGI(TAG, "GAP event handler registered");
			esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(512);
			if (local_mtu_ret) {
				ESP_LOGE(TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
			}
			ESP_LOGI(TAG, "Local mtu set to 512");
//			ret = esp_ble_gatts_app_register(applicationID);
//			if (ret) {
//				ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
//				return;
//			}
//			ESP_LOGI(TAG, "Register to gap service");

			ret = esp_ble_gap_set_device_name(mDeviceHandler->mName);
			if (ret) {
				ESP_LOGE(TAG, "set device name failed, error code = %x", ret);
			}
	}

}


bleDevice_handler_t *BleDevice_get(void)
{
	return mDeviceHandler;
}


unsigned int BleDevice_getProfileCount(void)
{
	return mDeviceHandler->profileCount;
}

void BleDevice_addProfile(void* pProfile)
{

}




static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{

}
static void gatts_event_handler(esp_gatts_cb_event_t event,	esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{

}


