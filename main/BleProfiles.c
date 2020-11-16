/*
 * BleProfiles.c
 *
 *  Created on: 15 Nov 2020
 *      Author: Rabby
 */


#include "BleProfiles.h"

static uint16_t mProfileId = 0;


ble_profile_t* BleProfiles_createProfile(void)
{
	ble_profile_t* np = (ble_profile_t*) malloc(sizeof(ble_profile_t));
	memset((void*)np, 0 , sizeof(ble_profile_t));
	np->mGatt_if = ESP_GATT_IF_NONE;
	np->mId = mProfileId;
	np->mGatts_cb = (void*)0;
	np->mServiceList = custom_list_new();
	mProfileId++;
	return np;
}


void BleProfiles_addService(ble_profile_t *pf, ble_service_t *service)
{
	if(pf != 0 && service!= 0)
	{
		list_node_t *node = custom_list_node_new((void*)service);
		custom_list_lpush(pf->mServiceList, node);
	}
}


ble_service_t* BleProfiles_createService(esp_gatt_srvc_id_t *suuid)
{
	ble_service_t* ns = (ble_service_t*) malloc(sizeof(ble_service_t));
	memset((void*)ns, 0 , sizeof(ble_service_t));
	ns->mCharList = custom_list_new();
	ns->service_id = suuid;
	return ns;
}
