
#ifndef __BASE_BUZZER_H__
#define __BASE_BUZZER_H__

#include <driver/ledc.h>
#include <driver/gpio.h>

/*!
 * @brief Initialize buzzer
 * 
 * @param[in] buzzer_pin Buzzer pin
 * @param[in] freq Frequency
 * @param[out] ledc_channel LEDC channel configuration
 * 
 * @return void
*/
void init_buzzer(int buzzer_pin, int freq, ledc_channel_config_t* ledc_channel);

#endif // __BASE_BUZZER_H__