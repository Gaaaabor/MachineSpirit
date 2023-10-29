// DeviceService.cpp
#include "DeviceService.h"

#include <ArduinoJson.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

DeviceService::DeviceService(WebSocketsClient &webSocketsClient)
{
  this->webSocketsClient = &webSocketsClient;
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

  Serial.println(message);

  webSocketsClient->sendTXT(message);
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

  Serial.println(message);

  webSocketsClient->sendTXT(message);
}

void DeviceService::VerifyDevice(String &userId, String &deviceId, String &passphrase)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "DeviceVerificationCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["Passphrase"] = passphrase;

  String message;
  serializeJson(doc, message);

  Serial.println(message);

  webSocketsClient->sendTXT(message);
}

void DeviceService::RecordMeasurement(String &userId, String &deviceId, String &deviceAttachmentId, float measurementValue, String &unitCode)
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

  Serial.println(message);

  webSocketsClient->sendTXT(message);
}

void DeviceService::RecordRange(String &userId, String &deviceId, String &deviceAttachmentId, float measurementValue)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "RecordRangeCommand";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;
  doc["DeviceAttachmentId"] = deviceAttachmentId;
  doc["Value"] = measurementValue;

  String message;
  serializeJson(doc, message);

  Serial.println(message);

  webSocketsClient->sendTXT(message);
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

  Serial.println(message);

  webSocketsClient->sendTXT(message);
}

void DeviceService::ListDeviceAttachments(String &userId, String &deviceId)
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "ListDeviceAttachmentsHardwareQuery";
  doc["UserId"] = userId;
  doc["DeviceId"] = deviceId;

  String message;
  serializeJson(doc, message);

  Serial.println(message);

  webSocketsClient->sendTXT(message);
}
