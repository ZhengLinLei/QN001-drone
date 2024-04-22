#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial ESP8266(D9, D10); // RX, TX
SoftwareSerial UART(D6, D3);     // RX, TX

const char *ssid     = "Oroneta_Network";
const char *password = "LinEsElMasGuapo";
const char *host     = "192.168.0.1";


const int buzzer =  D11;
const int batt   =  A0;

void setup() {
    pinMode(buzzer, OUTPUT);
    Serial.begin(9600);
    ESP8266.begin(9600);
    UART.begin(9600);

    ESP8266.println("AT+RST");
    delay(1000);

    if (ESP8266.find("OK"))
        Serial.println("Module Reset");

    ESP8266.println("AT+CWMODE=1");
    delay(1000);

    String cmd = "AT+CWJAP=\"";
    cmd += ssid;
    cmd += "\",\"";
    cmd += password;
    cmd += "\"";
    ESP8266.println(cmd);
    delay(5000);

    if (ESP8266.find("OK"))
        Serial.println("Connected!");
    else {
        Serial.println("Not connected!");
        return;
    }

    // Send wifi command
    String ssid_pass = ssid + ";" + password + ";";
    int len = ssid_pass.length();

    // 6 digit length fill with 0
    char str[7];
    sprintf(str, "%06d", len);

    UART.println("0000"+str+ssid_pass);
    // Send wake command
    UART.println("00010000040;0;");

    // Get camera server
    String cmd = "AT+CIPSTART=\"TCP\",\"";
    cmd += host;
    cmd += "\",60002";
    ESP8266.println(cmd);

    if (ESP8266.find("OK")) {
        cmd = "AT+CIPSEND=";
        cmd += url.length();
        ESP8266.println(cmd);

        if (ESP8266.find(">")) {
            ESP8266.print(url);
            ESP8266.println("AT+CIPCLOSE");
        }
    }

    delay(1000);

    while (ESP8266.available()) {
        String line = ESP8266.readStringUntil('\r');
        UART.println(line);
    }

    tone(buzzer, 1000); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    delay(1000);        // ...for 1sec


}

void loop() {
    float voltage = analogRead(A6) * (4.92 / 1023.0);
    float gps = {
            39.455109,
            -0.318429
    };
    float altitude = 11.1;
    float speed = 0;
    int mode = 0;
    int mission = 0;

    JsonDocument doc;
    doc["dic"] = "0";
    doc["key"] = "0";
    doc["voltage"] = voltage;
    doc["gps"] = gps;
    doc["altitude"] = altitude;
    doc["speed"] = speed;
    doc["mode"] = mode;
    doc["mission"] = mission;


    // Generate message
    String msg = "AT+SEND=";
    msg = serializeJson(doc, msg);

    // Send message
    ESP8266.println(msg);

    // Wait for response
    while (ESP8266.available()) {
        String line = ESP8266.readStringUntil('\r');
    }

    delay(1000);
}
