#include "Arduino.h"
#include "AD5933.h"

AD5933::AD5933(byte _slave)
{
  slave = _slave;
}
void AD5933::init() {
  Wire.begin();        // join i2c bus (address optional for master)
  delay(1000);
}
int AD5933::escribirData(byte pos, byte data) {
  Wire.beginTransmission(slave); // transmit to device #8
  Wire.write(pos);        // sends five bytes
  Wire.write(data);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  return 1;
}
byte AD5933::leerDato(byte pos)
{
  Wire.beginTransmission(slave); // transmit to device #8
  Wire.write(pos);        // sends five bytes
  Wire.endTransmission();    // stop transmitting
  byte data = 255;
  delay(100);
  Wire.requestFrom(slave, 1);    // request 6 bytes from slave device #8
  if (Wire.available() > 0) {
    char c = Wire.read(); // receive a byte as character
    data = (byte)c;
  }
  return data;
}

int AD5933::confParametros(void) {
  int seguimiento;

  // Configuración de la medición (Usando frecuencia por defecto de
  // 30 khz)
  byte reg82 = 0x00; //0x0F; 1khz
  byte reg83 = 0x20; //0x5C;
  byte reg84 = 0xC4; //0x28;

  seguimiento += escribirData((byte)0x82, reg82);
  delay(100);
  seguimiento += escribirData((byte)0x83, reg83);
  delay(100);
  seguimiento += escribirData((byte)0x84, reg84);
  delay(100);

  // Configuración del incremento de frecuencias (Defecto en 10 hz)
  /*
    uint8_t reg85 = 0x00;
    uint8_t reg86 = 0x01;
    uint8_t reg87 = 0x4F;
  */

  byte  reg85 = 0x00;
  byte  reg86 = 0x01;
  byte  reg87 = 0x4F;

  seguimiento += escribirData((byte)0x85, reg85);
  delay(100);
  seguimiento += escribirData((byte)0x86, reg86);
  delay(100);
  seguimiento += escribirData((byte)0x87, reg87);
  delay(100);

  // Configuración del número de incrementos (defecto 150) -> 10
  byte  reg88 = 0x00;
  byte  reg89 = 0x05;

  seguimiento += escribirData(0x88, reg88);
  seguimiento += escribirData(0x89, reg89);

  return seguimiento;
}

int AD5933::preMedicion(void)
{
  int seguimiento;
  //Stand by mode
  // Control Reg 0x80 (D15 to D8) Reg 0x81 (D7 to D0)
  // reg80 d15-d12 1011 stand d11 ni d10d9 Rango (voltaje) d8 ganancia
  //reg81 solo d4 reset y d3 0 para reloj interno.
  byte reg80 = 0b10110001;
  seguimiento += escribirData(0x80, reg80);
  //  usart_serial_write_packet(USART_BLUETOOTH, "En modo StandBy\n\r",sizeof("En modo StandBy\n\r"));
  delay(100);//4000);
  //Iniciar con frecuencia de inicio
  reg80 = 0b00010001;
  seguimiento += escribirData(0x80, reg80);
  //  usart_serial_write_packet(USART_BLUETOOTH,
  //    "En modo Inicio, esperando tiempo de asentamiento\n\r",
  //     sizeof("En modo Inicio, esperando tiempo de asentamiento\n\r"));
  delay(4000);
  //  usart_serial_write_packet(USART_BLUETOOTH,
  //    "Empezando barrido\n\r",
  //    sizeof("Empezando barrido\n\r"));
  return seguimiento;
}



int AD5933::medicion(void)
{
  int seguimiento;
  inc=0;
  //Condifuracion ciclos
  byte reg8A = 0xFF;
  byte reg8B = 0xFF;
  seguimiento += escribirData(0x8A, reg8A);
  delay(10);
  seguimiento += escribirData(0x8B, reg8B);
  delay(10);

  //Iniciando modo barrido
  byte reg80 = (byte)0b00100001;

  seguimiento += escribirData(0x80, reg80);
  delay(10);

  int estado = 1;
  byte lectura;

  while (estado)
  {
    lectura = leerDato(0x8F);
    if (lectura & 0b00000010)
    {
      //Leer informacion
      Serial.print("$:medi:");
      Serial.println(inc++);
      delay(10);
      realUp = leerDato(0x94);
      delay(10);
      realDown = leerDato(0x95);
      delay(10);
      imagUp = leerDato(0x96);
      delay(10);
      imagDown = leerDato(0x97);
      delay(10);
      real = (((byte) realUp) << 8) | (realDown);
      imag = (((byte) imagUp) << 8) | (imagDown);
      Serial.print("$:re;");
      Serial.print(real);
      Serial.print(":imag:");
      Serial.println(imag);
      
      delay(8);
      lectura = leerDato(0x8F);
      Serial.print("%:lectura:");
      Serial.println(lectura,BIN);

      if (lectura & 0b00000100)
      {
        estado = 0;
      }
      else
      {
        byte reg80 = 0b00110001;
        seguimiento += escribirData(0x80, reg80);
      }
    }
  }
}

