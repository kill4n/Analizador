#include "AD5933.h"
#include <SPI.h>
#include <Adafruit_WINC1500.h>
// Define the WINC1500 board connections below.
// If you're following the Adafruit WINC1500 board
// guide you don't need to modify these:
#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2

#define SLAVE_ADDRESS 0x0D
AD5933 analiz(SLAVE_ADDRESS);

// Setup the WINC1500 connection with the pins above and the default hardware SPI.
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);

#define LED_PIN  13

char ssid[] = "Analizador_1";      //  created AP name
char pass[] = "wing";         // (not supported yet)

int status = WL_IDLE_STATUS;
Adafruit_WINC1500Server server(80);

void setup() {
#ifdef WINC_EN
  pinMode(WINC_EN, OUTPUT);
  digitalWrite(WINC_EN, HIGH);
#endif

  //  while (!Serial);
  delay(1000);
  Serial.begin(9600);      // initialize serial communication
  Serial.println("Access Point Web Server");
  pinMode(LED_PIN, OUTPUT);      // set the LED pin mode
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  if (WiFi.beginAP(ssid) != WL_CONNECTED) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWifiStatus();

  analiz.init();

}
bool readADC;
int val;

void loop() {
  Adafruit_WINC1500Client client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<h1>Analizador de impedancias</h1><br><hr>");
            client.print("Clic <a href=\"/H\">aqui</a> para encender el LED<br>");
            client.print("Clic <a href=\"/L\">aqui</a> para apagar el LED<br>");
            client.print("Clic <a href=\"/ADC\">aqui</a> para leer el Analizador.<br>");
            if (readADC)
            {
              client.print("<hr> Valor Real: ");
              client.print(analiz.real);
              client.print("<br>");
              client.print("Valor imaginario: ");
              client.print(analiz.imag);
              client.print("<br>");
              client.print("<hr> impedancia: ");
              client.print(analiz.real);
              client.print(" + ");
              client.print(analiz.imag);
              client.print("i <br>");
              
            }

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          readADC = false;
          digitalWrite(LED_PIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          readADC = false;
          digitalWrite(LED_PIN, LOW);                // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /ADC")) {
          readADC = true;
          if ( analiz.confParametros() == 8)
          {
            Serial.println("conf Ok");  // start serial for output
            if (analiz.preMedicion() == 2) {
              Serial.println("Premedicion -'- ok");  // start serial for output
              if (analiz.medicion() == 6) {
                Serial.println("Medicion ok");
                Serial.print("real: ");
                Serial.println(analiz.real);
                Serial.print("imaginario: ");
                Serial.println(analiz.imag);
              }
            }
          }
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
