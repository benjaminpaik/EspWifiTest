#include <Arduino.h>
#line 1 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino"
#include <WiFi.h>
#include "webpage.h"
#include "definitions.h"

// create server on port 80
WiFiServer server(80);

#line 8 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino"
void setup();
#line 35 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino"
void loop();
#line 21 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\webpage.ino"
String getWebpage();
#line 30 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\webpage.ino"
void processInputs(String response);
#line 8 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino"
void setup() {
  // initialize LED outputs
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  digitalWrite(LED_WHITE, LOW);

  Serial.begin(115200);
  delay(2000);

  Serial.println("Initializing WIFI");
  WiFi.disconnect(true);
  // static memory allocation
  WiFi.useStaticBuffers(true);
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin(NETWORK_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin();
  Serial.println("WIFI Connected");
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();

  if (client) {

    bool newData = false;
    String rxLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        rxLine += c;
        // newline received
        if (c == '\n') {
          if(newData) {
            processInputs(rxLine);
            Serial.write(rxLine.c_str());
            rxLine = "";
            newData = false;
          }
          else {            
            // send a standard response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");

            if(CLOSE_CONNECTION) {
              client.println("Connection: close");
            }
            if(REFRESH_INTERVAL) {
              client.println("Refresh: " + String(REFRESH_INTERVAL));
            }

            client.println();
            client.println(getWebpage());
            break;
          }
        }
        // character is neither a newline nor a carriage return
        else if (c != '\r') {
          newData = true;
        }
      }
    }

    // wait before closing the connection
    delay(5);
    client.stop();
    Serial.println("client disconnected");
  }
}

#line 1 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\webpage.ino"
#include "webpage.h"

String htmlText = 

R"(<!DOCTYPE html>
<html>
   <form action="./input0">
      <input type="submit" value="LED On" />
   </form>
   <form action="./input1">
      <input type="submit" value="LED Off" />
   </form>
   <p>LED is {output0}</p>
   <p>Count is {output1}</p>
   </body>
</html>)";

uint8_t ledState = LOW;
int ledCount = 0;

String getWebpage() {
   String webpage = String(htmlText);
   // replace tags in html text
   String ledStateText = (ledState == HIGH) ? "ON" : "OFF";
   webpage.replace("{output0}", ledStateText);
   webpage.replace("{output1}", String(ledCount));
   return webpage;
}

void processInputs(String response) {
   if(response.indexOf("GET") >= 0) {
      if(response.indexOf("input0") >= 0) {
         ledState = HIGH;
         digitalWrite(LED_RED, ledState);
         ledCount++;
      }
      else if(response.indexOf("input1") >= 0) {
         ledState = LOW;
         digitalWrite(LED_RED, ledState);
         ledCount++;
      }
   }
}

