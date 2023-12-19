#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <uri/UriBraces.h>

#include "DeviceModel.h"

const String wifiSsid = "INSERT SSID HERE";
const String wifiPassword = "INSERT PASSWORD HERE";
const String websocketAddress = "INSERT SOCKET ADDRESS HERE";
const String connectionId = "INSERT CONNECTION ID HERE";
const uint16_t websocketPort = 80;
bool isWebsocketConnected = false;
bool isServerStarted = false;
unsigned long previousMillis = 0;
const long interval = 5000;

WebSocketsClient webSocketClient;
DeviceService deviceService(webSocketClient);
DeviceModel deviceModel(deviceService);
ESP8266WebServer server(80);

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  DynamicJsonDocument doc(2048);

  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[WSC] Disconnected!\n");
    isWebsocketConnected = false;
    deviceModel.IsCreated = false;
    deviceModel.IsConnected = false;
    deviceModel.IsVerified = false;
    break;

  case WStype_CONNECTED:
    Serial.printf("[WSC] Connected to url: %s\n", payload);
    isWebsocketConnected = true;
    break;

  case WStype_TEXT:
    Serial.printf("[WSC] Got text: %u\n", length);
    deserializeJson(doc, payload);
    deviceModel.Tell(doc);
    break;

  case WStype_BIN:
    Serial.printf("[WSC] Got binary length: %u\n", length);
    break;

  case WStype_PING:
    Serial.println("[WSC] Got PING");
    break;

  case WStype_PONG:
    Serial.println("[WSC] Got PONG");
    break;

  case WStype_ERROR:
    Serial.println("[WSC] Got ERROR");
    break;

  case WStype_FRAGMENT_TEXT_START:
    Serial.println("[WSC] Got TEXT START");
    break;

  case WStype_FRAGMENT_BIN_START:
    Serial.println("[WSC] Got BIN START");
    break;

  case WStype_FRAGMENT:
    Serial.println("[WSC] Got FRAGMENT");
    break;

  case WStype_FRAGMENT_FIN:
    Serial.println("[WSC] Got FRAGMENT FIN");
    break;
  }
}

void handle_Verify()
{
  String passphrase = server.pathArg(0);
  Serial.println("Webserver received /verify/" + passphrase);
  server.send(200, "text/html", "Thanks for the passphrase: " + passphrase);

  deviceModel.Verify(passphrase);

  Serial.println("Closing webserver");
  server.close();
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

  webSocketClient.begin(websocketAddress, 80, connectionId);
  webSocketClient.onEvent(webSocketEvent);

  Serial.println("WebSocket connected");

  server.on(UriBraces("/verify/{}"), handle_Verify);
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
  deviceModel.TryVerifyFromRom();
  deviceModel.TryListDeviceAttachments();
}

void loop()
{
  webSocketClient.loop();

  if (isServerStarted)
  {
    server.handleClient();
  }

  if (deviceModel.IsVerificationStarted && !isServerStarted)
  {
    Serial.println("Starting webserver");
    isServerStarted = true;
    server.begin();
  }

  timedStuff();
}