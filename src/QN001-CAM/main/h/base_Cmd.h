/*!
 * \file main/h/base_Cmd.h
 * \brief This file contains the declaration of the base command module
 *
 *      Protocol: 
 *          Command:
 *              CCCCNNNNNNMMMMMMM..{N times}
 *              CCCC - Command code
 *              NNNNNN - Command length {Not used for code 0001 and 0002}
 * 
 *          Reponse:
 *              0    - OK
 *             ---------------
 *              1    - Error
 *              2    - Invalid command
*/
#ifndef __BASE_CMD_H__
#define __BASE_CMD_H__

#include <string.h>
#include <driver/uart.h>

#define CMD_CODE_SIZE       4
#define CMD_LENGTH_SIZE     6
#define CMD_DATA_SIZE       54

/*!
 * @brief Base command structure
*/
typedef struct {
    uint8_t* code;                       /*!< Command code */
    uint8_t* length;                     /*!< Command length */
    uint8_t* data;                       /*!< Command data */
} base_cmd_t;

/*!
 * @brief Base command response structure
*/
void init_base_cmd(base_cmd_t* cmd);

/*!
 * @brief Reset base command
 * 
 * @param[in] cmd Base command
*/
void reset_base_cmd(base_cmd_t* cmd);

/*!
 * @brief Free base command
 * 
 * @param[in] cmd Base command
*/
void free_base_cmd(base_cmd_t* cmd);

/*!
 * @brief Init cmd and parse base command
 * 
 * @param[in] cmd Base command
 * @param[in] data Data
*/
void parse_base_cmd(base_cmd_t* cmd, uint8_t* data);


// -----------
// Command codes

#define CODE_WIFI           "0000"   // 0000000022test_wifi;password123;
#define CODE_WAKEUP         "0001"   // 00010000040;0;
#define CODE_END            "0002"   // 0002
#define CODE_SERVER         "0003"   // 0003000014google.com;80;

/*
    Example code

    0000000037JM_limpia_el_baño;LaTienesEnElRouter;
*/






// -----------
// Command functions


/*!
 * @brief Wait for wifi command
 * 
 * @param[in] uart_num UART number
 * @param[out] ssid SSID
 * @param[out] password Password
 * 
 * @return 0 if success, 1 if error
*/
int wait_for_wifi_command(uart_port_t uart_num, uint8_t* ssid, uint8_t* password);

/*!
 * @brief Wait for wake up command
 * 
 * @param[in] uart_num UART number
 * @param[out] dic Device identification code
 * @param[out] key Key
 * 
 * @return 0 if success, 1 if error
*/
int wait_for_wake_command(uart_port_t uart_num, uint8_t* dic, uint8_t* key);

/*!
 * @brief Wait for server command
 * 
 * @param[in] uart_num UART number
 * @param[out] server Server
 * @param[out] port Port
 * @param[out] intval Interval
 * 
 * @return 0 if success, 1 if error
*/
int wait_for_server_command(uart_port_t uart_num, uint8_t* server, int* port, int* intval);




#endif // __BASE_CMD_H__