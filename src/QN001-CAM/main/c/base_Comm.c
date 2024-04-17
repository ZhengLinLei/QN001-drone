#include <esp_log.h>
#include <driver/uart.h>

#include "env.h"
#include "base_Comm.h"

void init_uart(uart_port_t uart_num, int baud_rate, int tx_pin, int rx_pin, int tx_buffer_size, int rx_buffer_size)
{
    // Configuration of port UART
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, tx_buffer_size, rx_buffer_size, 0, NULL, 0);
}