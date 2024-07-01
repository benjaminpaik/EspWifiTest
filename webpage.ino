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
