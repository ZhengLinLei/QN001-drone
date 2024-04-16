#include <driver/gpio.h>
#include <driver/ledc.h>
#include "esp_camera.h"
#include <esp_http_client.h>

#include "env.h"
#include "base_Cam.h"

void take_and_send(uint8_t* dic, uint8_t* key, uint8_t* server, int port, int intval)
{
    // Initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        printf("Error al inicializar la cámara\n");
        return;
    }

    // Take pic
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        printf("Error al tomar la foto\n");
        return;
    }

    // Inicializar el cliente HTTP
    esp_http_client_handle_t http_client = esp_http_client_init(&http_config);

    // Establecer los encabezados HTTP necesarios
    esp_http_client_set_header(http_client, "Content-Type", "image/jpeg");

    // Enviar la imagen al servidor
    esp_http_client_open(http_client, fb->len);
    esp_http_client_write(http_client, (const char *)fb->buf, fb->len);

    // Limpiar
    esp_http_client_cleanup(http_client);
    esp_camera_fb_return(fb);
}