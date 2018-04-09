/*
  LED Strip constrol for ESP32
 */

#define BRIGHTER   0xF700FF
#define DARKER     0xF7807F
#define OFF        0xF740BF
#define ON         0xF7C03F
#define R          0xF720DF
#define G          0xF7A05F
#define B          0xF7609F
#define WHITE      0xF7E01F
#define ORANGE     0xF710EF
#define TURQUOISE  0xF7906F
#define PURPLE     0xF750AF
#define FLASH      0xF7D02F
#define YELLOW     0xF730CF
#define AQUAMARIN  0xF7B04F
#define ORCHID     0xF7708F
#define STROBE     0xF7F00F
#define LIMEGREEN  0xF708F7
#define STEELBLUE  0xF78877
#define MAGENTA    0xF748B7
#define FADE       0xF7C837
#define LIGHTGREEN 0xF728D7
#define SKYBLUE    0xF7A857
#define PINK       0xF76897
#define SMOOTH     0xF7E817

#include <WiFi.h>
#include "credentials.h"
// const char* ssid     = "xxx";          see credentials.ino
// const char* password = "yyy";
WiFiServer server(80);


#include <IRremote.h>
IRsend irsend;                       // ESP32 GPIO3 (DevKitC/WROOM32 pin 40)


void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  wifiConnect();
}

void loop() {
  serveClient();

  if(WiFi.status() != WL_CONNECTED) {
    server.end();
    wifiConnect();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void wifiConnect() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      
      count++;
      if(count > 20) {
        count = 0;
        WiFi.begin();
        Serial.println("#");
      }
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void serveClient(){
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
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
            client.print("Hic sunt dracones."); // security through obscurity (PoC only, don't do that!!!)

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was something
        if (currentLine.endsWith("GET /on")) {
          sendCode(ON);
        }
        if (currentLine.endsWith("GET /off")) {
          sendCode(OFF);
        }
        if (currentLine.endsWith("GET /yellow")) {
          sendCode(YELLOW);
        }
        if (currentLine.endsWith("GET /orange")) {
          sendCode(ORANGE);
        }
        if (currentLine.endsWith("GET /white")) {
          sendCode(WHITE);
        }
        if (currentLine.endsWith("GET /brighter")) {
          sendCode(BRIGHTER);
        }
        if (currentLine.endsWith("GET /darker")) {
          sendCode(DARKER);
        }
        if (currentLine.endsWith("GET /smooth")) {
          sendCode(SMOOTH);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void sendCode(int code) {
  Serial.print("## SEND ## ");
  Serial.println(code);
  irsend.sendNEC(code, 32);
}

