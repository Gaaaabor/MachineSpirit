// DeviceAttachment.cpp 
 #include "DeviceAttachment.h" 
  
 DeviceAttachment::DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String serial, String capability, int powerPin) 
 {     
   id = id;
   ownerUserId = ownerUserId;
   ownerDeviceSerial = ownerDeviceSerial; 
   attachmentName = attachmentName; 
   serial = serial; 
   capability = capability; 
   powerPin = powerPin; 
  
   analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0 
    
   pinMode(powerPin, OUTPUT); 
 } 
  
 float DeviceAttachment::measure()  
 { 
   digitalWrite(powerPin, HIGH); 
   delay(200); 
   int analogMeasurement = analogRead(analogInPin); 
   digitalWrite(powerPin, LOW);
   measurestate = (float)analogMeasurement;
   return measureState; 
 } 
  
 bool DeviceAttachment::toggle()  
 { 
   if(switchState) 
   { 
     digitalWrite(powerPin, LOW);
   }
   else
   {
     digitalWrite(powerPin, HIGH);
   }
   
   switchState = !state;
   return switchState;   
 } 
  
 float DeviceAttachment::dim(float value) { 
   dimState = value; 
   return dimState; 
 }
