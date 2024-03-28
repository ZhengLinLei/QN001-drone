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
    printf("SSID: %s\n", ssid);
    printf("Password: %s\n", password);
}

void loop() {
    while (1) {
        // Wait for wake command
        wait_for_wake_command(UART_NUM_0);

        // Turn on alarm light for 10 miliseconds
        gpio_set_level(LED_ALARM_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(LED_ALARM_PIN, 0);


        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    esp_log_level_set("*", ESP_LOG_NONE);

    setup();
    loop();
}
