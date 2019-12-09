#include <Wire.h>

void setup()
{
  Wire.begin(1); //PONER EL Nº DE GRUPO - 1 => SOMOS EL GRUPO 2, ENTONCES VA 1
  Wire.onRequest(sendData);
  Serial.begin(9600);
}

void loop()
{
  delay(10);
}

void sendData(int howMany)
{

  int xPos = analogRead(A1); //SENSOR Joystick X position
  char data[4];
  sprintf(data, "%0.4d", xPos);
  for (int i = 0; i < 4; i++) {
    Wire.write(data[i]);
  }

  int yPos = analogRead(A2); //SENSOR Joystick Y position
  sprintf(data, "%0.4d", yPos);
  for (int i = 0; i < 4; i++) {
    Wire.write(data[i]);
  }

  int zPos = digitalRead(2); //SENSOR Joystick Button
  sprintf(data, "%0.4d", zPos);
  for (int i = 0; i < 4; i++) {
    Wire.write(data[i]);
  }

  int fotoreceptor = analogRead(A0); //SENSOR Fotoreceptor
  sprintf(data, "%0.4d", fotoreceptor);
  for (int i = 0; i < 4; i++) {
    Wire.write(data[i]);
  }
}
