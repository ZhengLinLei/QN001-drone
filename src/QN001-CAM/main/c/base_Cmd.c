#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <driver/uart.h>


#include "env.h"
#include "base_Cmd.h"
#include "base_Comm.h"

void init_base_cmd(base_cmd_t* cmd) {
    cmd->code = (uint8_t*) malloc(CMD_CODE_SIZE + 1);
    cmd->length = (uint8_t*) malloc(CMD_LENGTH_SIZE + 1);
    cmd->data = (uint8_t*) malloc(CMD_DATA_SIZE + 1);
}

void reset_base_cmd(base_cmd_t* cmd) {
    memset(cmd->code, '\0', CMD_CODE_SIZE + 1);
    memset(cmd->length, '\0', CMD_LENGTH_SIZE + 1);
    memset(cmd->data, '\0', CMD_DATA_SIZE + 1);
}

void free_base_cmd(base_cmd_t* cmd) {
    free(cmd->code);
    free(cmd->length);
    free(cmd->data);
}

void parse_base_cmd(base_cmd_t* cmd, uint8_t* data) {
    init_base_cmd(cmd);
    // Copy code
    memcpy(cmd->code, data, CMD_CODE_SIZE);
    // Copy length
    memcpy(cmd->length, data + CMD_CODE_SIZE, CMD_LENGTH_SIZE);
    // Copy data
    memcpy(cmd->data, data + CMD_CODE_SIZE + CMD_LENGTH_SIZE, atoi((char *)cmd->length));
}

// ---------------------------------------------------------------------------

int wait_for_wifi_command(uart_port_t uart_num, uint8_t* ssid, uint8_t* password) 
{
    base_cmd_t cmd;
    int i;
    uint8_t* tok;
    // Init
    init_base_cmd(&cmd);
    reset_base_cmd(&cmd);
    // Enter sleep mode until wake command is received
    while (1) {
        // Receive data
        i = receive_uart(uart_num, cmd.code, CMD_CODE_SIZE, pdMS_TO_TICKS(1000));
        
        // Check if wake command is received
        if (i > 0) {            
            // Check if wake command is received
            if (strcmp((const char *)cmd.code, CODE_WIFI) == 0) {
                // Receive length
                i = receive_uart(uart_num, cmd.length, CMD_LENGTH_SIZE, pdMS_TO_TICKS(1000));
                // Receive data
                i = receive_uart(uart_num, cmd.data, atoi((char *)cmd.length), pdMS_TO_TICKS(1000));

#ifdef VERBOSE                
                // Print received data
                printf("Code: %s\n", cmd.code);
                printf("Length: %s\n", cmd.length);
                printf("Data: %s\n", cmd.data);
#endif

                // Parse data
                tok = (uint8_t *) strtok((char *)cmd.data, ";");
                memcpy(ssid, tok, strlen((const char *)tok));

                tok = (uint8_t *) strtok(NULL, ";");
                memcpy(password, tok, strlen((const char *)tok));

                send_uart(uart_num, "0", 2);

                // Free memory
                free_base_cmd(&cmd);

                return 0;
            } else {
                send_uart(uart_num, "1", 2);
            }
        } // End if
    } // End while
    return 0;
} 

int wait_for_wake_command(uart_port_t uart_num) 
{
    base_cmd_t cmd;
    int i;
    // Init
    init_base_cmd(&cmd);
    reset_base_cmd(&cmd);
    // Enter sleep mode until wake command is received
    while (1) {
        // Receive data
        i = receive_uart(uart_num, cmd.code, CMD_CODE_SIZE, pdMS_TO_TICKS(1000));
        
        // Check if wake command is received
        if (i > 0) {            
            // Check if wake command is received
            if (strcmp((const char *)cmd.code, CODE_WAKEUP) == 0) {

                send_uart(uart_num, "0", 1);

                // Free memory
                free_base_cmd(&cmd);

                return 0;
            } else {
                send_uart(uart_num, "1", 1);
            }
        } // End if
    } // End while
    return 0;
}
