#include <driver/ledc.h>
#include <driver/gpio.h>

#include "env.h"
#include "base_Buzzer.h"


void init_buzzer(int buzzer_pin, int freq, ledc_channel_config_t *ledc_channel)
{
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