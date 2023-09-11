// DeviceService.h
#ifndef DeviceService_h
#define DeviceService_h

#include <ArduinoWebsockets.h>

using namespace websockets;

class DeviceService
{
private:
  WebsocketsClient wsClient;

public:
  void Initialize(WebsocketsClient &wsClient);
  void CreateDevice(String &userId, String &deviceId, String &deviceSerial, String &deviceName);
  void ConnectDevice(String &userId, String &deviceId, String &deviceSerial);
  void VerifyDevice(String &userId, String &deviceId);
  void RecordMeasurement(String &userId, String &deviceId, String &deviceAttachmentId, long measurementValue, String &unitCode);
  void RecordRange(String &userId, String &deviceId, String &deviceAttachmentId, long measurementValue);
  void RecordSwitch(String &userId, String &deviceId, String &deviceAttachmentId, bool measurementValue);
  void ListDeviceAttachments(String &userId, String &deviceId);
};
#endif
