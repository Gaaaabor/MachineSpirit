#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

#include "DeviceModel.h"

using namespace websockets;

const String wifiSsid = "PLACEHOLDER";
const String wifiPassword = "PLACEHOLDER";
const String websocketAddress = "PLACEHOLDER";
const uint16_t websocketPort = 80;
bool websocketConnected = false;

// Use WiFiClient class to create TCP connections
WebsocketsClient wsClient;
DeviceService deviceService;
DeviceModel deviceModel("PLACEHOLDER", "PLACEHOLDER", "PLACEHOLDER", "PLACEHOLDER");

void onWebsocketsMessage(WebsocketsMessage websocketsMessage)
{
  String message = websocketsMessage.data();

  Serial.print("Got Message: ");
  Serial.println(message);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  deviceModel.Tell(doc, deviceService);
}

void onWebsocketsEvent(WebsocketsEvent event, String data)
{
  if (event == WebsocketsEvent::ConnectionOpened)
  {
    Serial.println("Connnection Opened");
    websocketConnected = true;
  }
  else if (event == WebsocketsEvent::ConnectionClosed)
  {
    Serial.println("Connnection Closed");
    Serial.println(data);
    websocketConnected = false;
  }
  else if (event == WebsocketsEvent::GotPing)
  {
    Serial.println("Got a Ping!");
  }
  else if (event == WebsocketsEvent::GotPong)
  {
    Serial.println("Got a Pong!");
  }
}

void connectWebsocketsClient()
{
  if (websocketConnected)
  {
    Serial.println("WebSocketClient connected!");
    return;
  }

  Serial.println("Connecting to websocket server");

  while (!wsClient.connect(websocketAddress, websocketPort, "/"))
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("WebSocketClient connected!");
}

void pollWebsocketsClient()
{
  if (wsClient.available())
  {
    wsClient.poll();
  }

  if (!websocketConnected)
  {
    delay(5000);
    connectWebsocketsClient();
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

  // Setup Callbacks
  wsClient.onMessage(onWebsocketsMessage);
  wsClient.onEvent(onWebsocketsEvent);

  connectWebsocketsClient();

  deviceService.Initialize(wsClient);

  delay(2000);

  deviceModel.CreateDevice(deviceService);

  wsClient.ping();
  Serial.println("Ping!");
}

void loop()
{
  pollWebsocketsClient();
}