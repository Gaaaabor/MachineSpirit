// DeviceAttachment.h
#ifndef DeviceAttachment_h
#define DeviceAttachment_h

#include <Arduino.h>

class DeviceAttachment
{
  private:
    unsigned long lastMeasureTime;
  public:
    String Id;
    String OwnerUserId;
    String OwnerDeviceSerial;
    String AttachmentName;
    String AttachmentSerial;
    float MeasureState;
    bool SwitchState;
    float DimState;
    String Capability;
    byte PowerPin;    
    unsigned long MeasurementFrequency;

    DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String attachmentSerial, String capability, byte powerPin, unsigned long measurementFrequency);
    float Measure();
    bool Toggle(bool value);
    float Dim(float value);
    bool ShouldMeasure();
};
#endif
