// DeviceAttachment.cpp
#include "DeviceAttachment.h"

DeviceAttachment::DeviceAttachment(String id, String userId, String deviceSerial, String attachmentName, String attachmentSerial, String capability, int powerPin, unsigned long measurementFrequency)
{
  Id = id;
  UserId = userId;
  DeviceSerial = deviceSerial;
  AttachmentName = attachmentName;
  AttachmentSerial = attachmentSerial;
  Capability = capability;
  PowerPin = powerPin;
  MeasurementFrequency = measurementFrequency;
  lastMeasureTime = 0;

  pinMode(PowerPin, OUTPUT);
}

float DeviceAttachment::Measure()
{
  if (Capability != "Measure") // Todo: use byte(2)
  {
    return 0;
  }

  digitalWrite(PowerPin, HIGH);
  delay(200);

  int analogInPin = A0; // ESP8266 Analog Pin ADC0 = A0
  int analogMeasurement = analogRead(analogInPin);
  digitalWrite(PowerPin, LOW);
  MeasureState = (float)analogMeasurement;
  return MeasureState;
}

bool DeviceAttachment::Switch(bool value)
{
  if (Capability != "Switch") // Todo: use byte (0)
  {
    return false;
  }

  Serial.println("Switch!");

  SwitchState = value;
  if (SwitchState)
  {
    digitalWrite(PowerPin, HIGH);
  }
  else
  {
    digitalWrite(PowerPin, LOW);
  }

  return SwitchState;
}

float DeviceAttachment::Range(float value)
{
  if (Capability != "Range") // Todo: use byte (1)
  {
    return 0;
  }

  Serial.println("Range!");

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

bool DeviceAttachment::ShouldMeasure()
{
  // Todo: add overflow checks
  unsigned long now = millis();
  unsigned long nextMeasureTime = lastMeasureTime + MeasurementFrequency;
  bool shouldMeasure = Capability == "Measure" && MeasurementFrequency > 0 && nextMeasureTime <= now;
  if (shouldMeasure)
  {
    lastMeasureTime = now;
  }

  return shouldMeasure;
}
