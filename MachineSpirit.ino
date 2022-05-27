#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include "DeviceAttachment.h"

using namespace websockets;

DeviceAttachment *attachments[10];
int attachmentSlots = 0;

const char *ssid = "";
const char *password = "";
const String ownerUserId = "";
const String deviceSerial = "";
const String deviceName = "";
const char *serverAddress = "";
const uint16_t serverPort = 80;
bool websocketConnected = false;

// Use WiFiClient class to create TCP connections
WebsocketsClient wsclient;

void onMessageCallback(WebsocketsMessage websocketsMessage)
{
  String message = websocketsMessage.data();

  Serial.print("Got Message: ");
  Serial.println(message);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  String messageType = String(doc["MessageType"]);

  if (messageType == "AttachmentCreatedNotification")
  {
    String ownerDeviceSerial = String(doc["OwnerDeviceSerial"]);
    if (ownerDeviceSerial != deviceSerial || attachmentSlots > 9)
    {
      return;
    }

    String id = String(doc["Id"]);
    String attachmentName = String(doc["AttachmentName"]);
    String serial = String(doc["Serial"]);
    byte capability = byte(doc["Capability"]);
    byte powerPin = byte(doc["PowerPin"]);

    //DeviceAttachment attachment = new DeviceAttachment(id, ownerUserId, deviceSerial, attachmentName, serial, capability, powerPin);
    attachments[0] = new DeviceAttachment(id, ownerUserId, deviceSerial, attachmentName, serial, capability, powerPin);
    attachmentSlots++;

    // Todo: state!
    String state = String(doc["State"]);
    return;
  }

  if (messageType == "AttachmentRemovedNotification")
  {
    String id = String(doc["Id"]);

    // Todo: delete an attachmentfrom the array of attachments

    return;
  }

  if (messageType == "AttachmentStateChangedNotification")
  {
    String id = String(doc["Id"]);
    // Todo: Get the attachment from the array by id
    for (int i = 0; i < attachmentSlots; i++) {
      if (attachments[i]->id == id)
      {
        switch (int(attachments[i]->capability))
        {
          case 0: // BinarySwitch
            attachments[i]->toggle(String(doc["State"]) == "True");
            break;
          case 1: // Dim
            attachments[i]->dim(float(doc["State"]));
            break;
            //case 2: // Measure should not set, because its a read method
        }
      }
    }

    return;
  }
}

void onEventsCallback(WebsocketsEvent event, String data)
{
  if (event == WebsocketsEvent::ConnectionOpened)
  {
    Serial.println("Connnection Opened");
    websocketConnected = true;
  }
  else if (event == WebsocketsEvent::ConnectionClosed)
  {
    Serial.println("Connnection Closed");
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

void setup()
{
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup Callbacks
  wsclient.onMessage(onMessageCallback);
  wsclient.onEvent(onEventsCallback);

  connect();

  // Send RegisterDevice command
  sendRegisterDeviceRequest();
}

void connect()
{
  if (websocketConnected)
  {
    Serial.println("WebSocketClient connected!");
    return;
  }

  // Connect to websocket server
  Serial.println("Connecting to websocket server");
  while (!wsclient.connect(serverAddress, serverPort, "/deviceconnection"))
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WebSocketClient connected!");
}

void loop()
{
  if (wsclient.available())
  {
    wsclient.poll();
  }

  if (!websocketConnected)
  {
    delay(5000);
    connect();
  }
}

// Todo: Cleanup these
void sendRegisterDeviceRequest()
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RegisterDeviceRequest";
  doc["OwnerUserId"] = ownerUserId;
  doc["Serial"] = deviceSerial;
  doc["Name"] = deviceName;

  String message;
  serializeJson(doc, message);
  wsclient.send(message);

  Serial.println("Register Device");
}

void sendRecordMeasurementRequest(String attachmentSerial, long measurementValue, String unitCode)
{
  String mvalue = String(measurementValue);

  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RecordMeasurementRequest";
  doc["Serial"] = attachmentSerial;
  doc["Value"] = mvalue;
  doc["UnitCode"] = unitCode;

  String message;
  serializeJson(doc, message);
  wsclient.send(message);

  Serial.println("Sent RecordMeasurementRequest: " + mvalue);
}

void sendRecordSwitchRequest(String attachmentSerial, String measurementValue)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RecordSwitchRequest";
  doc["Serial"] = attachmentSerial;
  doc["Value"] = measurementValue;

  String message;
  serializeJson(doc, message);
  wsclient.send(message);

  Serial.println("Sent RecordSwitchRequest: " + measurementValue);
}

void sendRecordDimRequest(String attachmentSerial, long measurementValue)
{
  String mvalue = String(measurementValue);

  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RecordDimRequest";
  doc["Serial"] = attachmentSerial;
  doc["Value"] = mvalue;

  String message;
  serializeJson(doc, message);
  wsclient.send(message);

  Serial.println("Sent RecordDimRequest: " + mvalue);
}
