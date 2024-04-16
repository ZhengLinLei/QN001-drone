#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <driver/gpio.h>

#include "env.h"
#include "base_Buzzer.h"
#include "base_Comm.h"
#include "server_Util.h"

void exit_t(ledc_channel_config_t ledc_channel)
{
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