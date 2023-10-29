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
    DeviceAttachment *deviceAttachments[9];
    DeviceService *deviceService;

    void measure();

public:
    bool IsCreated;
    bool IsConnected;
    bool IsVerificationStarted;
    bool IsVerified;
    bool IsAttachmentsListed;
    DeviceModel(String &userId, String &deviceId, String &deviceSerial, String &deviceName, DeviceService &deviceService);
    void Tell(DynamicJsonDocument &dynamicJsonDocument);
    void TryCreate();
    void TryConnect();
    void TryVerifyFromRom();
    void Verify(String &passphrase);
    void TryListDeviceAttachments();
};
#endif
