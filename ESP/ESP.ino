#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"


const char* ssid = "UPC"; //ssid ul de la wifi
const char* password = "12345678Aa"; //parola de la wifi
const long utcOffset = 7200; //diferenta utc in secunde
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffset);
//WiFiServer server(80);
String header;
AsyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  timeClient.begin();
  timeClient.update();
  Serial.println(WiFi.localIP());
  delay(1);
  Serial.println(timeClient.getHours());
  delay(1);
  Serial.println(timeClient.getMinutes());
  delay(1);
  Serial.println(timeClient.getSeconds());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    int param = request->params();
    //Serial.println(param);
    for (int i = 0; i < param; i++) {
      AsyncWebParameter* p = request->getParam(i);
      Serial.println("Param name: ");
      delay(5);
      Serial.println(p->name());
      delay(5);
      Serial.println(p->value());
    }
    request->send(200, "text/html", "<html><body><h1>Mesaj primit</h1></body></html>");
  });
  server.begin();
}

void loop() {

}
