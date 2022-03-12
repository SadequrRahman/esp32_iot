/*
 * BleDevice.c
 *
 *  Created on: 14 Nov 2020
 *  Author: Mohammad Sadequr Rahman
 *  Email: sadequr.rahman.rabby@gmnail.com 
 */


#include "BleDevice.h"
#include "BleProfiles.h"

static const char *TAG = "BleDevice";

bleDevice_handler_t *mDeviceHandler = (void*)0;

//
static void gap_event_handler(esp_gap_ble_cb_event_t event,
		esp_ble_gap_cb_param_t *param);
static void gatts_event_handler(esp_gatts_cb_event_t event,
		esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static uint8_t defaultAdvUUID128[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x89, 0x78, 0x67, 0x56, 0x45, 0x34, 0x23, 0x12 };
//{0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x09, 0x18, 0x00, 0x00 };



void BleDevice_init(char *pName, bleDevice_config_t *config)
{
	if(!mDeviceHandler)
	{
		mDeviceHandler = (bleDevice_handler_t*) malloc(sizeof(bleDevice_handler_t));
		memset((void*)mDeviceHandler, 0, sizeof(bleDevice_handler_t));
		mDeviceHandler->mName = pName;
		mDeviceHandler->mProfileCount = 0;
		mDeviceHandler->mProfileList =  uList_createList();
		mDeviceHandler->mConfig = config;
		// start init the ESP specific
		esp_err_t ret;
		ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
		esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
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
		ret = esp_ble_gap_set_device_name(mDeviceHandler->mName);
		if (ret) {
			ESP_LOGE(TAG, "Set device name failed, error code = %x", ret);
		}
		ret = esp_ble_gap_config_adv_data(&mDeviceHandler->mConfig->mAdvData);
		if (ret) {
			ESP_LOGE(TAG, "config adv data failed, error code = %x", ret);
		}

	}

}


bleDevice_handler_t *BleDevice_getHandler(void)
{
	return mDeviceHandler;
}

bleDevice_config_t* BleDevice_getDefaultConfig(void)
{
	bleDevice_config_t * config = (bleDevice_config_t*) malloc(sizeof(bleDevice_config_t ));
	memset((void*)config, 0, sizeof(bleDevice_config_t));
	// adv data
	config->mAdvData.set_scan_rsp = true;
	config->mAdvData.include_name = true;
	config->mAdvData.include_txpower = true;
	config->mAdvData.min_interval = 0x0006;
	config->mAdvData.max_interval = 0x0010;
	config->mAdvData.appearance = 0x00;
	config->mAdvData.manufacturer_len = 0;
	config->mAdvData.p_manufacturer_data = NULL;
	config->mAdvData.service_data_len = 0;
	config->mAdvData.p_service_data = NULL;
	config->mAdvData.service_uuid_len = sizeof(defaultAdvUUID128);
	config->mAdvData.p_service_uuid = defaultAdvUUID128;
	config->mAdvData.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
	// adv params
	config->mAdvParams.adv_int_min = 0x20;
	config->mAdvParams.adv_int_max = 0x40;
	config->mAdvParams.adv_type = ADV_TYPE_IND;
	config->mAdvParams.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
	config->mAdvParams.channel_map = ADV_CHNL_ALL;
	config->mAdvParams.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

	return config;
}

uint8_t BleDevice_getProfileCount(void)
{
	return mDeviceHandler->mProfileCount;
}

void BleDevice_addProfile(ble_profile_t* pProfile)
{
	if(pProfile!=0 && mDeviceHandler!=0)
	{
		uNode_t *node = uList_createNode((void*)pProfile, sizeof(ble_profile_t),NODE_NON_SELF_ALLOC);
		uList_append(mDeviceHandler->mProfileList, node);
		mDeviceHandler->mProfileCount++;
	}

}

void BleDevice_activateProfiles(void)
{
	if(mDeviceHandler)
	{
		ble_profile_t* profile = (void*)0;
		uNode_t * profileNode = mDeviceHandler->mProfileList->tail;
		while (profileNode)
		{
			/* code */
			profile = (ble_profile_t*)profileNode->value;
			esp_err_t ret = esp_ble_gatts_app_register(profile->mId);
			if (ret)
			{
				ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
			}
			else
			{
				ESP_LOGI(TAG, "gatts app registered. id: %d", profile->mId);
			}
			profileNode = profileNode->nextNode;
		}
	}
}



void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	switch (event) 
	{
		case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
			esp_ble_gap_start_advertising(&mDeviceHandler->mConfig->mAdvParams);
			esp_ble_gap_set_device_name(mDeviceHandler->mName);
			break;
		case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
			ESP_LOGI(TAG, "ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT");
			esp_ble_gap_start_advertising(&mDeviceHandler->mConfig->mAdvParams);
			break;
		case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
			//advertising start complete event to indicate advertising start successfully or failed
			if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) 
			{
				ESP_LOGE(TAG, "Advertising start failed\n");
			} else 
			{
				ESP_LOGI(TAG, "Ble Advertising");
			}
			break;
		case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
			if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) 
			{
				ESP_LOGE(TAG, "Advertising stop failed\n");
			} else 
			{
				ESP_LOGI(TAG, "Stop adv successfully\n");
			}
			break;
		case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
			ESP_LOGI(TAG,
			"update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
			param->update_conn_params.status,
			param->update_conn_params.min_int,
			param->update_conn_params.max_int,
			param->update_conn_params.conn_int,
			param->update_conn_params.latency,
			param->update_conn_params.timeout);
			break;
		case ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:
			ESP_LOGI(TAG, "ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT");
			break;
		case ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT:
			ESP_LOGI(TAG, "ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT");
			break;
		case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT:
			ESP_LOGI(TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT");
			break;
		default:
			break;
		
	}
}


void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
	switch (event)
	{
		case ESP_GATTS_REG_EVT: /*!< When register application id, the event comes */
		{
			ESP_LOGI(TAG, "ESP_GATTS_REG_EVT");
			ble_profile_t* profile = (void*)0;
			// tail is the starting node of the list
			uNode_t *profileNode = mDeviceHandler->mProfileList->tail;
			while (profileNode)
			{
				profile = (ble_profile_t*)profileNode->value;
				ESP_LOGI(TAG, "Profile ID %d", profile->mId);
				if(param->reg.app_id == profile->mId)
				{
					ESP_LOGI(TAG, "Found profile index. setting gatts if");
					profile->mGatt_if = gatts_if;
					ESP_LOGI(TAG, "profile->mGatt_if %d, gatts_if %d\n", profile->mGatt_if, gatts_if);
					ble_service_t * service = NULL;
					uNode_t *serviceNode =  profile->mServiceList->tail;
					while (serviceNode)
					{
						/* code */
						service = (ble_service_t*) serviceNode->value;
						ESP_LOGI(TAG, "Adding services to profile %d", profile->mId );
						esp_ble_gatts_create_service(profile->mGatt_if, service->mService_id, service->mNumHandle);
						serviceNode = serviceNode->nextNode;
					}
				}
				// go to next node
				profileNode = profileNode->nextNode;
			}
		}
			break;
		case ESP_GATTS_READ_EVT:			/*!< When gatt client request read operation, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_READ_EVT");
			break;
		case ESP_GATTS_WRITE_EVT:			/*!< When gatt client request write operation, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_WRITE_EVT");		
			break;
		case ESP_GATTS_EXEC_WRITE_EVT:                 /*!< When gatt client request execute write, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
			break;
		case ESP_GATTS_MTU_EVT:                        /*!< When set mtu complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT");
			break;
		case ESP_GATTS_CONF_EVT:                       /*!< When receive confirm, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT");
			break;
		case ESP_GATTS_UNREG_EVT:                      /*!< When unregister application id, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_UNREG_EVT");
			break;
		case ESP_GATTS_CREATE_EVT:                     /*!< When create service complete, the event comes */
		{
			ESP_LOGI(TAG, "CREATE_SERVICE_EVT, status %d, service_handle %d\n", param->create.status, param->create.service_handle);

			ble_profile_t* profile = (void*)0;
			uNode_t* profileNode = mDeviceHandler->mProfileList->tail;
			while (profileNode)
			{
				/* code */
				profile = (ble_profile_t*)profileNode->value;
				ESP_LOGI(TAG, "profile->mGatt_if %d, gatts_if %d\n", profile->mGatt_if, gatts_if);
				if(profile->mGatt_if == gatts_if)
				{
					ble_service_t* service = (void*)0;
					uNode_t * serviceNode = profile->mServiceList->tail;
					while (serviceNode)
					{
						/* code */
						service = (ble_service_t*)serviceNode->value;
						if(memcmp((void*)&service->mService_id->id.uuid.uuid,
								(void*)&param->create.service_id.id.uuid.uuid,
								service->mService_id->id.uuid.len) == 0 )
						{
							service->mServiceHandle = param->create.service_handle;
							ESP_LOGI(TAG, "\r\nStart service: %d\r\n", service->mService_id->id.inst_id);
							esp_ble_gatts_start_service(service->mServiceHandle);
							ble_char_t* ch = (void*)0;
							uNode_t * charNode = service->mCharList->tail;
							while(charNode)
							{
								ch = (ble_char_t*)charNode->value;
								esp_ble_gatts_add_char(service->mServiceHandle, ch->mChar_uuid, ch->mPerm, ch->mProperty,ch->mAtt, &ch->mRsp);
								charNode = charNode->nextNode;
							}
						}
						serviceNode = serviceNode->nextNode;
					}
				}
				profileNode = profileNode->nextNode;
			}
		}
			break;
		case ESP_GATTS_ADD_INCL_SRVC_EVT:              /*!< When add included service complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_ADD_INCL_SRVC_EVT");
			break;
		case ESP_GATTS_ADD_CHAR_EVT:                   /*!< When add characteristic complete, the event comes */
		{
			ESP_LOGI(TAG, "ESP_GATTS_ADD_CHAR_EVT.\r\nSerive Handle: %d\r\n", param->add_char.service_handle);
			uint16_t length = 0;
		    const uint8_t *prf_char;
			esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle, &length, &prf_char);
			if (get_attr_ret == ESP_FAIL)
			{
			   ESP_LOGE(TAG, "ILLEGAL HANDLE");
			}
			ble_profile_t* profile = (void*)0;
			uNode_t * profileNode = mDeviceHandler->mProfileList->tail;
			while (profileNode)
			{
				/* code */
				profile = (ble_profile_t*)profileNode->value;
				if(profile->mGatt_if == gatts_if)
				{
					ble_service_t* service = (void*)0;
					uNode_t * serviceNode = profile->mServiceList->tail;
					while (serviceNode)
					{
						/* code */
						service = (ble_service_t*)serviceNode->value;
						if(service->mServiceHandle == param->add_char.service_handle)
						{
							ble_char_t* characteristic = (void*)0;
							uNode_t * charNode = service->mCharList->tail;
							while (charNode)
							{
								/* code */
								characteristic = (ble_char_t*) charNode->value; 
								if(memcmp((void*)&characteristic->mChar_uuid->uuid,
									(void*)&param->add_char.char_uuid.uuid,
									characteristic->mChar_uuid->len) == 0 )
								{
									characteristic->mChar_handle = param->add_char.attr_handle;
									ESP_LOGI(TAG, "Found char.Assigned handle id: %d\r\n", characteristic->mChar_handle);
									ble_descrp_t* descrp = (void*)0;
									uNode_t *descrpNode = characteristic->mDescrList->tail;
									while (descrpNode)
									{
										/* code */
										descrp = (ble_descrp_t*) descrpNode->value;
										esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr( service->mServiceHandle,  descrp->mDescr_uuid,
										            											descrp->mPerm, descrp->mAtt, &descrp->mRsp);
										 if (add_descr_ret)
										 {
											ESP_LOGE(TAG, "add char descr failed, error code = %x", add_descr_ret);
										 }
										descrpNode = descrpNode->nextNode;
									}
								}
								charNode = charNode->nextNode;
							}
						}
						serviceNode = serviceNode->nextNode;
					}
				}
				profileNode = profileNode->nextNode;
			}
		}
			break;
		case ESP_GATTS_ADD_CHAR_DESCR_EVT:            /*!< When add descriptor complete, the event comes */
			ESP_LOGI(TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
			                  param->add_char.status, param->add_char.attr_handle,
			                  param->add_char.service_handle);
			break;
		case ESP_GATTS_DELETE_EVT:                    /*!< When delete service complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_DELETE_EVT");
			break;
		case ESP_GATTS_START_EVT:                     /*!< When start service complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_START_EVT");
			break;
		case ESP_GATTS_STOP_EVT:                      /*!< When stop service complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_STOP_EVT");
			break;
		case ESP_GATTS_CONNECT_EVT: 	/*!< When gatt client connect, the event comes */
		{
			esp_ble_conn_update_params_t conn_params = { 0 };
			memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
			/* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
			conn_params.latency = 0;
			conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
			conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
			conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
			ESP_LOGI(TAG,
					"ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
					param->connect.conn_id, param->connect.remote_bda[0],
					param->connect.remote_bda[1], param->connect.remote_bda[2],
					param->connect.remote_bda[3], param->connect.remote_bda[4],
					param->connect.remote_bda[5]);
			ble_profile_t* profile = (void*)0;
			uNode_t * profileNode = mDeviceHandler->mProfileList->tail;
			while (profileNode)
			{
				/* code */
				profile = (ble_profile_t*)profileNode->value;
				if(profile->mGatt_if == gatts_if)
				{
					profile->mConn_id = param->connect.conn_id;
					break;
				}
				profileNode = profileNode->nextNode;
			}
			//start sent the update connection parameters to the peer device.
			esp_ble_gap_update_conn_params(&conn_params);
		}
			break;
		case ESP_GATTS_DISCONNECT_EVT: 					/*!< When gatt client disconnect, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT.\r\n disconnect reason 0x%x", param->disconnect.reason);
			esp_ble_gap_start_advertising(&mDeviceHandler->mConfig->mAdvParams);
			break;
		case ESP_GATTS_OPEN_EVT:                      /*!< When connect to peer, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_OPEN_EVT");
			break;
		case ESP_GATTS_CANCEL_OPEN_EVT:               /*!< When disconnect from peer, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_CANCEL_OPEN_EVT");
			break;
		case ESP_GATTS_CLOSE_EVT:                     /*!< When gatt server close, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_CLOSE_EVT");
			break;
		case ESP_GATTS_LISTEN_EVT:                    /*!< When gatt listen to be connected the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_LISTEN_EVT");
			break;
		case ESP_GATTS_CONGEST_EVT:                   /*!< When congest happen, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_CONGEST_EVT");
			break;
		/* following is extra event */
		case ESP_GATTS_RESPONSE_EVT:                  /*!< When gatt send response complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_RESPONSE_EVT");
			break;
		case ESP_GATTS_CREAT_ATTR_TAB_EVT:            	/*!< When gatt create table complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_CREAT_ATTR_TAB_EVT");
			break;
		case ESP_GATTS_SET_ATTR_VAL_EVT:     			/*!< When gatt set attr value complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_SET_ATTR_VAL_EVT");
			break;
		case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:	      	/*!< When gatt send service change indication complete, the event comes */
			ESP_LOGI(TAG, "ESP_GATTS_SEND_SERVICE_CHANGE_EVT");
			break;
		default:
			break;
	}

}



