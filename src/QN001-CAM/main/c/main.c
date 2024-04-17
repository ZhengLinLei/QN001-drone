#include <stdio.h>

// IDF headers
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_system.h>
#include <esp_http_client.h>

// Project headers
#include "env.h"
#include "base_Comm.h"
#include "base_Buzzer.h"
#include "base_Cmd.h"
#include "base_Wifi.h"
#include "include_Cam.h"
#include "server_Util.h"


// Constants
ledc_channel_config_t ledc_channel;


// Definitions
void setup();
void loop();
void app_main();


// Code
void setup() {
    int iRet;


    // Initialize UART communication
    init_uart(UART_NUM_0, UART_BAUDRATE, TX_PIN, RX_PIN, UART_BUFFER_SIZE, UART_BUFFER_SIZE);

    // Initialize Alarm Led
    esp_rom_gpio_pad_select_gpio(LED_ALARM_PIN);
    gpio_set_direction(LED_ALARM_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_ALARM_PIN, 0);

    // Initialize Light Led
    esp_rom_gpio_pad_select_gpio(LED_LIGHT_PIN);
    gpio_set_direction(LED_LIGHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_LIGHT_PIN, 0);

    // Initialize Alarm Buzzer
    init_buzzer(BUZZER_ALARM_PIN, 1000, &ledc_channel);

    uint8_t* ssid = (uint8_t*) malloc(32);
    uint8_t* password = (uint8_t*) malloc(32);

    memset(ssid, '\0', 32);
    memset(password, '\0', 32);
    iRet = wait_for_wifi_command(UART_NUM_0, ssid, password);

    if (iRet == -1) {
        exit_t(ledc_channel);
    }

#ifdef VERBOSE                
    printf("SSID: %s\n", ssid);
    printf("Password: %s\n", password);
#endif

    // Emit wake sound
    wake_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    wake_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(&ledc_channel);

    // Connect to WiFi
    iRet = wifi_connect(ssid, password);

    if (iRet == 1 || iRet == -1) {
        exit_t(ledc_channel);
    }

    send_uart(UART_NUM_0, "0", 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    alarm_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(&ledc_channel);
}

void loop() {
    int iRet;

    while (1) {
        uint8_t* dic = (uint8_t*) malloc(64);
        uint8_t* key = (uint8_t*) malloc(64);

        memset(dic, '\0', 64);
        memset(key, '\0', 64);
        // Wait for wake command
        iRet = wait_for_wake_command(UART_NUM_0, dic, key);

        if (iRet == -1) {
            exit_t(ledc_channel);
        }

        uint8_t* server = (uint8_t*) malloc(56);
        int port, intval;

        memset(server, '\0', 56);

        // Wait server command
        iRet = wait_for_server_command(UART_NUM_0, server, &port, &intval);

        if (iRet == -1) {
            exit_t(ledc_channel);
        }

        // Configuración del cliente HTTP
        // esp_http_client_config_t http_config = {
        //     .url = "http://mi-servidor.com/upload",
        // };

#ifdef VERBOSE                
        printf("Server: %s\n", server);
        printf("Port: %d\n", port);
        printf("Interval: %d\n", intval);
#endif
        // Emit wake sound
        wake_sound(&ledc_channel);

        // Turn on alarm light for 10 miliseconds
        gpio_set_level(LED_ALARM_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(LED_ALARM_PIN, 0);

        // Stop wake sound
        stop_sound(&ledc_channel);

        vTaskDelay(pdMS_TO_TICKS(1000));


        // -----------------------------
        // Concatenate server and port
        uint8_t* url = (uint8_t*) malloc(64);
        uint8_t* auth = (uint8_t*) malloc(64);
        memset(url, '\0', 64);
        memset(auth, '\0', 64);
        sprintf((char*) url, "http://%s:%d/%s", server, port, dic);
        sprintf((char*) auth, "Bearer %s", key);

        esp_http_client_config_t http_config = {
            .url = (char*) url,
            .method = HTTP_METHOD_POST,
        };
        // -----------------------------
        // Initialize the camera
        esp_err_t err = esp_camera_init(&camera_config);
        if (err != ESP_OK) {
#ifdef VERBOSE
            printf("Error activating camera\n");
#endif
            exit_t(ledc_channel);
        }

#ifdef VERBOSE
        printf("Camera activated\n");
#endif
        while (1) {
            // Check if need to stop
            iRet = check_for_end_command(UART_NUM_0);

            if (iRet == -1) {
                exit_t(ledc_channel);
            }
#ifdef VERBOSE
            printf("Taking picture\n");
#endif

            // Take pic and send
            camera_fb_t *fb = esp_camera_fb_get();
            if (!fb) {
#ifdef VERBOSE
                printf("Error taking picture\n");
#endif

                return;
            }

 #ifdef VERBOSE
            printf("Picture taken. Length: %d\n", fb->len);
#endif

            // Send to server
            esp_http_client_handle_t http_client = esp_http_client_init(&http_config);

            // Set necessary HTTP headers Content-Type and Authorization with Bearer token
            esp_http_client_set_header(http_client, "Content-Type", "image/jpeg");
            esp_http_client_set_header(http_client, "Authorization", (const char*) auth);

            esp_http_client_open(http_client, fb->len);
            esp_http_client_write(http_client, (const char *)fb->buf, fb->len);

            // Read response
            int content_length = esp_http_client_fetch_headers(http_client);
            char* response = (char*) malloc(content_length + 1);
            esp_http_client_read(http_client, response, content_length);
            response[content_length] = '\0';

#ifdef VERBOSE
            printf("Response: %s\n", response);
#endif

            // Clean
            esp_http_client_cleanup(http_client);
            esp_camera_fb_return(fb);


            // ms to ticks
            vTaskDelay(pdMS_TO_TICKS(intval));
        }



        //!TODO Remmber to free memory: dic, key, server
        free(dic);
        free(key);
        free(server);
    }
}

void app_main() {
    esp_log_level_set("*", ESP_LOG_NONE);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    setup();
    loop();
}
