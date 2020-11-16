/*
 * BleProfiles.h
 *
 *  Created on: 15 Nov 2020
 *      Author: Rabby
 */

#ifndef INC_BLEPROFILES_H_
#define INC_BLEPROFILES_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "custom_list.h"

//profiles
typedef struct {
    esp_gatts_cb_t mGatts_cb;
    uint16_t mGatt_if;
    uint16_t mId;
    uint16_t mConn_id;
    list_t*  mServiceList;
}ble_profile_t;

typedef struct {
	uint16_t service_handle;
	esp_gatt_srvc_id_t* service_id;
	list_t* mCharList;
}ble_service_t;

typedef struct {
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
}ble_char_t;

ble_profile_t* BleProfiles_createProfile(void);
ble_service_t* BleProfiles_createService(esp_gatt_srvc_id_t *suuid);
void BleProfiles_addService(ble_profile_t *pf, ble_service_t *service);




#endif /* INC_BLEPROFILES_H_ */
