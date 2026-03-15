#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

extern "C" {
  #include "user_interface.h"
}

// Configuration
const char* ssid = "Wifi_name";
const char* password = "Wifi_passwd";
const char* serverUrl = "Server_URL";
uint8_t newMac[] = {0x38, 0x18, 0x2B, 0xB2, 0xAE, 0x5C};

unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // Send data every 5 seconds

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Apply Spoofing
  if (wifi_set_macaddr(STATION_IF, newMac)) {
    Serial.println("[Success] MAC Spoofed.");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[Connected] IP: " + WiFi.localIP().toString());
}

void loop() {
  // Send data at regular intervals
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      // Generate random 3-bit data (0 to 7)
      int randomData = random(0, 8);

      Serial.printf("[HTTP] Sending data: %d\n", randomData);

      // Start connection
      http.begin(client, serverUrl);
      http.addHeader("Content-Type", "application/json");

      // Send HTTP POST request with the value
      String httpRequestData = "{\"value\":" + String(randomData) + "}";
      int httpResponseCode = http.POST(httpRequestData);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }
    lastTime = millis();
  }
}
