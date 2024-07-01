# 1 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino"
# 2 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino" 2
# 3 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino" 2
# 4 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\EspWifiTest.ino" 2

// create server on port 80
NetworkServer server(80);

void setup() {
  // initialize LED outputs
  pinMode(7, 0x03);
  pinMode(2, 0x03);
  digitalWrite(2, 0x0);

  Serial0.begin(115200);
  delay(2000);

  Serial0.println("Initializing WIFI");
  WiFi.disconnect(true);
  // static memory allocation
  WiFi.useStaticBuffers(true);
  WiFi.setHostname("testhost");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin("Technodrome", "STM32F405");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial0.print(".");
  }

  server.begin();
  Serial0.println("WIFI Connected");
}

void loop() {
  // listen for incoming clients
  NetworkClient client = server.available();

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
            Serial0.write(rxLine.c_str());
            rxLine = "";
            newData = false;
          }
          else {
            // send a standard response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");

            if(false) {
              client.println("Connection: close");
            }
            if(0) {
              client.println("Refresh: " + String(0));
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
    Serial0.println("client disconnected");
  }
}
# 1 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\webpage.ino"
# 2 "C:\\Users\\benja\\Documents\\Arduino\\EspWifiTest\\webpage.ino" 2

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

uint8_t ledState = 0x0;
int ledCount = 0;

String getWebpage() {
   String webpage = String(htmlText);
   // replace tags in html text
   String ledStateText = (ledState == 0x1) ? "ON" : "OFF";
   webpage.replace("{output0}", ledStateText);
   webpage.replace("{output1}", String(ledCount));
   return webpage;
}

void processInputs(String response) {
   if(response.indexOf("GET") >= 0) {
      if(response.indexOf("input0") >= 0) {
         ledState = 0x1;
         digitalWrite(7, ledState);
         ledCount++;
      }
      else if(response.indexOf("input1") >= 0) {
         ledState = 0x0;
         digitalWrite(7, ledState);
         ledCount++;
      }
   }
}
