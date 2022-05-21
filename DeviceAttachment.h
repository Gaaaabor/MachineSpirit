// DeviceAttachment.h 
 #ifndef DeviceAttachment_h 
 #define DeviceAttachment_h 
  
 #include <Arduino.h> 
  
 class DeviceAttachment { 
   private: 
     String id; 
     String ownerUserId;
     String ownerDeviceSerial; 
     String attachmentName; 
     String serial; 
     float measureState;
     bool switchState;
     float dimState;
     String capability; 
     int powerPin; 
     int analogInPin; 
  
   public: 
     DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String serial, String capability, int powerPin); 
     float measure(); 
     bool toggle();     
     float dim(float value);     
 }; 
  
 #endif
