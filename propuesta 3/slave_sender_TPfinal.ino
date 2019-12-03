// Wire Slave Sender
 
#include <Wire.h>
int table[]={0,0,0};
int pinIncl= 2;
int temp;
int tempe;
int incl = 0;
int grupo;

void setup() {

  pinMode(pinIncl,INPUT);
  Wire.begin(2);                 //Inicializa la libreria wire y se conecta al bus de I2C como un esclavo de dir 
  Wire.onRequest(requestEvent); // Register a function to be called when a master requests data from this slave device.
}

void loop()
{
 
  temp=analogRead(A0); //lee el sensor de temperatura
 
  tempe = (temp * 5 * 100)/1024; //convierte a grados celsius
 
  incl=digitalRead(pinIncl);
 
 
  grupo = 1;
  table[0]=tempe;
  table[1]=incl;
  table[2]=grupo;
 
  delay(100);
}

//Funcion callback que se ejecuta ante un evento de solicitud del master
void requestEvent() {
  uint8_t Buffer[2];
  Buffer[0]=table[0];
  Buffer[1]=table[1];
  Buffer[2]=table[2];

  Wire.write(Buffer,3); //pone en cola bytes para la transmisión de un dispositivo maestro a esclavo 
   
}
