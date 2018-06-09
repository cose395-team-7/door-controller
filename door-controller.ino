// door-controller.ino
#include <WiFiEsp.h>
#include <Servo.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

#include "connection_info.h"

#define PIN_WIFI_TX 6
#define PIN_WIFI_RX 7
#define PIN_SERVO   9

#define COMMAND_LEN 4

enum DOOR_STATUS {
  DOOR_CLOSED,
  DOOR_OPENED
};

enum COMMAND {
  COMMAND_NONE,
  COMMAND_OPEN,
  COMMAND_UNKNOWN,
  COMMAND_ERROR
};

const char *ssid = WIFI_SSID;
const char *pswd = WIFI_PSWD;
const char *logicServerIP = LOGIC_SERVER_IP;
const short logicServerPort = LOGIC_SERVER_PORT;
int connectionStatus = 0;

SoftwareSerial wifiSerial(PIN_WIFI_TX, PIN_WIFI_RX);
WiFiEspClient client;
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
  if (!connectionStatus) return;

  switch (fetchCommand()) {
  case COMMAND_NONE: break;
  case COMMAND_OPEN: {
    onOpen();
    break;
  }
  case COMMAND_ERROR: {
    Serial.println("[err] Error about logic server");
    while (true); // problem with logic server
    break;
  }
  case COMMAND_UNKNOWN: {
    alertError("Unknown-Error");
  }
  }
}

void onOpen()
{
  Serial.println("[debug] onOpen()");
  // TODO
}

void alertError(const String msg)
{
  delay(1000);
  Serial.println("[debug] alertError(" + msg + ")");
  client.print("GET /api/ShowError/");
  client.print(msg.c_str());
  client.println(" HTTP/1.1");
  client.println();

  while (client.available() <= 20); // hack
  while (client.available()) client.read();
}

COMMAND fetchCommand()
{
  String comm = "";
  int readn = 0;

  client.println("GET /api/door HTTP/1.1");
  client.println();

  Serial.println("[debug] Fetching command...");
  while (client.available() <= 200); // hack
  readn = client.available();

  for (int i = 0; i < readn - COMMAND_LEN; i++) client.read();
  for (int i = 0; i < COMMAND_LEN; i++) comm += (char)client.read();
  
  if (readn <= 0) {
    return COMMAND_ERROR;    
  }
  
  Serial.print("[debug] Command from logic server : ");
  Serial.println(comm);
  
  if (comm.equals("open")) {
    return COMMAND_OPEN;
  }
  else if (comm.equals("none")) {
    return COMMAND_NONE;
  }
  return COMMAND_UNKNOWN;
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

  Serial.println("[debug] Connecting to the logic server");
  if (client.connect(logicServerIP, logicServerPort)) {
    connectionStatus = 1;
    Serial.println("[debug] Connected to logic server");
  } else {
    Serial.println("[debug] Connection failed");
  }
}
