#include <Wire.h>

#define direccion 1
#define numerodebytes 2

byte paquete[numerodebytes];

void setup()
{
  Serial.begin(9600);
  Wire.begin(direccion);
  Wire.onRequest(requestEvent);
}

void loop()
{ 
  paquete[0] = 49; 
  paquete[1] = 88;
}

void requestEvent()
{
  Serial.println("Sending data");
  Wire.write(paquete,numerodebytes);  
}
