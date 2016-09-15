#ifndef AD5933_H
#define AD5933_H

#include "Arduino.h"
#include <Wire.h>
class AD5933
{
  public:
    AD5933(byte _slave);
    void init();
    byte slave;
    //private:
    int escribirData(byte pos, byte data);
    byte leerDato(byte pos);
    int confParametros(void);
    int preMedicion(void);
    int medicion(void);
    int real, imag;
  private:
    byte realUp, realDown, imagUp, imagDown;
    int inc;
};

#endif //AD5933_H
