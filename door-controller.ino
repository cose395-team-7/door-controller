// door-controller.ino
#include <WiFiEsp.h>
#include <Servo.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

#include "connection_info.h"

#define PIN_WIFI_TX 6
#define PIN_WIFI_RX 7
#define PIN_SERVO   9

enum DOOR_STATUS {
  DOOR_CLOSED,
  DOOR_OPENED
};

enum COMMAND {
  COMMAND_NONE,
  COMMAND_CLOSE,
  COMMAND_OPEN
};

const char *ssid = WIFI_SSID;
const char *pswd = WIFI_PSWD;
const char *logicServer = LOGIC_SERVER;

SoftwareSerial wifiSerial(PIN_WIFI_TX, PIN_WIFI_RX);
int wifiStatus = WL_IDLE_STATUS;

Servo servo;

void setup()
{
  Serial.begin(115200);
  servo.attach(PIN_SERVO);
  setupNetwork();
}

void loop()
{
  switch (fetchCommand()) {
  case COMMAND_NONE: break;
  case COMMAND_CLOSE: {
    onClose(); 
  }
  case COMMAND_OPEN: {
    onOpen();
  }
  default: {
    alertError("Unknown Error");
  }
  }
}

void onClose()
{
}

void onOpen()
{
}

void alertError(const char* msg)
{
}

COMMAND fetchCommand()
{
  return COMMAND_OPEN;
}

void setupNetwork()
{
  wifiSerial.begin(9600);

  WiFi.init(&wifiSerial);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("[err]  WiFi shield not present");
    while (true);
  }

  while (wifiStatus != WL_CONNECTED) {
    Serial.println("[debug] Attempting to connect to WiFi..");
    wifiStatus = WiFi.begin(ssid, pswd);
  }

  Serial.println("[debug] Now connected to the network");
  Serial.print("[debug] IP : ");
  Serial.println(WiFi.localIP());
}
