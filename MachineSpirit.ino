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
const String deviceId = "";
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

  if (messageType == "DeviceCreatedNotification" || messageType == "DeviceAlreadyExistsNotification")
  {
    sendListDeviceAttachmentsRequest();
    return;
  }

  if (messageType == "ListDeviceAttachmentsResponse")
  {
    attachmentSlots = 0;
    int attachmentCount = int(doc["AttachmentCount"]);
    for (int i = 0; i < attachmentCount; i++)
    {
      String id = String(doc["Attachments"][i]["Id"]);
      String attachmentName = String(doc["Attachments"][i]["AttachmentName"]);
      String attachmentSerial = String(doc["Attachments"][i]["Serial"]);
      byte capability = byte(doc["Attachments"][i]["Capability"]);
      byte powerPin = byte(doc["Attachments"][i]["PowerPin"]);

      attachments[i] = new DeviceAttachment(id, ownerUserId, deviceSerial, attachmentName, attachmentSerial, capability, powerPin);
      attachmentSlots++;
    }

    return;
  }

  if (messageType == "DeviceAttachmentCreatedNotification")
  {
    String ownerDeviceId = String(doc["OwnerDeviceId"]);
    if (ownerDeviceId != deviceId || attachmentSlots > 9)
    {
      return;
    }

    Serial.println("Deserializing..");

    String id = String(doc["Id"]);
    String attachmentName = String(doc["AttachmentName"]);
    String attachmentSerial = String(doc["Serial"]);
    byte capability = byte(doc["Capability"]);
    byte powerPin = byte(doc["PowerPin"]);

    attachments[attachmentSlots] = new DeviceAttachment(id, ownerUserId, deviceSerial, attachmentName, attachmentSerial, capability, powerPin);

    Serial.print("Added to slot: ");
    Serial.print(String(attachmentSlots));

    attachmentSlots++;
    return;
  }

  if (messageType == "DeviceAttachmentRemovedNotification")
  {
    String id = String(doc["Id"]);

    // Todo: delete an attachmentfrom the array of attachments

    return;
  }

  if (messageType == "DeviceAttachmentStateChangedNotification")
  {
    String id = String(doc["Id"]);

    for (int i = 0; i < attachmentSlots; i++) {
      if (attachments[i]->Id.equals(id))
      {
        Serial.println("Found!");
        Serial.println(int(attachments[i]->Capability));
        switch (int(attachments[i]->Capability))
        {
          case 0: // BinarySwitch
            attachments[i]->Toggle(String(doc["State"]["Value"]) == "True");
            break;
          case 1: // Dim
            attachments[i]->Dim(float(doc["State"]["Value"]));
            break;
        }
      }
      else
      {
        Serial.println("Mismatch " + attachments[i]->Id + " vs " + id);
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

void sendRegisterDeviceRequest()
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RegisterDeviceRequest";
  doc["Id"] = deviceId;
  doc["OwnerUserId"] = ownerUserId;
  doc["DeviceSerial"] = deviceSerial;
  doc["Name"] = deviceName;

  String message;
  serializeJson(doc, message);
  wsclient.send(message);

  Serial.println("Register Device");
}

void sendListDeviceAttachmentsRequest()
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "ListDeviceAttachmentsRequest";
  doc["OwnerUserId"] = ownerUserId;
  doc["DeviceSerial"] = deviceSerial;

  String message;
  serializeJson(doc, message);
  wsclient.send(message);

  Serial.println("List device attachments");
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
