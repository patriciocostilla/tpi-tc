#include <Wire.h>

int bytes_8 = 8; // cantidad de bytes asignados para 2 sensores (4 bytes por cada uno)
int bytes_16 = 16; // cantidad de bytes asignados para 2 sensores particulares-> joystick (12 bytes:2 de 4 bytes por los ejes y 4 bytes por pulsador) y 4 bytes para el sensore restante.
int cant_placas = 9; // cantidad de placas a conectar a la red I2C

void setup()
{
  Wire.begin(); 
  Serial.begin(9600);
}

void loop()
{
  char c;
  String data = "{";
  for (int i=0; i<cant_placas; i++) {
    if (i == 1) {
      Wire.requestFrom(i, 1bytes_16);
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

    if (i == 1) {
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
  Serial.println(data);
  delay(500);
}
