#include <stdio.h>

// IDF headers
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_log.h>

// Project headers
#include "env.h"
#include "base_Comm.h"
#include "base_Buzzer.h"
#include "base_Cmd.h"
#include "base_Wifi.h"


// Constants
ledc_channel_config_t ledc_channel;


// Definitions
void setup();
void loop();
void app_main();


// Code
void setup() {
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
    wait_for_wifi_command(UART_NUM_0, ssid, password);

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
    vTaskDelay(pdMS_TO_TICKS(100));
    alarm_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(&ledc_channel);

    // Connect to WiFi
    // wifi_connect(ssid, password);

}

void loop() {
    while (1) {
        // Wait for wake command
        wait_for_wake_command(UART_NUM_0);

        // Emit wake sound
        wake_sound(&ledc_channel);

        // Turn on alarm light for 10 miliseconds
        gpio_set_level(LED_ALARM_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(LED_ALARM_PIN, 0);

        // Stop wake sound
        stop_sound(&ledc_channel);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    esp_log_level_set("*", ESP_LOG_NONE);

    setup();
    loop();
}
