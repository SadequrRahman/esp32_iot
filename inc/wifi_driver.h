/*
 * wifi_driver.h
 *
 *  Created on: 12 Mar 2019
 *      Author: Sadequr Rahman Rabby
 */

#ifndef __WIFIDRIVER_H__
#define __WIFIDRIVER_H__

#include "stdlib.h"
#include "stdint.h"


#define WIFI_SSID			"TP-Link_504E"
#define WIFI_PASSWORD		"82302283"

typedef enum{
	WIFI_CONNECTED,
	WIFI_DISCONNECTED
}wifi_driver_event_t;

typedef void(*wifiCallback_t)(wifi_driver_event_t);

void wifiDriver_init(void);
void wifiDriver_addListener(wifiCallback_t callback);
uint8_t wifi_isConnected(void);




#endif /* MAIN_WIFIMANAGER_H_ */
