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
#include "base_Cam.h"


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
        // Send 0x02 if iRet == -1
        send_uart(UART_NUM_0, "2", 1);

        vTaskDelay(pdMS_TO_TICKS(100));
        shutdown_sound(&ledc_channel);
        vTaskDelay(pdMS_TO_TICKS(100));
        stop_sound(&ledc_channel);

        vTaskDelay(pdMS_TO_TICKS(200));
        // Reset
        esp_restart();
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
    // vTaskDelay(pdMS_TO_TICKS(100));
    // alarm_sound(&ledc_channel);
    // vTaskDelay(pdMS_TO_TICKS(100));
    // stop_sound(&ledc_channel);

    // Connect to WiFi
    iRet = wifi_connect(ssid, password);

    if (iRet == 1 || iRet == -1) {
        // Send 0x01 if iRet == 1, 0x02 if iRet == -1
        send_uart(UART_NUM_0, "2", 1);

        vTaskDelay(pdMS_TO_TICKS(100));
        error_sound(&ledc_channel);
        vTaskDelay(pdMS_TO_TICKS(100));
        stop_sound(&ledc_channel);

        vTaskDelay(pdMS_TO_TICKS(200));
        // Reset
        esp_restart();
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
        uint8_t* dic = (uint8_t*) malloc(32);
        uint8_t* key = (uint8_t*) malloc(32);

        memset(dic, '\0', 32);
        memset(key, '\0', 32);
        // Wait for wake command
        iRet = wait_for_wake_command(UART_NUM_0, dic, key);

        if (iRet == -1) {
            // Send 0x02 if iRet == -1
            send_uart(UART_NUM_0, "2", 1);

            vTaskDelay(pdMS_TO_TICKS(100));
            shutdown_sound(&ledc_channel);
            vTaskDelay(pdMS_TO_TICKS(100));
            stop_sound(&ledc_channel);

            vTaskDelay(pdMS_TO_TICKS(200));
            // Reset
            esp_restart();
        }

        uint8_t* server = (uint8_t*) malloc(56);
        int port, intval;

        memset(server, '\0', 56);

        // Wait server command
        iRet = wait_for_server_command(UART_NUM_0, server, &port, &intval);

        if (iRet == -1) {
            // Send 0x02 if iRet == -1
            send_uart(UART_NUM_0, "2", 1);

            vTaskDelay(pdMS_TO_TICKS(100));
            shutdown_sound(&ledc_channel);
            vTaskDelay(pdMS_TO_TICKS(100));
            stop_sound(&ledc_channel);

            vTaskDelay(pdMS_TO_TICKS(200));
            // Reset
            esp_restart();
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


        // Start sending data to server
        take_pic_and_send(dic, key, server, port, intval);



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
