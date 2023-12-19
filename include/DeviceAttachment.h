// DeviceAttachment.h
#ifndef DeviceAttachment_h
#define DeviceAttachment_h

#include <Arduino.h>
#include "MeasurementModel.h"

class DeviceAttachment
{
private:
  unsigned long lastMeasureTime;

public:
  String Id;
  String UserId;
  String AttachmentName;
  String AttachmentSerial;
  float MeasureState;
  bool SwitchState;
  float DimState;
  String Capability;
  int PowerPin;
  unsigned long MeasurementFrequency;
  String Type;

  DeviceAttachment(
      String id,
      String userId,
      String attachmentName,
      String attachmentSerial,
      String capability,
      int powerPin,
      unsigned long measurementFrequency,
      String type);

  MeasurementModel Measure();
  bool Switch(bool value);
  MeasurementModel Range(float value);
  bool ShouldMeasure();
};
#endif
