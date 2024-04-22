#include <stdio.h>

// IDF headers
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_system.h>
#include <esp_http_client.h>
#include <esp_psram.h>

// Project headers
#include "env.h"
#include "base_Comm.h"
#include "base_Buzzer.h"
#include "base_Cmd.h"
#include "base_Wifi.h"
#include "include_Cam.h"
#include "server_Util.h"


// Constants
ledc_channel_config_t ledc_channel;


// Definitions
void setup();
void loop();
void app_main();


// Code
void setup() {
    int iRet;


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
    iRet = wait_for_wifi_command(UART_NUM_0, ssid, password);

    if (iRet == -1) {
        exit_t(ledc_channel);
    }

#ifdef VERBOSE                
    printf("SSID: %s\n", ssid);
    printf("Password: %s\n", password);
#endif

    // Emit wake sound
    wake_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    wake_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(&ledc_channel);

    // Connect to WiFi
    iRet = wifi_connect(ssid, password);

    if (iRet == 1 || iRet == -1) {
        exit_t(ledc_channel);
    }

    send_uart(UART_NUM_0, "0", 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    alarm_sound(&ledc_channel);
    vTaskDelay(pdMS_TO_TICKS(100));
    stop_sound(&ledc_channel);
}

void loop() {
    int iRet, tryCap = 0, trySend = 0;

    while (1) {
        uint8_t* dic = (uint8_t*) malloc(64);
        uint8_t* key = (uint8_t*) malloc(64);

        memset(dic, '\0', 64);
        memset(key, '\0', 64);
        // Wait for wake command
        iRet = wait_for_wake_command(UART_NUM_0, dic, key);

        if (iRet == -1) {
            exit_t(ledc_channel);
        }

        uint8_t* server = (uint8_t*) malloc(56);
        int port, intval;

        memset(server, '\0', 56);

        // Wait server command
        iRet = wait_for_server_command(UART_NUM_0, server, &port, &intval);

        if (iRet == -1) {
            exit_t(ledc_channel);
        }

        // Configuración del cliente HTTP
        // esp_http_client_config_t http_config = {
        //     .url = "http://mi-servidor.com/upload",
        // };

#ifdef VERBOSE                
        printf("Server: %s\n", server);
        printf("Port: %d\n", port);
        printf("Interval: %d\n", intval);
#endif
        // Emit wake sound
        wake_sound(&ledc_channel);

        // Turn on alarm light for 10 miliseconds
        gpio_set_level(LED_ALARM_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));

        // Stop wake sound
        stop_sound(&ledc_channel);

        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(LED_ALARM_PIN, 0);



        // -----------------------------
        // Concatenate server and port
        uint8_t* url = (uint8_t*) malloc(64);
        uint8_t* auth = (uint8_t*) malloc(64);
        memset(url, '\0', 64);
        memset(auth, '\0', 64);
        sprintf((char*) url, "http://%s:%d/%s", server, port, dic);
        sprintf((char*) auth, "Bearer %s", key);

        esp_http_client_config_t http_config = {
            .url = (char*) url,
            .method = HTTP_METHOD_POST,
        };

#ifdef VERBOSE
        printf("URL: %s\n", url);
        printf("Auth: %s\n", auth);
#endif

        // -----------------------------
        // Initialize the camera
        // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
        //                      for larger pre-allocated frame buffer.
        if(camera_config.pixel_format == PIXFORMAT_JPEG){
            if(esp_psram_get_size() > 0){
                camera_config.jpeg_quality = 10;
                camera_config.fb_count = 2;
                camera_config.grab_mode = CAMERA_GRAB_LATEST;
            } else {
                // Limit the frame size when PSRAM is not available
                camera_config.frame_size = FRAMESIZE_SVGA;
                camera_config.fb_location = CAMERA_FB_IN_DRAM;
            }
        } else {
            // Best option for face detection/recognition
            camera_config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
            camera_config.fb_count = 2;
#endif
        }
        esp_err_t err = esp_camera_init(&camera_config);
        if (err != ESP_OK) {

#ifdef VERBOSE
            printf("Error activating camera\n");
#endif
            exit_t(ledc_channel);
        }

#ifdef VERBOSE
        printf("Camera activated\n");
#endif
        // Turn on flashlight
        gpio_set_level(LED_LIGHT_PIN, 1);
        while (1) {
            // Check if need to stop
            iRet = check_for_end_command(UART_NUM_0);

            if (iRet == -1) {
                exit_t(ledc_channel);
            }
#ifdef VERBOSE
            printf("Taking picture\n");
#endif

            // Take pic and send
            camera_fb_t *fb = esp_camera_fb_get();
            if (!fb) {
#ifdef VERBOSE
                printf("Error taking picture\n");
#endif
                if (tryCap >= 5) {
                    exit_t(ledc_channel);
                }
                
                tryCap++;
                continue;
            }

 #ifdef VERBOSE
            printf("Picture taken. Length: %d\n", fb->len);
#endif


            // Send to server using HTTP POST
            esp_http_client_handle_t http_client = esp_http_client_init(&http_config);

            char jpegSizeStr[7] = "";
            snprintf(jpegSizeStr, sizeof(jpegSizeStr), "%d", fb->len);


            // Set necessary HTTP headers Content-Type and Authorization with Bearer token
            esp_http_client_set_header(http_client, "Content-Type", "image/jpeg");
            esp_http_client_set_header(http_client, "Authorization", (const char*) auth);
            esp_http_client_set_header(http_client, "Content-Length", (const char*) jpegSizeStr);

#ifdef VERBOSE
            printf("Sending picture\n");
#endif

            // esp_http_client_open(http_client, fb->len);
            esp_http_client_set_method(http_client, HTTP_METHOD_POST);
            // esp_http_client_write(http_client, (const char *)fb->buf, fb->len);
            esp_http_client_set_post_field(http_client, (const char *)fb->buf, fb->len);
            esp_err_t result = esp_http_client_perform(http_client);

            if (result != ESP_OK) {
#ifdef VERBOSE
    printf("Error sending picture: %s\n", esp_err_to_name(result));
#endif
                if (trySend >= 5) {
                    exit_t(ledc_channel);
                }
                
                trySend++;
                continue;
            } 
            
#ifdef VERBOSE
            else {
                printf("Picture sent\n");
            }
#endif

#ifdef VERBOSE
            printf("Picture sent\n");
#endif

            // Read response 
            // if http code 201 nothing to do
            // if http code 200 buzzer alarm

            int http_code = esp_http_client_get_status_code(http_client);
#ifdef VERBOSE
                printf("HTTP Status = %d\n", http_code);
#endif
            if (http_code == 200) {
                // Emit alarm sound 5 times
                for (int i = 0; i < 5; i++) {
                    alarm_sound(&ledc_channel);
                    vTaskDelay(pdMS_TO_TICKS(50));
                    stop_sound(&ledc_channel);
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
            }

            // Clean
            esp_http_client_cleanup(http_client);
            esp_camera_fb_return(fb);


            // ms to ticks
            vTaskDelay(pdMS_TO_TICKS(intval));
        }
        gpio_set_level(LED_LIGHT_PIN, 0);

        // Deinit camera
        esp_camera_deinit();
        //!TODO Remmber to free memory: dic, key, server
        free(dic);
        free(key);
        free(server);
        free(url);
        free(auth);
    }
}

void app_main() {
    esp_log_level_set("*", ESP_LOG_NONE);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    setup();
    loop();
}
