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
    String UserId;
    String DeviceSerial;
    String AttachmentName;
    String AttachmentSerial;
    float MeasureState;
    bool SwitchState;
    float DimState;
    String Capability;
    int PowerPin;
    unsigned long MeasurementFrequency;

    DeviceAttachment(String id, String userId, String deviceSerial, String attachmentName, String attachmentSerial, String capability, int powerPin, unsigned long measurementFrequency);    
    float Measure();
    bool Switch(bool value);
    float Range(float value);
    bool ShouldMeasure();
};
#endif
