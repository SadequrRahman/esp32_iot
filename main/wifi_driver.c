/*
 * wifi_driver.c
 *
 *  Created on: 12 Mar 2019
 *      Author: Sadequr Rahman Rabby
 */
#include "wifi_driver.h"
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "tcpip_adapter.h"
#include "MessageQueue.h"


static bool _isConnected = false;

static void event_handler(void *handler_arg, esp_event_base_t base, int32_t id,
		void *event_data)
{
	ESP_LOGI("wifi", "event handler id: %d", id);

	switch (id) {
	case WIFI_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case WIFI_EVENT_STA_CONNECTED:
		break;
	case IP_EVENT_STA_GOT_IP:
	{
		_isConnected = true;
		msg_t *m = (msg_t*)malloc(sizeof(msg_t));
		uint8_t *status = (uint8_t*)malloc(sizeof(uint8_t));
		m->src = wifiDriver;
		*status = WIFI_CONNECTED;
		m->msg = (void*)status;
		MessageQueue_Send(m);
		free(m);
	}
		break;
	case WIFI_EVENT_STA_DISCONNECTED:
	{
		msg_t *m = (msg_t*)malloc(sizeof(msg_t));
		uint8_t *status = (uint8_t*)malloc(sizeof(uint8_t));
		m->src = wifiDriver;
		*status = WIFI_DISCONNECTED;
		m->msg = (void*)status;
		MessageQueue_Send(m);
		free(m);
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
	wifi_config_t wifi_config = { .sta = {
			.ssid = WIFI_SSID,
			.password = WIFI_PASSWORD,
		},
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
}

uint8_t wifi_isConnected(void) {
	return _isConnected;
}


