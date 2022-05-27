// DeviceAttachment.h
#ifndef DeviceAttachment_h
#define DeviceAttachment_h

#include <Arduino.h>

class DeviceAttachment
{
  public:
    String id;
    String ownerUserId;
    String ownerDeviceSerial;
    String attachmentName;
    String serial;
    float measureState;
    bool switchState;
    float dimState;
    byte capability;
    byte powerPin;
    byte analogInPin;

    DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String serial, byte capability, byte powerPin);
    float measure();
    bool toggle(bool value);
    float dim(float value);
};
#endif
