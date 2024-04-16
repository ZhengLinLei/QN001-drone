/*!
* @file main/h/base_Wifi.h
* @brief This file contains the declaration of the base wifi module
*
*/

#ifndef __BASE_WIFI_H__
#define __BASE_WIFI_H__

#include <freertos/FreeRTOS.h>
#include <driver/uart.h>

#include "env.h"

typedef const char*  esp_event_base_t; /**< unique pointer to a subsystem that exposes events */

#define EXAMPLE_ESP_WIFI_SSID      ""
#define EXAMPLE_ESP_WIFI_PASS      ""


#define WIFI_ESP_MAXIMUM_RETRY      5
#define WIFI_ESP_WIFI_MODE          WIFI_MODE_STA
#define WIFI_ESP_CONN_TIMEOUT       portMAX_DELAY //pdMS_TO_TICKS(10000)

#if CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""
#elif CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#elif CONFIG_ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#endif
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/*!
 * @brief Wifi event handler
 * 
 * @param[in] arg Argument
 * @param[in] event_base Event base
 * @param[in] event_id Event id
 * @param[in] event_data Event data
 * 
 * @return void
 */
// static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/*!
 * @brief Initialize wifi
 * 
 * @param[in] ssid SSID
 * @param[in] password Password
 * 
 * @return status 0 - Ok
 *                1 - Failed
 *               -1 - Timeout or Error
 */
int wifi_connect(uint8_t* ssid, uint8_t* password);

#endif // __BASE_WIFI_H__