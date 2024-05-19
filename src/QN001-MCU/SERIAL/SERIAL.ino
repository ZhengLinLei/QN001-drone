/*
    #define CODE_WIFI           "0000"   // 0000000022test_wifi;password123;
    #define CODE_WAKEUP         "0001"   // 00010000040;0;
    #define CODE_END            "0002"   // 0002
    #define CODE_SERVER         "0003"   // 0003000014google.com;80;


    Cunfigurate the RX and TX pins for the communication with the ESP32
    
    And then send the AT commands to the ESP32 to connect to the WiFi network and then to the server
*/

#define CODE_WIFI           "0000"   // 0000000022test_wifi;password123;
#define CODE_WAKEUP         "0001"   // 00010000040;0;
#define CODE_END            "0002"   // 0002
#define CODE_SERVER         "0003"   // 0003000014google.com;80;


#define RX_PIN              9
#define TX_PIN              10

#define DRONE_INFO         "0001000020ESP00002-123-0033;1;"

#define UART_NUM_0          UART_NUM_0
#define UART_BAUDRATE       115200
#define UART_BUFFER_SIZE    1024

#define SERVER_IP           "156.67.26.182"
#define SERVER_PORT         "60003"

#define WIFI_SSID           "Test_Wifi"
#define WIFI_PASS           "password123"

#define DATA_MAX_SIZE       500

#define TIMEOUT_MILLIS      10000


// ---------------------
#include <string.h>
#include <SoftwareSerial.h>

SoftwareSerial esp32(RX_PIN, TX_PIN); // RX, TX

// Reset function
void(* resetFunc) (void) = 0;

int checkResponse(String response) {
    // If response == "2" then exit and restart
    // if response == "1" then redo the command
    // if response == "0" then continue

    if (response == "2") {
        // Restart the arduino
        resetFunc();
    } else if (response == "1") {
        return 1;
    } else {
        return 0;
    }
}


String getRespose() {
    // Wait until get response or timeout
    unsigned long start = millis();
    while (!esp32.available()) { // ESP32
        if (millis() - start > TIMEOUT_MILLIS) {
            return "Timeout";
        }

        delay(100);
        if (esp32.available()) { // ESP32
            break;
        }

    }
ESP00002-123-0033;1;
    // Receive data
    String response = esp32.readString(); // ESP32

    return response;
}

void setup() {
    char data[DATA_MAX_SIZE];
    int iRet;
    String response;


    esp32.begin(UART_BAUDRATE);
    Serial.begin(UART_BAUDRATE);

    // Send 0000${LENGTH}${SSID};${PASS};
    do {
        memset(data, '\0', DATA_MAX_SIZE);
        sprintf(data, "%s%06d%s;%s;", CODE_WIFI, sizeof(WIFI_SSID) + sizeof(WIFI_PASS) + 2, WIFI_SSID, WIFI_PASS);
        esp32.print(data); // ESP32
        Serial.println(String("WIFI ") + data);

        delay(200);

        // Check data received
        String response = getRespose();
        Serial.println(String("WIFI R ") + response);

        // Check if timeout send 0002 and reset
        if (response == "Timeout") {
            esp32.print(CODE_END); // ESP32
            delay(200);
            resetFunc();
        }
    }
    while (checkResponse(response) == 1);


    // Send 0001000020ESP00002-123-0033;1;
    do {
        esp32.print(DRONE_INFO); // ESP32
        Serial.println(String("DIC ") + DRONE_INFO);
        delay(200);

        /// Check data received
        String response = getRespose();
        Serial.println(String("DIC R ") + response);

        // Check if timeout send 0002 and reset
        if (response == "Timeout") {
            esp32.print(CODE_END); // ESP32
            delay(200);
            resetFunc();
        }
    }
    while (checkResponse(response) == 1);

    // Send 0003${LENGTH}${SERVER};${PORT};
    do {
        memset(data, '\0', DATA_MAX_SIZE);
        sprintf(data, "%s%06d%s;%s;", CODE_SERVER, sizeof(SERVER_IP) + sizeof(SERVER_PORT) + 2, SERVER_IP, SERVER_PORT);
        esp32.print(data); // ESP32
        Serial.println(String("SERVER ") + data);

        delay(200);

        /// Check data received
        String response = getRespose();
        Serial.println(String("SERVER R ") + response);

        // Check if timeout send 0002 and reset
        if (response == "Timeout") {
            esp32.print(CODE_END); // ESP32
            delay(200);
            resetFunc();
        }
    }
    while (checkResponse(response) == 1);

}

void loop() {
    // Check if receive 2
    // If receive 2 then restart
    while (esp32.available()) {
        String response = esp32.readString();
        Serial.println(response);

        if (response == "2") {
            // Restart the arduino
            resetFunc();
        }
    }
    delay(100);
}
