// DeviceAttachment.cpp
#include "DeviceAttachment.h"

DeviceAttachment::DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String attachmentSerial, byte capability, byte powerPin)
{
  Id = id;
  OwnerUserId = ownerUserId;
  OwnerDeviceSerial = ownerDeviceSerial;
  AttachmentName = attachmentName;
  AttachmentSerial = attachmentSerial;
  Capability = capability;
  PowerPin = powerPin;

  // Not sure
  AnalogInPin = A0; // ESP8266 Analog Pin ADC0 = A0

  pinMode(PowerPin, OUTPUT);
}

float DeviceAttachment::Measure()
{
  if (Capability != 2) // Capability 2 is Measure
  {
    return 0;
  }

  digitalWrite(PowerPin, HIGH);
  delay(200);
  int analogMeasurement = analogRead(AnalogInPin);
  digitalWrite(PowerPin, LOW);
  MeasureState = (float)analogMeasurement;
  return MeasureState;
}

bool DeviceAttachment::Toggle(bool value)
{
  if (Capability != 0) // Capability 0 is BinarySwitch
  {
    return false;
  }

  Serial.println("Toggle!");

  SwitchState = value;
  if (SwitchState)
  {
    digitalWrite(PowerPin, LOW);
  }
  else
  {
    digitalWrite(PowerPin, HIGH);
  }

  return SwitchState;
}

float DeviceAttachment::Dim(float value)
{
  if (Capability != 1) // Capability 1 is Dim
  {
    return 0;
  }

  if (value > 255)
  {
    value = 255;
  }
  else if (value < 0)
  {
    value = 0;
  }

  analogWrite(PowerPin, value);

  DimState = value;
  return DimState;
}
