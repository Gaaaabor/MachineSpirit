// DeviceAttachment.h
#ifndef DeviceAttachment_h
#define DeviceAttachment_h

#include <Arduino.h>

class DeviceAttachment
{
  public:
    String Id;
    String OwnerUserId;
    String OwnerDeviceSerial;
    String AttachmentName;
    String AttachmentSerial;
    float MeasureState;
    bool SwitchState;
    float DimState;
    byte Capability;
    byte PowerPin;
    byte AnalogInPin;

    DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String attachmentSerial, byte capability, byte powerPin);
    float Measure();
    bool Toggle(bool value);
    float Dim(float value);
};
#endif
