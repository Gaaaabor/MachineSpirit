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
    String userId = "INSERT ID HERE";
    String deviceId = "INSERT ID HERE";
    String deviceSerial = "INSERT SERIAL HERE";
    String deviceName = "INSERT NAME HERE";

    int attachmentSlots;
    DeviceAttachment *deviceAttachments[9];
    DeviceService *deviceService;

    void _Measure();

public:
    bool IsCreated;
    bool IsConnected;
    bool IsVerificationStarted;
    bool IsVerified;
    bool IsAttachmentsListed;
    DeviceModel(DeviceService &deviceService);
    void Tell(DynamicJsonDocument &dynamicJsonDocument);
    void TryCreate();
    void TryConnect();
    void TryVerifyFromRom();
    void Verify(String &passphrase);
    void TryListDeviceAttachments();
};
#endif
