//Adaugare library-uri
#include <LiquidCrystal.h>  //pt LCD
#include <SoftwareSerial.h> //pt comunicarea cu esp-ul
#include <DHT.h> //pt senzorul de umiditate/temperatura
#include <Servo.h> // pt controlul servomotoarelor

#define DHTPIN 4   //pinul pentru senzorul de umiditate/temperatura
#define DHTTYPE DHT22 //tipul de senzor

Servo servo; //initializare servo

DHT dht(DHTPIN, DHTTYPE); //initializare senzor

//initializare variabile
int tempSensor = 0, tempVal = 0, prev_temp_val = 0, med_temp_val = 0, temp_dorita;
bool temp_dorita_bool = false, temp_dorita_reset = false;
int lightSensor = 1, lightValue = 0, light_count = 10;
int piezoPin = A2;

bool time_date_bool = true;

int time_date_h = 0, time_date_m, time_date_s;
bool alarma = false;
int h_alarma = 0, m_alarma = 0, alarma_light = 0;
int h_alarma_birou = 0, m_alarma_birou = 0; bool alarma_birou = false;
int check_time = 0;
bool pauza = false; int pauza_left = 10, pauza_count = 0;
float hum, temp, med_hum, prev_hum = 0;


//sun icons in bytes
byte sun_full[] = {B00100, B01110, B11111, B11111, B11111, B11111, B01110, B00100};
byte sun_rise[] = {  B00000, B00000, B00000, B00000, B01110, B11111, B11111, B11111};
//moon icon in bytes
byte moon_half[] = { B00000, B00011, B00110, B01110, B11100, B11100, B11000, B11000};
byte moon_rise[] = { B00000, B00000, B00000, B00110, B01000, B10000, B10000, B11000};
byte moon_full[] = {  B00000, B11111, B11101, B11001, B11111, B10111, B11111, B00000};


LiquidCrystal lcd(7, 8, 9, 10, 11, 12); /// REGISTER SELECT PIN,ENABLE PIN,D4 PIN,D5 PIN, D6 PIN, D7 PIN
SoftwareSerial esp(2, 3); // RX TX

void setup() {

  dht.begin();
  int loading_phase = 0;
  String loading_char[3];
  loading_char[0] = "       /";
  loading_char[1] = "       -";
  loading_char[2] = "       |";
  lcd.begin(16, 2);

  lcd.createChar(0, sun_full);
  lcd.createChar(1, sun_rise);
  lcd.createChar(2, moon_full);
  lcd.createChar(3, moon_rise);
  lcd.createChar(4, moon_half);


  tempVal = analogRead(tempSensor);
  Serial.begin(9600);
  Serial.println("Starting...");
  esp.begin(9600);
  lcd.setCursor(0, 0);
  while (esp.available() < 1) {
    if (loading_phase == 3) loading_phase = 0;
    lcd.setCursor(0, 0);
    lcd.print(loading_char[loading_phase]);
    loading_phase++;
    delay(450);
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print("Asteptare IP");
  int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
  delay(50);
  char* in;
  while (esp.available() > 0 && time_date_bool) {
    in = esp.read();
    if (in != ".") {
      lcd.clear();
      ip1 = esp.parseInt();
      ip2 = esp.parseInt();
      ip3 = esp.parseInt();
      ip4 = esp.parseInt();
      time_date_h = esp.parseInt();
      time_date_m = esp.parseInt();
      time_date_s = esp.parseInt();
      delay(25);
      ip1 = ip1 + 100;
      Serial.print(ip1); Serial.print("."); Serial.print(ip2); Serial.print("."); Serial.print(ip3);Serial.print(".");Serial.println(ip4);
      Serial.print(time_date_h); Serial.print(" "); Serial.print(time_date_m); Serial.print(" "); Serial.println(time_date_s);
      time_date_bool = false;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print(ip1); lcd.print("."); lcd.print(ip2); lcd.print("."); lcd.print(ip3); lcd.print("."); lcd.print(ip4);
  time_date_s += 2;
  delay(2000);
  lcd.clear();
  servo.attach(6);
  delay(5);
  servo.write(0);
  delay(15);
  servo.detach();
}

void loop() {
  lcd.setCursor(0, 0);
  if (time_date_s >= 60) {
    lcd.clear();
    time_date_s = 0;
    time_date_m++;
    check_time++;

  }
  if (time_date_m >= 60) {
    time_date_m = 0;
    time_date_h++;
    lcd.clear();
  }
  if (time_date_h >= 24) {
    time_date_h = 0;
  }
  if (light_count == 10) {

    lightValue = analogRead(lightSensor);
    light_count = 0;
    tempVal = dht.readTemperature();
    hum = dht.readHumidity();
    //tone(A2, 650, 250);
    float temp;
    temp = tempVal;
    med_temp_val = (prev_temp_val + temp) / 2;
    med_hum = (prev_hum + hum) / 2;
    prev_hum = hum;
    prev_temp_val = temp;
    if (temp_dorita_reset && temp_dorita_bool != true) {
      temp_dorita_bool = true;
    }
  }
  
  if (alarma && time_date_h == h_alarma && time_date_m == m_alarma && time_date_s == 0) {
    servo.attach(6);
    delay(50);
    servo.write(180);
    delay(15);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TREZIREA!!");
    lcd.setCursor(0, 1);
    lcd.print("ESTE ORA "); lcd.print(time_date_h); lcd.print(":"); lcd.print(time_date_m);
    delay(500);
    int count_alarma = 0;
    int alarma_sensor = A3;
    while (alarma) {
      alarma_light = analogRead(alarma_sensor);
      alarma_light = alarma_light / 100;
      for (int i = 0; i < 1; i++) {
        tone(A2, 420, 500);
        delay(500);
        tone(A2, 350, 500);
        delay(500);
      }
      Serial.println(alarma_light);
      if (alarma_light > (lightValue / 100) + 1) {
        servo.write(0);
        delay(25);
        servo.detach();
        alarma = false;
      }
      count_alarma += 2;
    }
    lcd.clear();
    time_date_s += count_alarma;
  }
  if (alarma_birou && time_date_h == h_alarma_birou && time_date_m == m_alarma_birou && time_date_s == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("La lucru!");
    lcd.setCursor(0, 1);
    lcd.print(time_date_h); lcd.print(":"); lcd.print(time_date_m);
    for (int i = 0; i < 10; i++) {
      tone(A2, 550, 500);
      delay(500);
      tone(A2, 750, 500);
      delay(500);
    }
    time_date_s += 10;
    lcd.clear();
  }
  //alarma cu 10 minute inainte de a trebui sa fii la birou
  if (alarma_birou && time_date_h == h_alarma_birou && time_date_m == m_alarma_birou - 10 && time_date_s == 0) {
    lcd.clear();
    lcd.home();
    lcd.print("Mai sunt");
    lcd.setCursor(0, 1);
    lcd.print("10 minute");
    tone(A2, 450, 500);
    delay(1500);
    time_date_s += 2;
    lcd.clear();
  }

  //alarma la 50 de minute pentru 5 minute pauza
  if (time_date_m == 50 && time_date_s == 0 && time_date_h>=8 && time_date_h<=14) {
    lcd.clear();
    lcd.print("Ia o pauza de 5");
    lcd.setCursor(0, 1);
    lcd.print("minute");
    for (int i = 0; i < 2; i++) {
      tone(A2, 750, 500);
      delay(500);
      tone(A2, 650, 500);
      delay(500);
    }
    time_date_s += 5;
    lcd.clear();
  }

  String esp_data = "";
  boolean String_ready = false;
  while (esp.available()) {
    esp_data = esp.readString();
    String_ready = true;
    String sub_string = esp_data.substring(11, 20);
    sub_string.replace(" ", "");
    sub_string.trim();
    Serial.print("Sub string: ");
    Serial.println(sub_string);
    if (String_ready && sub_string == "alarma") {
      esp_data.remove(0, 20);
      Serial.print("esp_data: ");
      Serial.print(esp_data);
      int esp_data_toInt = esp_data.toInt();
      //Serial.println(esp_data_toInt);
      h_alarma = esp_data_toInt / 100;
      m_alarma = esp_data_toInt % 100;
      //Serial.print(h_alarma); Serial.println(m_alarma);
      alarma = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarma Setata");
      lcd.setCursor(0, 1);
      lcd.print(h_alarma); lcd.print(":"); lcd.print(m_alarma);
      tone(A2, 550, 500);
      delay(1500);
      time_date_s += 2;
    }
    if (String_ready && sub_string == "alarmb") {
      esp_data.remove(0, 20);
      Serial.print(esp_data);
      int esp_data_toInt = esp_data.toInt();
      h_alarma_birou = esp_data_toInt / 100;
      m_alarma_birou = esp_data_toInt % 100;
      alarma_birou = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reminder birou");
      lcd.setCursor(0, 1);
      lcd.print("setat");
      tone(A2, 250, 500);
      delay(1500);
      time_date_s += 2;
    }
    if (String_ready && sub_string == "rpauza") {
      esp_data.remove(0, 20);
      int esp_data_toInt = esp_data.toInt();
      pauza_left = esp_data_toInt;
      pauza = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pauza ");
      lcd.print(pauza_left);
      lcd.print(" minute");
      tone(A2, 440, 500);
      delay(1000);
      time_date_s += 2;
    }
    if (String_ready && sub_string == "temper") {
      esp_data.remove(0, 20);
      int esp_data_toInt = esp_data.toInt();
      temp_dorita = esp_data_toInt;
      if (temp_dorita == 0 ) {
        temp_dorita_bool = false;
        temp_dorita_reset = false;
      } else {
        temp_dorita_bool = true;
        temp_dorita_reset = true;
        lcd.clear(); lcd.home();
        lcd.print("Temperatura de");
        lcd.setCursor(0, 1);
        lcd.print(temp_dorita); lcd.print(char(223)); lcd.print("C setata");
        delay(2000);
        time_date_s += 2;
      }
    }
    if (String_ready && sub_string == "TIME") {
      time_date_h = esp.parseInt();
      time_date_m = esp.parseInt();
      time_date_s = esp.parseInt();
      time_date_s++;
    }
    lcd.clear();
  }
  if (check_time == 15) {
    esp.println("TIME");
    check_time = 0;
    time_date_s++;
    esp.println("");
  }

  lcd.setCursor(0, 0);
  lcd.print(time_date_h); lcd.print(":"); lcd.print(time_date_m); lcd.print(":"); lcd.print(time_date_s); lcd.print(" ");
  lcd.print(abs(med_temp_val)); lcd.print(char(223)); lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Lumina "); lcd.print(lightValue / 100); lcd.print(" ");
  if (time_date_h >= 6 && time_date_h < 9)  lcd.write(byte(1));
  if (time_date_h >= 9 && time_date_h < 19) lcd.write(byte(0));
  if (time_date_h >= 19 && time_date_h < 21) lcd.write(byte(3));
  if (time_date_h >= 21 && time_date_h < 0) lcd.write(byte(4));
  if (time_date_h >= 0 && time_date_h < 6) lcd.write(byte(2));

  lcd.print(" ");
  lcd.print(int(med_hum));
  lcd.print("% ");
  if (pauza) lcd.print(pauza_left);
  if (pauza_count == 60) {
    pauza_count = 0;
    pauza_left--;
  }

  if (pauza_left == 0) {
    pauza = false;
    lcd.clear();
    lcd.home();
    lcd.print("Pauza s-a");
    lcd.setCursor(0, 1);
    lcd.print("terminat");
    tone(A2, 550, 500);
    delay(500);
    tone(A2, 1024, 500);
    delay(500);
    time_date_s++;
    pauza = 1;
  }


  if (temp_dorita < med_temp_val && temp_dorita_bool) {
    lcd.clear(); lcd.home();
    lcd.print("Temperatura e");
    lcd.setCursor(0, 1);
    lcd.print("peste nivelul setat");
    tone(A2, 130, 250);
    delay(1000);
    time_date_s++;
    temp_dorita_bool = false;
    lcd.clear();
  }
  if (temp_dorita > med_temp_val && temp_dorita_bool) {
    lcd.clear(); lcd.home();
    lcd.print("Temperatura e");
    lcd.setCursor(0, 1);
    lcd.print("sub nivelul setat");
    tone(A2 , 130, 250);
    delay(1000);
    time_date_s++;
    temp_dorita_bool = false;
    lcd.clear();
  }
  delay(999);
  //adauga la counter ul pentru verificarea luminii si modifica temperatura trecuta pentru a putea face o medie intre temperatura pe care o masoara si cea pe care a masurat-o cu o secunda inainte
  light_count++;
  time_date_s++;
  if (pauza) pauza_count++;
}
