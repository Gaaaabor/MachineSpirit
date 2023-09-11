// DeviceService.cpp
#include "DeviceService.h"

#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

using namespace websockets;

void DeviceService::Initialize(WebsocketsClient &wsClient)
{
  wsClient = wsClient;
}

void DeviceService::CreateDevice(String &userId, String &deviceId, String &deviceSerial, String &deviceName)
{
  Serial.println("Creating device");

  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "CreateDeviceCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["DeviceSerial"] = deviceSerial;
  doc["Name"] = deviceName;

  String message;
  serializeJson(doc, message);
  wsClient.send(message);  

  Serial.println("Creation sent:");
  Serial.println(message);
}

void DeviceService::ConnectDevice(String &userId, String &deviceId, String &deviceSerial)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "ConnectToDeviceCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["DeviceSerial"] = deviceSerial;

  String message;
  serializeJson(doc, message);
  wsClient.send(message);

  Serial.println("Connecting device");
}

void DeviceService::VerifyDevice(String &userId, String &deviceId)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "DeviceVerificationCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["Passphrase"] = "pass";

  String message;
  serializeJson(doc, message);
  wsClient.send(message);

  Serial.println("Verifing device");
}

void DeviceService::RecordMeasurement(String &userId, String &deviceId, String &deviceAttachmentId, long measurementValue, String &unitCode)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RecordMeasurementCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["DeviceAttachmentId"] = deviceAttachmentId;
  doc["Value"] = measurementValue;
  doc["UnitCode"] = unitCode;

  String message;
  serializeJson(doc, message);
  wsClient.send(message);

  Serial.println("Sending RecordMeasurementCommand");
}

void DeviceService::RecordRange(String &userId, String &deviceId, String &deviceAttachmentId, long measurementValue)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RecordRangeCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["DeviceAttachmentId"] = deviceAttachmentId;
  doc["Value"] = measurementValue;

  String message;
  serializeJson(doc, message);
  wsClient.send(message);

  Serial.println("Sending RecordRangeCommand");
}

void DeviceService::RecordSwitch(String &userId, String &deviceId, String &deviceAttachmentId, bool measurementValue)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RecordSwitchCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["DeviceAttachmentId"] = deviceAttachmentId;
  doc["Value"] = measurementValue;

  String message;
  serializeJson(doc, message);
  wsClient.send(message);

  Serial.println("Sending RecordSwitchCommand");
}

void DeviceService::ListDeviceAttachments(String &userId, String &deviceId)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "ListDeviceAttachmentsQuery";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;

  String message;
  serializeJson(doc, message);
  wsClient.send(message);

  Serial.println("Sending ListDeviceAttachmentsQuery");
}
