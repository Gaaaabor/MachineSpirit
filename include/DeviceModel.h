// DeviceModel.h
#ifndef DeviceModel_h
#define DeviceModel_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "DeviceAttachment.h"
#include "DeviceService.h"

class DeviceModel
{
private:
    String userId;
    String deviceId;
    String deviceSerial;
    String deviceName;
    int attachmentSlots;
    DeviceAttachment *attachments[9];

    void tryMeasure(DeviceService &deviceService);

public:
    bool IsCreated;
    bool IsConnected;
    DeviceModel(String &userId, String &deviceId, String &deviceSerial, String &deviceName);
    void Tell(DynamicJsonDocument &dynamicJsonDocument);
};
#endif
