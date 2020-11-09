#include <LiquidCrystal.h>
#include <SoftwareSerial.h>



// initialize the library with the numbers of the interface pins
int tempSensor = 0, tempVal = 0, prev_temp_val = 0, med_temp_val = 0;
int lightSensor = 1, lightValue = 0, light_count = 10;
int piezoPin = A2;
bool time_date_bool = true;
int time_date_h = 0, time_date_m, time_date_s;
bool alarma = false;
int h_alarma = 0, m_alarma = 0;

byte sun_char[] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
};

byte first_moon[] = {
  B00111,
  B01100,
  B11000,
  B10000,
  B10000,
  B11000,
  B01100,
  B00111,
};

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); /// REGISTER SELECT PIN,ENABLE PIN,D4 PIN,D5 PIN, D6 PIN, D7 PIN
SoftwareSerial esp(2, 3);

void setup() {


  int loading_phase = 0;
  String loading_char[3];
  loading_char[0] = "       /";
  loading_char[1] = "       -";
  loading_char[2] = "       |";
  lcd.begin(16, 2);

  lcd.createChar(0, sun_char);
  lcd.createChar(1, first_moon);

  tempVal = analogRead(tempSensor);
  Serial.begin(9600);
  Serial.println("Starting...");
  esp.begin(9600);
  lcd.setCursor(0, 0);
  while (esp.available() < 1) {
    if (loading_phase == 3) loading_phase = 0;
    lcd.setCursor(0, 0);
    //Serial.print(loading_char[loading_phase]);
    lcd.print(loading_char[loading_phase]);
    loading_phase++;
    delay(500);
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print("Asteptare IP");
  int ip1 = 0, ip2 = 0, ip3 = 0, trash = 0;
  delay(50);
  char* in;
  while (esp.available() > 0 && time_date_bool) {
    in = esp.read();
    if (in != ".") {
      lcd.clear();
      ip1 = esp.parseInt();
      ip2 = esp.parseInt();
      trash = esp.parseInt();
      ip3 = esp.parseInt();
      time_date_h = esp.parseInt();
      time_date_m = esp.parseInt();
      time_date_s = esp.parseInt();
      delay(25);
      ip1 = ip1 + 100;
      Serial.print(ip1); Serial.print("."); Serial.print(ip2); Serial.print("."); Serial.println(ip3);
      Serial.print(time_date_h); Serial.print(" "); Serial.print(time_date_m); Serial.print(" "); Serial.println(time_date_s);
      time_date_bool = false;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print(ip1); lcd.print("."); lcd.print(ip2); lcd.print("."); lcd.print(ip3);
  delay(2000);
  time_date_s += 2;
  lcd.clear();
}




void loop() {
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
      Serial.println(esp_data_toInt);
      h_alarma = esp_data_toInt / 100;
      m_alarma = esp_data_toInt % 100;
      Serial.print(h_alarma); Serial.println(m_alarma);
      alarma = true;



      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarma Setata");
      lcd.setCursor(0, 1);
      lcd.print(h_alarma); lcd.print(":"); lcd.print(m_alarma);
      delay(1000);
    }
    if (String_ready && sub_string == "pauzam") {
      esp_data.remove(0, 20);
      esp_data.trim();
      Serial.print(esp_data);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(esp_data);
      delay(1000);
    }
    time_date_s += 1;
  }

  if (time_date_s >= 60) {
    time_date_s = 0;
    time_date_m++;
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
    tempVal = analogRead(tempSensor);
    //tone(A2, 650, 250);
    float temp = tempVal * 5.0;
    temp = temp / 1024.0;
    temp = int((temp - 0.6) * 100);
    med_temp_val = (prev_temp_val + temp) / 2;
    prev_temp_val = temp;
  }

  lcd.setCursor(0, 0);
  lcd.print(time_date_h);//print name
  lcd.print(":");
  lcd.print(time_date_m);
  lcd.print(" temp:"); lcd.print(abs(med_temp_val)); lcd.print(char(223)); lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Lumina "); lcd.print(lightValue / 100);



  lcd.write(byte(2));

  if (alarma && time_date_h == h_alarma && time_date_m == m_alarma) {
    for (int i = 0; i < 5; i++) {
      tone(A2, 420, 500);
      delay(500);
      tone(A2, 350, 500);
      delay(500);
      alarma = false;
    }
    time_date_s += 2;
  }

  delay(1000);
  //adauga la counter ul pentru verificarea luminii si modifica temperatura trecuta pentru a putea face o medie intre temperatura pe care o masoara si cea pe care a masurat-o cu o secunda inainte
  light_count++;
  time_date_s++;


}
