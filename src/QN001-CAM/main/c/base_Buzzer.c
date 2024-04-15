#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <driver/gpio.h>

#include "env.h"
#include "base_Buzzer.h"


void init_buzzer(int buzzer_pin, int freq, ledc_channel_config_t *ledc_channel)
{
    // Configure the buzzer pin as output
    esp_rom_gpio_pad_select_gpio(buzzer_pin);
    gpio_set_direction(buzzer_pin, GPIO_MODE_OUTPUT);

    // Configuration of port LEDC
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = freq,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };

    ledc_timer_config(&ledc_timer);

    ledc_channel->channel = LEDC_CHANNEL_0; 
    ledc_channel->duty = 0;
    ledc_channel->gpio_num = buzzer_pin;
    ledc_channel->speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel->timer_sel = LEDC_TIMER_0;

    ledc_channel_config(ledc_channel);
}


void wake_sound(ledc_channel_config_t *ledc_channel)
{
    ledc_set_duty(ledc_channel->speed_mode, ledc_channel->channel, 512);
    ledc_update_duty(ledc_channel->speed_mode, ledc_channel->channel);
}

void alarm_sound(ledc_channel_config_t *ledc_channel)
{
    ledc_set_duty(ledc_channel->speed_mode, ledc_channel->channel, 512);
    ledc_update_duty(ledc_channel->speed_mode, ledc_channel->channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    ledc_set_duty(ledc_channel->speed_mode, ledc_channel->channel, 2012);
    ledc_update_duty(ledc_channel->speed_mode, ledc_channel->channel);
}

void stop_sound(ledc_channel_config_t *ledc_channel)
{
    ledc_set_duty(ledc_channel->speed_mode, ledc_channel->channel, 0);
    ledc_update_duty(ledc_channel->speed_mode, ledc_channel->channel);
}