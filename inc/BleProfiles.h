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
#include "esp_log.h"
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
	list_t *mServiceList;
} ble_profile_t;

typedef struct {
	uint16_t mServiceHandle;
	uint8_t mNumHandle;
	esp_gatt_srvc_id_t *mService_id;
	list_t *mCharList;
} ble_service_t;

typedef struct {
	uint16_t mChar_handle;
	esp_bt_uuid_t *mChar_uuid;
	esp_gatt_perm_t mPerm;
	esp_gatt_char_prop_t mProperty;
	esp_attr_value_t *mAtt;
	esp_attr_control_t mRsp;
	list_t *mDescrList;
} ble_char_t;

typedef struct {
	uint16_t mDescr_handle;
	esp_bt_uuid_t mDescr_uuid;
	esp_attr_value_t mAtt;
	esp_attr_control_t mRsp;
} ble_descrp_t;


ble_profile_t* BleProfiles_createProfile(void);
ble_service_t* BleProfiles_createService(uint8_t *uuid, uint8_t len, uint8_t nHandle, bool isPrimary);
ble_char_t* BleProfiles_createCharacteristic(uint8_t *uuid, uint8_t len, uint8_t autoRsp);
void BleProfiles_setCharacteristicPermission(ble_char_t *ch, uint16_t pem);
void BleProfiles_setCharacteristicProperty(ble_char_t *ch, uint8_t property);
void BleProfile_setCharacteristicValue(ble_char_t *ch, uint8_t *value, uint8_t len, uint8_t maxLen);
void BleProfiles_addService(ble_profile_t *pf, ble_service_t *service);
void BleProfiles_addCharacteristic(ble_service_t *service, ble_char_t *characteristic);

//ble_descrp_t* BleProfiles_createDescription(uint8_t *uuid, uint8_t uuidLen, esp_gatt_char_prop_t property);



#endif /* INC_BLEPROFILES_H_ */
