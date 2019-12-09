#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>

// ----------------  CREDENCIALES IBM WATSON -------------------
#define ORG "**********" // ORG IBM IoT
#define DEVICE_ID "**********"        //  id dispositivo 
#define DEVICE_TYPE "*********"      // tipo de dispositivo
#define TOKEN "************"         // Token dispositivo autognerado

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char TopicPub[] = "iot-2/evt/status/fmt/json"; //tópico a publicar 

char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
int port = 1883;

//---------------- CONFIGURACIÓN DE RED ETHERNET ----------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 137, 2);
//IPAddress myDns(192, 168, 0, 1);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

int bytes_8 = 8; // cantidad de bytes asignados para 2 sensores (4 bytes por cada uno)
int bytes_16 = 16; // cantidad de bytes asignados para 2 sensores particulares-> joystick (12 bytes:2 de 4 bytes por los ejes y 4 bytes por pulsador) y 4 bytes para el sensore restante.
int cant_placas = 9; // cantidad de placas a conectar a la red I2C
int publishInterval = 1000; // 10 segundos
long lastPublishMillis;

EthernetClient ethernetClient;
PubSubClient client(server, port,NULL, ethernetClient);

void setup() {
  Wire.begin();
  Serial.begin(9600); // establecemos comunicación serieal

  while (!Serial) {
    ; // Espere a que se conecte el puerto serie. Necesario solo para puerto USB nativo
  }

  // inicia conexión via ethernet:
  Serial.println("Inicializar Ethernet con DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Error al configurar Ethernet usando DHCP");
    // Compruebe si hay hw Ethernet presente
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("No se encontró la shield de Ethernet. Lo sentimos, no se puede ejecutar sin hardware." );
      while (true) {
        delay(1); // no hacer nada, no tiene sentido ejecutar sin hardware Ethernet
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("El cable de Ethernet no está conectado.");
    }
    // intenta congifurar usando la dirección IP en lugar de DHCP:
    Ethernet.begin(mac, ip, gateway,subnet);
  } else {
    Serial.print("  IP asignada por DHCP ");
    Serial.println(Ethernet.localIP());
  }
  // se le da un un segundo para inicializar
  delay(1000);
}

void loop() {

  mqttConnect();
  
   if (millis() - lastPublishMillis > publishInterval) 
  {
    String payload = construirJson(); 
    Serial.print("Enviando payload: "); Serial.println(payload);
      
    if (client.publish(TopicPub, (char*) payload.c_str()),true) 
    {
      Serial.println("Publicado correctamente"); // Envio correcto
    } 
    else 
    {
      Serial.println("Problemas al publicar"); // Fallo en el envio
    }
   lastPublishMillis = millis();
  }
}


void mqttConnect() {// maneja la conexion por MQTT
   if (!!!client.connected()) 
   {  
     Serial.print("Reconectando cliente a "); 
     Serial.println(server);
     while ( !!!client.connect(clientId, authMethod, token)) 
     {
        Serial.print(".");
        delay(500);
     }
     Serial.println();
   }
}

String construirJson(){
  char c;
  String data = "{";
  for (int i=0; i<cant_placas; i++) {
    if (i == 1) {
      Wire.requestFrom(i, bytes_16);
    } else {
       Wire.requestFrom(i, bytes_8);
    }
        
    //SENSOR 1
    data += "\"val_";
    data += i*2;
    data += "\": ";
    String num = "";
    for (int j=0; j<4; j++) {
      c = Wire.read();
      num += c;
    }
    data += num.toInt();
  
    if (i == 1) { //equipo con Sensor de Joystick
      //SENSOR 1Y
      data += ", \"val_";
      data += i*2;
      data += "Y\": ";
      num = "";
      for (int j=0; j<4; j++) {
        c = Wire.read();
        num += c;
      }
      data += num.toInt();
      
      //SENSOR 1Z
      data += ", \"val_";
      data += i*2;
      data += "Z\": ";
      num = "";
      for (int j=0; j<4; j++) {
        c = Wire.read();
        num += c;
      }
      data += num.toInt();
    }
  
    //SENSOR 2
    data += ", \"val_";
    data += i*2 + 1;
    data += "\": ";
    num = "";
    for (int j=0; j<4; j++) {
      c = Wire.read();
      num += c;
    }
    data += num.toInt();
    data += ", ";
  }
  data.remove(data.length()-2, 2);
  data += "}";
  return data;
}
