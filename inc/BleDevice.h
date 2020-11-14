/*
 * BleDevice.h
 *
 *  Created on: 14 Nov 2020
 *      Author: Rabby
 */

#ifndef INC_BLEDEVICE_H_
#define INC_BLEDEVICE_H_

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "custom_list.h"


typedef struct{
	char * mName;
	unsigned int profileCount;
	list_t *pList;
}bleDevice_handler_t;

// init the ble device with device name
void BleDevice_init(char *pName);

// get the ble device handler. this will use to add profiles
// return null_ptr if the ble device does not initialised
bleDevice_handler_t *BleDevice_get(void);


// get the number of profile that are added to this device
unsigned int BleDevice_getProfileCount(void);


void BleDevice_addProfile(void* pProfile);



#endif /* INC_BLEDEVICE_H_ */
