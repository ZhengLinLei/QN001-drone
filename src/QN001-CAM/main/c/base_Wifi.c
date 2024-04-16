/* 
    WiFi station Example
*/
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/sys.h>

#include "base_Wifi.h"


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static int s_retry_num = 0;


void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;

#ifdef VERBOSE
            printf("Retry to connect to the AP\n");
#endif
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }

#ifdef VERBOSE
        printf("Connect to the AP fail\n");
#endif
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
#ifdef VERBOSE
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("Got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));
#endif

        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

int wifi_connect(uint8_t* ssid, uint8_t* password) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    static wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };

    strcpy((char*)wifi_config.sta.ssid, (const char*)ssid); // copy chars from hardcoded configs to struct
    strcpy((char*)wifi_config.sta.password, (const char*)password);

#ifdef VERBOSE
    printf("SSID: %s\n", wifi_config.sta.ssid);
    printf("Password: %s\n", wifi_config.sta.password);
#endif

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    esp_wifi_connect();

#ifdef VERBOSE
    printf("Connecting to %s\n", ssid);
#endif

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, WIFI_ESP_CONN_TIMEOUT);

    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler);
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler);

    if (bits & WIFI_CONNECTED_BIT) {
#ifdef VERBOSE
        printf("Connected to ssid:%s, password:%s\n", ssid, password);
#endif
        return 0;
    } else if (bits & WIFI_FAIL_BIT) {
#ifdef VERBOSE
        printf("Failed to connect to ssid:%s, password:%s\n", ssid, password);
#endif
        return 1;
    } else {
#ifdef VERBOSE
        printf("Error connecting. Reset 0x00\n");
#endif
        return -1;
    }
}