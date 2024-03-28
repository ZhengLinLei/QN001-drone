

#ifndef __BASE_COMM_H__
#define __BASE_COMM_H__

#include <stdint.h>
#include <stddef.h>
#include <driver/uart.h>

/*!
 * @brief Initialize UART
 * 
 * @param[in] uart_num UART number
 * @param[in] baud_rate Baud rate
 * @param[in] tx_pin TX pin
 * @param[in] rx_pin RX pin
 * @param[in] tx_buffer_size TX buffer size
 * @param[in] rx_buffer_size RX buffer size
 * 
 * @return void
*/
void init_uart(uart_port_t uart_num, int baud_rate, int tx_pin, int rx_pin, int tx_buffer_size, int rx_buffer_size);

/*!
 * @brief Send data through UART
 * 
 * @param[in] uart_num UART number
 * @param[in] data Data to send
 * @param[in] len Length of data
 * 
 * @return void
*/
#define send_uart uart_write_bytes

/*!
 * @brief Receive data through UART
 * 
 * @param[in] uart_num UART number
 * @param[out] data Data to receive
 * @param[in] len Length of data
 * @param[in] timeout Timeout
 * 
 * @return int Length of received data
*/
#define receive_uart uart_read_bytes

#endif // __BASE_COMM_H__