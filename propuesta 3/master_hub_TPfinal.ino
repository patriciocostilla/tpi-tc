
// Wire Master Reader
#include <Wire.h>  // Wire library for I2C communication

#include <ESP8266WiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient/releases/tag/v2.3
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson/releases/tag/v5.0.7    //Libraries needed to the connection to IBM Watson

//Connection to IBM 
//-------- Customise these values -----------
const char* ssid = "Zimmermann";
const char* password = "z03g14a16ff";

#define ORG "7xbev7" // This information is in the previous image
#define DEVICE_ID "fede"        //  Only for this Example 
#define DEVICE_TYPE "ESP8266"      // your device type 
#define TOKEN "8aL0mU5g3FyNdb?CiK"         // your device token
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char publishTopic[] = "iot-2/evt/status/fmt/json";
const char responseTopic[] = "iotdm-1/response";
const char manageTopic[] = "iotdevice-1/mgmt/manage";
const char updateTopic[] = "iotdm-1/device/update";
const char rebootTopic[] = "iotdm-1/mgmt/initiate/device/reboot";


void callback(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);


int table[]={0,0,0};
String stSend="";
String stSendRes="";
int temp;
int incl;
String payload;
int publishInterval = 10000;
long millisant;


void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  wifiConnect();
  mqttConnect();
  initManagedDevice();
  delay(1000);
}

void loop()
{
  if (!client.loop()) 
    {
     mqttConnect();
     initManagedDevice();
    } //if there is no connection, the program try to reconnect to IBM Watson
    
  Wire.requestFrom(2,3)  ;  // request 3 bytes from slave device #2. For I2C communication, the master is requesting information from the slave.
  
  for(int i=0;i<3;i++)
    {
      int c = Wire.read(); // receive a byte as character. The information is restored 1 byte a time in the "c" variable.
      if(i==0) //the first column is the temperature. 
        {
          temp=c;
        };
      if(i==1) //The second column is the inclination. This if statement is to restore the values of temperature and inclination to prepare later the json package for sending to IBM hub.
        {
          incl=c;
        };
      stSend+=c;
      stSend+=",";   //The values of the sensors are printed in this format: "temp,incl,group"
    }
    
  if ((stSend!=stSendRes)||((millis()-millisant)>publishInterval)) //The information is printed (Serial: OnPremise Solution)and published (IBM Watson: OnCloud Solution) every time the data change or every 10 seconds (if the data doesn't change).
    {
      Serial.print("DATA,TIME,");
      Serial.println(stSend);  //Each time the data is printed in this format: "DATA,TIME,temp,incl,group". 
      
      payload = "{\"TEMP\":";  //In the String variable "payload" the json package is built. 
      payload += temp;
      payload += ",\"INLC\":";
      payload += incl;
      payload += "}";

      client.publish(publishTopic, (char*) payload.c_str()); //The JSON package is published 
      millisant=millis();
    }
  stSendRes=stSend;
  stSend="";  
}


void wifiConnect() {
 //Serial.print("Connecting to "); Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(100);
   //Serial.print(".");
 }
 //Serial.print("WiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
 if (!!!client.connected()) {
   //Serial.print("Reconnecting MQTT client to "); Serial.println(server);
   while (!!!client.connect(clientId, authMethod, token)) {
     //Serial.print(".");
     delay(100);
   }
   //Serial.println();
 }
}

void initManagedDevice() {
 //if (client.subscribe("iotdm-1/response")) {
   //Serial.println("subscribe to responses OK");
 //} else {
   //Serial.println("subscribe to responses FAILED");
// }

// if (client.subscribe(rebootTopic)) {
 //  Serial.println("subscribe to reboot OK");
 //} else {
 //  Serial.println("subscribe to reboot FAILED");
// }

// if (client.subscribe("iotdm-1/device/update")) {
//   Serial.println("subscribe to update OK");
// } else {
//   Serial.println("subscribe to update FAILED");
// }

 StaticJsonBuffer<300> jsonBuffer;
 JsonObject& root = jsonBuffer.createObject();
 JsonObject& d = root.createNestedObject("ldr");
 JsonObject& metadata = d.createNestedObject("metadata");
 metadata["publishInterval"] = publishInterval;
 JsonObject& supports = d.createNestedObject("supports");
 supports["deviceActions"] = true;

 char buff[300];
 root.printTo(buff, sizeof(buff));
 //Serial.println("publishing device metadata:"); Serial.println(buff);
 //if (client.publish(manageTopic, buff)) {
   //Serial.println("device Publish ok");
 //} else {
   //Serial.print("device Publish failed:");
 //}
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
 //Serial.print("callback invoked for topic: "); Serial.println(topic);

 if (strcmp (responseTopic, topic) == 0) {
   return; // just print of response for now
 }

 if (strcmp (rebootTopic, topic) == 0) {
   //Serial.println("Rebooting...");
   ESP.restart();
 }

 if (strcmp (updateTopic, topic) == 0) {
   handleUpdate(payload);
 }
}



void handleUpdate(byte* payload) {
 StaticJsonBuffer<300> jsonBuffer;
 JsonObject& root = jsonBuffer.parseObject((char*)payload);
 if (!root.success()) {
   //Serial.println("handleUpdate: payload parse FAILED");
   return;
 }
 //Serial.println("handleUpdate payload:"); root.prettyPrintTo(Serial); Serial.println();

 JsonObject& d = root["daystats"];
 JsonArray& fields = d["fields"];
 for (JsonArray::iterator it = fields.begin(); it != fields.end(); ++it) {
   JsonObject& field = *it;
   const char* fieldName = field["field"];
   if (strcmp (fieldName, "metadata") == 0) {
     JsonObject& fieldValue = field["value"];
     if (fieldValue.containsKey("publishInterval")) {
       publishInterval = fieldValue["publishInterval"];
      // Serial.print("publishInterval:"); Serial.println(publishInterval);
     }
   }
 }
}
