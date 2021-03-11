#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "UPC"; //ssid ul de la wifi
const char* password = "12345678Aa"; //parola de la wifi
const long utcOffset = 7200; //diferenta utc in secunde
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffset);

String header;
AsyncWebServer server(80);

IPAddress local_IP(192,168,0,206);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
String response="<!DOCTYPEhtml><html><head><title>ESPCLOCK</title></head><body style='background-color:teal;'><h1>ESP</h1><form action='http://192.168.0.206/?alarma'method='GET'><div><label for='alarma'>Seteaza alarma pe care o doresti</label><input type='number' name='alarma' id='alarma'/><button>Trimite</button></div></form><form action='http://192.168.0.206/?alarmb' method='GET'><div><label for='alarmb'>Seteaza ora la care doresti sa primesti reminder-ul:</label><input type='number'name='alarmb'id='alarmb'/><button>Trimite</button></div></form><form action='http://192.168.0.206/?rpauza=' method='GET'><div><lablel for='pauza'>Ia o pauza de: </lablel><input type='number' name='rpauza'> minute  <button>Trimite</button></div></form><form action='http://192.168.0.206/?temper=' method='GET'><div><lablel for='temp'>Seteaza temperatura pe care o doresti </label><input type='number'name='temper'><button>Trimite</button></div></form></body></html>";

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  WiFi.hostname("espclock");
  WiFi.begin(ssid, password);
  MDNS.begin("espclock");
  //WiFi.config(local_IP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);w
    Serial.print(".");
  }
  
  timeClient.begin();
  
  Serial.begin(9600);
  Serial.println(WiFi.localIP());
  timeClient.update();
  delay(25);
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
    request->send(200, "text/html", response);
  });
  server.begin();
}

void loop() {
  String arduino_data = "";
  bool string_ready = false;
  if (Serial.available() > 0) {
    arduino_data = Serial.readString();
    string_ready = true;
    arduino_data.trim();
    if (arduino_data == "TIME") {
      timeClient.update();
      Serial.println("TIME");
      Serial.println(timeClient.getHours());
      Serial.println(timeClient.getMinutes());
      Serial.print(timeClient.getSeconds());
    }
  }
}
