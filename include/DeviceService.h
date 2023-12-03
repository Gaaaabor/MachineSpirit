// DeviceService.h
#ifndef DeviceService_h
#define DeviceService_h

#include <WebSocketsClient.h>

class DeviceService
{
private:
  WebSocketsClient *webSocketsClient;

public:
  DeviceService(WebSocketsClient &webSocketClient);
  void CreateDevice(String &userId, String &deviceId, String &deviceSerial, String &deviceName);
  void ConnectDevice(String &userId, String &deviceId, String &deviceSerial);
  void VerifyDevice(String &userId, String &deviceId, String &passphrase);
  void RecordMeasurement(String &userId, String &deviceId, String &deviceAttachmentId, float measurementValue, String &unitCode, String &requestId);
  void RecordRange(String &userId, String &deviceId, String &deviceAttachmentId, float measurementValue, String &requestId);
  void RecordSwitch(String &userId, String &deviceId, String &deviceAttachmentId, bool measurementValue, String &requestId);
  void ListDeviceAttachments(String &userId, String &deviceId);
};
#endif
