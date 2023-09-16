#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "DeviceModel.h"

const String wifiSsid = "PLACEHOLDER";
const String wifiPassword = "PLACEHOLDER";
const String websocketAddress = "PLACEHOLDER";
const uint16_t websocketPort = 80;
bool isWebsocketConnected = false;
unsigned long previousMillis = 0;
const long interval = 5000;

String userId = "PLACEHOLDER";
String deviceId = "PLACEHOLDER";
String deviceSerial = "PLACEHOLDER";
String deviceName = "PLACEHOLDER";

WebSocketsClient webSocketClient;
DeviceService deviceService(webSocketClient);
DeviceModel deviceModel(userId, deviceId, deviceSerial, deviceName, deviceService);

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  DynamicJsonDocument doc(1024);

  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[WSc] Disconnected!\n");
    isWebsocketConnected = false;
    deviceModel.IsCreated = false;
    deviceModel.IsConnected = false;
    break;

  case WStype_CONNECTED:
    Serial.printf("[WSc] Connected to url: %s\n", payload);
    isWebsocketConnected = true;
    break;

  case WStype_TEXT:
    Serial.printf("[WSc] got text: %s\n", payload);
    deserializeJson(doc, payload);
    deviceModel.Tell(doc);
    break;

  case WStype_BIN:
    Serial.printf("[WSc] got binary length: %u\n", length);
    break;

  case WStype_PING:
    Serial.println("[WSc] got PING");
    break;

  case WStype_PONG:
    Serial.println("[WSc] got PONG");
    break;

  case WStype_ERROR:
    Serial.println("[WSc] got ERROR");
    break;

  case WStype_FRAGMENT_TEXT_START:
    Serial.println("[WSc] got TEXT START");
    break;

  case WStype_FRAGMENT_BIN_START:
    Serial.println("[WSc] got BIN START");
    break;

  case WStype_FRAGMENT:
    Serial.println("[WSc] got FRAGMENT");
    break;

  case WStype_FRAGMENT_FIN:
    Serial.println("[WSc] got FRAGMENT FIN");
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(wifiSsid);

  WiFi.begin(wifiSsid, wifiPassword);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());

  webSocketClient.begin("devices.cloudpie.dev", 80, "/");
  webSocketClient.onEvent(webSocketEvent);

  Serial.println("WebSocket connected");
}

void timedStuff()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < interval)
  {
    return;
  }

  Serial.println("timedStuff triggered!");
  previousMillis = currentMillis;

  if (!isWebsocketConnected)
  {
    Serial.println("WebSocket not connected");
    return;
  }

  deviceModel.TryCreate();
  deviceModel.TryConnect();
}

void loop()
{
  webSocketClient.loop();
  timedStuff();
}