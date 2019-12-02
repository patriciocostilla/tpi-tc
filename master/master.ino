#include <Wire.h>
#include <stdlib.h>


#define numerodebytes 2
#define retardo 100

int paquete[numerodebytes];

void setup()
{
  Serial.begin(9600); 
  Wire.begin();        // Activate I2C link
}

void loop()
{
  for (int direccion = 1; direccion <= 3; direccion++)
  {
    Wire.requestFrom(direccion, numerodebytes); 
    if(Wire.available() == numerodebytes)
    {
      for (int i = 0; i < numerodebytes; i++) paquete[i] = Wire.read();
      for (int j = 0; j < numerodebytes; j++) Serial.print((char)paquete[j]);   
    }
    Serial.println();
  }
    //delay(retardo);
}