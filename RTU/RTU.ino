#include <Wire.h>

void setup()
{
  Wire.begin(5); //PONER EL Nº DE GRUPO - 1 => PARA EL GRUPO 1 VA 0
  Wire.onRequest(sendData);
  Serial.begin(9600);
}

void loop()
{
  delay(10);
}

void sendData(int howMany)
{
  int value1 = analogRead(A0); //SENSOR 1
  char data[4];
  sprintf(data, "%0.4d", value1);
  for (int i = 0; i<4; i++) {
    Wire.write(data[i]);
  }

  int value2 = analogRead(A1); //SENSOR 2
  sprintf(data, "%0.4d", value2);
  for (int i = 0; i<4; i++) {
    Wire.write(data[i]);
  }
}
