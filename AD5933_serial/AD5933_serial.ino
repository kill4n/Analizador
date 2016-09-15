#include <Wire.h>
#include "AD5933.h"
#define SLAVE_ADDRESS 0x0D
AD5933 analiz(SLAVE_ADDRESS);

void setup() {

  Serial.begin(9600);  // start serial for output
  delay(1000);
  Serial.println("Iniciar -o- Modulo.");  // start serial for output
  analiz.init();
}
byte x = 0;
int rcv;
void loop() {
  Serial.print("Configurar parametros. ");  // start serial for output
  if ( analiz.confParametros() == 8)
  {
    Serial.println("conf Ok");  // start serial for output
    if (analiz.preMedicion() == 2) {
      Serial.println("Premedicion -'- ok");  // start serial for output
      if (analiz.medicion() == 6) {
        Serial.println("Medicion ok");
        //Serial.print("real: ");
        //Serial.println(analiz.real);
        //Serial.print("imaginario: ");
        //Serial.println(analiz.imag);
        Serial.print("Impedancia: ");
        Serial.print(analiz.real);
        Serial.print(" + ");
        Serial.print(analiz.imag);
        Serial.println("i");
      }
    }
  }
  delay(4000);
}
