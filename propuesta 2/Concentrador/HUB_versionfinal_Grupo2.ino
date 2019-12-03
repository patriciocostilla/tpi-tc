#include <Wire.h>

void setup()
{
  Wire.begin();
  Serial.begin(9600);
}

void loop()
{
  char c;
  String data = "{";
  for (int i=1; i<2; i++) {
    //if (i == 1) {
      Wire.requestFrom(i, 16);
    //} else {
      // Wire.requestFrom(i, 8);
    //}
 
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
