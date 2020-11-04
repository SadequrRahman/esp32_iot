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
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "tcpip_adapter.h"
#include "MessageQueue.h"

#define WIFI_SSID  "TP-Link_504E"
#define WIFI_PASSWORD  "82302283"


typedef enum{
	WIFI_DISCONNECTED = 0,
	WIFI_CONNECTED,
	WIFI_STATION_CONNECTED
}wifi_driver_event_t;

typedef struct{
	wifi_driver_event_t eventId;
	tcpip_adapter_ip_info_t ipInfo;
}wifiDriver_Msg_t;


void wifiDriver_init(void);
bool wifiDriver_isConnected(void);
bool wifiDriver_isGotIp(void);
tcpip_adapter_ip_info_t wifiDriver_getIpInfo(void);





#endif /* MAIN_WIFIMANAGER_H_ */
