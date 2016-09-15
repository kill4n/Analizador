//Comentarear Todo todo todo....

#include "AD5933.h"
#define SLAVE_ADDRESS 0x0D
AD5933 analiz(SLAVE_ADDRESS);
#define LED_PIN  13
void setup() {
  delay(1000);
  Serial.begin(9600);      // initialize serial communication
  pinMode(LED_PIN, OUTPUT);      // set the LED pin mode
  delay(100);
  Serial.println("#:Start");
  analiz.init();
}

void loop() {
  Serial.println("#:hola");
  if ( analiz.confParametros() == 8)
  {
    Serial.println("#:confPar");
    if (analiz.preMedicion() == 2) {
      Serial.println("#:PreMed");
      if (analiz.medicion() >0) {
        Serial.println("#:Med");
        Serial.print("@:");
        Serial.print(analiz.real);
        Serial.print(":");
        Serial.println(analiz.imag);
      }
    }
  }
}
