

#ifndef __SERVER_UTIL__
#define __SERVER_UTIL__

#include <stdint.h>
#include <stddef.h>
#include <driver/uart.h>
#include <driver/ledc.h>

/*!
* @brief Exit program and restart
*
* @param[in] status Status
*
* @return void
*/
void exit_t(ledc_channel_config_t ledc_channel);

#endif // __BASE_COMM_H__