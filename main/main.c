#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_websocket_client.h"
#include "esp_log.h"

static const char *TAG = "WEBSOCKET";
static const char *WEBSOCKET_ECHO_ENDPOINT = "wss://echo.websocket.org";

static void websocket_event_handler(void *handler_args, esp_event_base_t base,
		int32_t event_id, void *event_data) {
	// esp_websocket_client_handle_t client = (esp_websocket_client_handle_t)handler_args;
	esp_websocket_event_data_t *data = (esp_websocket_event_data_t*) event_data;
	switch (event_id) {
	case WEBSOCKET_EVENT_CONNECTED:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");

		break;
	case WEBSOCKET_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
		break;

	case WEBSOCKET_EVENT_DATA:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
		ESP_LOGW(TAG, "Received=%.*s\r\n", data->data_len,
				(char* ) data->data_ptr);
		break;
	case WEBSOCKET_EVENT_ERROR:
		ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
		break;
	}
}

static void websocket_task(void *args) {
	ESP_LOGI(TAG, "Connectiong to %s...", WEBSOCKET_ECHO_ENDPOINT);

	const esp_websocket_client_config_t websocket_cfg = { .uri =
			WEBSOCKET_ECHO_ENDPOINT, // or wss://echo.websocket.org for websocket secure
			};

	esp_websocket_client_handle_t client = esp_websocket_client_init(
			&websocket_cfg);
	esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY,
			websocket_event_handler, (void*) client);

	esp_websocket_client_start(client);
	char data[32];
	int i = 0;
	while (true) {
		if (esp_websocket_client_is_connected(client)) {
			int len = sprintf(data, "hello %04d", i++);
			ESP_LOGI(TAG, "Sending %s", data);
			esp_websocket_client_send(client, data, len, portMAX_DELAY);
		}
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
	esp_websocket_client_stop(client);
	ESP_LOGI(TAG, "Websocket Stopped");
	esp_websocket_client_destroy(client);
}

static void heartBeat_task(void *args) {
	ESP_LOGI("HeartBeat", "starting heartbeat");
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	int level = 0;
	while (true) {
		gpio_set_level(GPIO_NUM_2, level);
		level = !level;
		vTaskDelay(300 / portTICK_PERIOD_MS);
	}
	ESP_LOGI("HeartBeat", "Stop heart beat");
}

esp_err_t event_handler(void *ctx, system_event_t *event) {
	return ESP_OK;
}

void app_main(void) {
	nvs_flash_init();
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	wifi_config_t sta_config = { .sta = { .ssid = CONFIG_ESP_WIFI_SSID,
			.password = CONFIG_ESP_WIFI_PASSWORD, .bssid_set = false } };
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());

	xTaskCreate(websocket_task, "websocket_app", 1024 * 5, NULL, 5, NULL);
	xTaskCreate(heartBeat_task, "heartBeat", 1024 * 5, NULL, 5, NULL);

}

