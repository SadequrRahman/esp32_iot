/*
 * wifi_driver.c
 *
 *  Created on: 12 Mar 2019
 *      Author: Sadequr Rahman Rabby
 */
#include "wifi_driver.h"


static bool _isConnected = false;
static bool _isGotIp = false;
static tcpip_adapter_ip_info_t _mIpInfo = {
		{0},
		{0},
		{0}
};

static void event_handler(void *handler_arg, esp_event_base_t base, int32_t id,
		void *event_data)
{
	switch (id) {
	case WIFI_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case WIFI_EVENT_STA_CONNECTED:
	{
		_isConnected = true;
		msg_t *m = (msg_t*)malloc(sizeof(msg_t));
		wifiDriver_Msg_t *msg = (wifiDriver_Msg_t*)malloc(sizeof(wifiDriver_Msg_t));
		m->src = wifiDriver;
		msg->eventId = WIFI_STATION_CONNECTED;
		m->msg = (void*)msg;
		MessageQueue_Send(m);
		free(m);
		free(msg);
	}
		break;
	case IP_EVENT_STA_GOT_IP:
	{
		_isGotIp = true;
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		msg_t *m = (msg_t*)malloc(sizeof(msg_t));
		wifiDriver_Msg_t *msg = (wifiDriver_Msg_t*)malloc(sizeof(wifiDriver_Msg_t));
		m->src = wifiDriver;
		msg->eventId = WIFI_CONNECTED;
		memcpy((void*)&msg->ipInfo, (void*)&event->ip_info, sizeof(tcpip_adapter_ip_info_t) );
		memcpy((void*)&_mIpInfo, (void*)&event->ip_info, sizeof(tcpip_adapter_ip_info_t) );
		m->msg = (void*)msg;
		MessageQueue_Send(m);
		free(m);
		free(msg);
	}
		break;
	case WIFI_EVENT_STA_DISCONNECTED:
	{
		_isGotIp = false;
		_isConnected = false;
		msg_t *m = (msg_t*)malloc(sizeof(msg_t));
		wifiDriver_Msg_t *msg = (wifiDriver_Msg_t*)malloc(sizeof(wifiDriver_Msg_t));
		m->src = wifiDriver;
		msg->eventId = WIFI_DISCONNECTED;
		m->msg = (void*)msg;
		MessageQueue_Send(m);
		free(m);
		free(msg);
		memset((void*)&_mIpInfo, 0, sizeof(tcpip_adapter_ip_info_t) );
		esp_wifi_connect();
	}
		break;
	default:
		break;
	}
}

void wifiDriver_init(void) {
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL ));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL ));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	wifi_config_t wifi_config= { .sta =
			{
			.ssid = WIFI_SSID,
			.password = WIFI_PASSWORD,
			},
	};

//	memcpy((void*)&wifi_config.sta.ssid, (void*)ssid, strlen((const char*)ssid));
//	memcpy((void*)&wifi_config.sta.password, (void*)pwd, strlen((const char*)pwd));
//	memset((void*)&_mIpInfo,0,sizeof(tcpip_adapter_ip_info_t));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
}

bool wifiDriver_isConnected(void) {
	return _isConnected;
}

bool wifiDriver_isGotIp(void){
	return _isGotIp;
}

tcpip_adapter_ip_info_t wifiDriver_getIpInfo(void){
	return _mIpInfo;
}


