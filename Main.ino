 #include <ESP8266WiFi.h> 
 #include <WebSocketClient.h> 
 #include <ArduinoWebsockets.h> 
 #include <ArduinoJson.h> 
 #include "DeviceAttachment.h" 
  
 using namespace websockets; 
  
 const char* ssid = ""; 
 const char* password = ""; 
 const String ownerUserId = ""; 
 const String deviceSerial = ""; 
 const char* serverAddress = ""; 
 const uint16_t serverPort = 80;
 bool websocketConnected = false;
  
 // Use WiFiClient class to create TCP connections 
 WebsocketsClient wsclient; 
  
 void onMessageCallback(WebsocketsMessage websocketsMessage) 
 { 
   String message = websocketsMessage.data(); 
  
   Serial.print("Got Message: "); 
   Serial.println(message); 

   DynamicJsonDocument doc(1024); 
   deserializeJson(doc, message); 
   
   String messageType = String(doc["MessageType"]);

   if(messageType == "AttachmentCreatedNotification")
   {
     String id = String(doc["Id"]); 
     String ownerDeviceSerial = String(doc["OwnerDeviceSerial"]); 
     String attachmentName = String(doc["AttachmentName"]); 
     String serial = String(doc["Serial"]); 
     String state = String(doc["State"]); 
     String capability = String(doc["Capability"]); 
     String powerPin = String(doc["PowerPin"]);

     //Todo: create an attachment and add to an array of attachments
     return;
    }
 } 
  
 void onEventsCallback(WebsocketsEvent event, String data) 
 { 
   if (event == WebsocketsEvent::ConnectionOpened) 
   { 
     Serial.println("Connnection Opened"); 
     websocketConnected = true; 
   } 
   else if (event == WebsocketsEvent::ConnectionClosed) 
   { 
     Serial.println("Connnection Closed"); 
     websocketConnected = false; 
   } 
   else if (event == WebsocketsEvent::GotPing) 
   { 
     Serial.println("Got a Ping!"); 
   } 
   else if (event == WebsocketsEvent::GotPong) 
   { 
     Serial.println("Got a Pong!"); 
   } 
 } 
  
 void setup() 
 { 
   Serial.begin(115200); 
   delay(10);
  
   Serial.println(); 
   Serial.println(); 
   Serial.println("Connecting to "); 
   Serial.println(ssid); 
  
   WiFi.begin(ssid, password); 
  
   while (WiFi.status() != WL_CONNECTED) 
   { 
     delay(500); 
     Serial.print("."); 
   } 
  
   Serial.println(); 
   Serial.println("WiFi connected"); 
   Serial.print("IP address: "); 
   Serial.println(WiFi.localIP()); 
  
   // Setup Callbacks 
   wsclient.onMessage(onMessageCallback); 
   wsclient.onEvent(onEventsCallback); 
  
   connect(); 
  
   //Send RegisterDevice command
   sendRegisterDeviceRequest();   
 } 
  
 void connect() 
 { 
   if (websocketConnected) 
   { 
     Serial.println("WebSocketClient connected!"); 
     return; 
   } 
  
   // Connect to websocket server 
   Serial.println("Connecting to websocket server"); 
   while (!wsclient.connect(serverAddress, serverPort, "/deviceconnection")) 
   { 
     delay(500); 
     Serial.print("."); 
   } 
  
   Serial.println("WebSocketClient connected!"); 
 } 
  
 void loop() 
 { 
   if (wsclient.available()) 
   { 
     wsclient.poll(); 
   } 
  
   if (!websocketConnected) 
   { 
     delay(5000); 
     connect(); 
   }
 } 
  

//Todo: Cleanup these
 void sendRegisterDeviceRequest() 
 { 
   wsclient.send("{\"MessageType\":\"RegisterDeviceRequest\",\"OwnerUserId\":\""+ownerUserId+"\",\"Serial\":\""+deviceSerial+"\",\"Name\":\"Test device 2\"}"); 
   Serial.println("Register Device"); 
 }
  
 void sendRecordMeasurementRequest(String serial, long measurementValue, String unitCode) 
 {   
   wsclient.send("{\"MessageType\":\"RecordMeasurementRequest\",\"Serial\":\""+serial+"\",\"Value\":"+String(measurementValue)+",\"UnitCode\":\""+unitCode+"\"}"); 
   Serial.println("Sent RecordMeasurementRequest: "+String(measurementValue)); 
 } 
  
 void sendRecordSwitchRequest(String serial, String measurementValue) 
 {   
   wsclient.send("{\"MessageType\":\"RecordSwitchRequest\",\"Serial\":\""+serial+"\",\"Value\":\""+measurementValue+"\"}"); 
   Serial.println("Sent RecordSwitchRequest: "+measurementValue); 
 } 
  
 void sendRecordDimRequest(String serial, long measurementValue) 
 {   
   wsclient.send("{\"MessageType\":\"RecordDimRequest\",\"Serial\":\""+serial+"\",\"Value\":"+String(measurementValue)+"}"); 
   Serial.println("Sent RecordDimRequest: "+String(measurementValue)); 
 }
