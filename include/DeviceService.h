// DeviceService.h
#ifndef DeviceService_h
#define DeviceService_h

#include "MeasurementModel.h"
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
  void RecordMeasurement(String &requestId, String &userId, String &deviceId, String &deviceAttachmentId, MeasurementModel &measurement);
  void RecordRange(String &requestId, String &userId, String &deviceId, String &deviceAttachmentId, MeasurementModel &measurement);
  void RecordSwitch(String &requestId, String &userId, String &deviceId, String &deviceAttachmentId, bool value);
  void ListDeviceAttachments(String &userId, String &deviceId);
};
#endif
