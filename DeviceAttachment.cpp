// DeviceAttachment.cpp
#include "DeviceAttachment.h"

DeviceAttachment::DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String serial, byte capability, byte powerPin)
{
  id = id;
  ownerUserId = ownerUserId;
  ownerDeviceSerial = ownerDeviceSerial;
  attachmentName = attachmentName;
  serial = serial;
  capability = capability;
  powerPin = powerPin;

  // Not sure
  analogInPin = A0; // ESP8266 Analog Pin ADC0 = A0

  pinMode(powerPin, OUTPUT);
}

float DeviceAttachment::measure()
{
  if (capability != 2) // Capability 2 is Measure
  {
    return 0;
  }

  digitalWrite(powerPin, HIGH);
  delay(200);
  int analogMeasurement = analogRead(analogInPin);
  digitalWrite(powerPin, LOW);
  measureState = (float)analogMeasurement;
  return measureState;
}

bool DeviceAttachment::toggle(bool value)
{
  if (capability != 0) // Capability 0 is BinarySwitch
  {
    return false;
  }

  switchState = value;
  if (switchState)
  {
    digitalWrite(powerPin, LOW);
  }
  else
  {
    digitalWrite(powerPin, HIGH);
  }

  return switchState;
}

float DeviceAttachment::dim(float value)
{
  if (capability != 1) // Capability 1 is Dim
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

  analogWrite(powerPin, value);

  dimState = value;
  return dimState;
}
