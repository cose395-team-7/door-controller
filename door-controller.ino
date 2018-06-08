// door-controller.ino
#include <WiFiEsp.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

#include "wifi_info.h"

#define PIN_WIFI_TX 7
#define PIN_WIFI_RX 6

const char *ssid = SSID;
const char *pswd = PSWD;

SoftwareSerial wifiSerial(PIN_WIFI_TX, PIN_WIFI_RX);
int wifiStatus = WL_IDLE_STATUS;

void setup()
{
  Serial.begin(115200);
  wifiSerial.begin(9600);

  WiFi.init(&wifiSerial);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("[err]  WiFi shield not present");
    while (true);
  }

  WiFi.scanNetworks(); // hack
  while (wifiStatus != WL_CONNECTED) {
    Serial.println("[debug] Attempting to connect to WiFi..");
    wifiStatus = WiFi.begin(ssid, pswd);
  }

  Serial.println("[debug] Now connected to the network");
}

void loop()
{
}
