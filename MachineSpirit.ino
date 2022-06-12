#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include "DeviceAttachment.h"

using namespace websockets;

DeviceAttachment *attachments[9];
int attachmentSlots = 0;

const String ssid = "";
const String password = "";
const String ownerUserId = "";
const String deviceId = "";
const String deviceSerial = "";
const String deviceName = "";
const String serverAddress = "";
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
      String capability = String(doc["Attachments"][i]["Capability"]);
      int powerPin = int(doc["Attachments"][i]["PowerPin"]);
      unsigned long measurementFrequency = (unsigned long)(doc["Attachments"][i]["MeasurementFrequency"]);

      attachments[i] = new DeviceAttachment(id, ownerUserId, deviceSerial, attachmentName, attachmentSerial, capability, powerPin, measurementFrequency);
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

    Serial.println("Deserializing...");

    String id = String(doc["Id"]);
    String attachmentName = String(doc["AttachmentName"]);
    String attachmentSerial = String(doc["Serial"]);
    String capability = String(doc["Capability"]);
    int powerPin = int(doc["PowerPin"]);
    unsigned long measurementFrequency = (unsigned long)(doc["MeasurementFrequency"]);

    attachments[attachmentSlots] = new DeviceAttachment(id, ownerUserId, deviceSerial, attachmentName, attachmentSerial, capability, powerPin, measurementFrequency);

    Serial.println("Added to slot: " + String(attachmentSlots));

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
        Serial.println("Found: " + String(attachments[i]->Capability));

        String capability = attachments[i]->Capability;
        if (capability == "BinarySwitch")
        {
          attachments[i]->Toggle(String(doc["State"]["Value"]) == "True");
        }
        else if (capability == "Dim")
        {
          attachments[i]->Dim(float(doc["State"]["Value"]));
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
  Serial.println("IP address:");
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

  tryMeasure();
  //trySleep(); // Todo: Implement
}

void tryMeasure()
{
  for (int i = 0; i < attachmentSlots; i++)
  {
    if (attachments[i]->ShouldMeasure())
    {
      float measurement = attachments[i]->Measure();
      sendRecordMeasurementRequest(attachments[i]->AttachmentSerial, measurement, "%");
    }
  }
}

void trySleep()
{
  // 1. Check if the sleep frequency is due
  // 2. Sleep
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
  doc["OwnerUserId"] = ownerUserId;
  doc["DeviceSerial"] = deviceSerial;
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
