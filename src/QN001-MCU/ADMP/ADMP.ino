#include <SoftwareSerial.h>

// Configura los pines RX y TX para la comunicación con el ESP8266
SoftwareSerial esp8266(9, 10); // RX, TX

void setup() {
  // Inicia el monitor serial
  Serial.begin(9600);
  
  // Inicia la comunicación con el ESP8266
  esp8266.begin(9600);

  Serial.println("Restart");

  // Conéctate a la red WiFi
  esp8266.println("AT+CWJAP=\"JM_limpia_el_baño\",\"LaTienesEnElRouter\"");
  delay(2000);

  // Lee la respuesta del ESP8266
  while(1) {
    Serial.print(".");
    while (esp8266.available()) {
      String respuesta = esp8266.readString();
      Serial.println(respuesta);

      // Comprueba si la conexión fue exitosa
      if (respuesta.indexOf("WIFI CONNECTED") != -1) {
        Serial.println("Conexión exitosa a la red WiFi.");
      } else if (respuesta.indexOf("WIFI DISCONNECT") != -1) {
        Serial.println("No se pudo conectar a la red WiFi.");
      }
    }
    delay(100);
  }

  // Configura la conexión como una única conexión
  esp8266.println("AT+CIPMUX=0");
  delay(2000);

  // Conéctate al servidor web
  esp8266.println("AT+CIPSTART=\"TCP\",\"www.tuweb.com\",80");
  delay(2000);

  // Envía una solicitud GET
  esp8266.println("AT+CIPSEND=51");
  delay(2000);
  esp8266.println("GET / HTTP/1.1\r\nHost: www.tuweb.com\r\n\r\n");
  delay(2000);

  Serial.println("Web");
}

void loop() {
  // Si hay datos disponibles desde el ESP8266
  if (esp8266.available()) {
    // Imprime los datos en el monitor serial
    Serial.write(esp8266.read());
  }
}
