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
     byte capability; 
     byte powerPin; 
     byte analogInPin; 
  
   public: 
     DeviceAttachment(String id, String ownerUserId, String ownerDeviceSerial, String attachmentName, String serial, byte capability, byte powerPin); 
     float measure(); 
     bool toggle();     
     float dim(float value);     
 }; 
  
 #endif
