int plus = 5;
int minus = 6;


void setup() {
  Serial.begin(9600);
  pinMode(plus, OUTPUT);
  pinMode(minus, OUTPUT);
}

void loop() {
  String serial = "";
  while (Serial.available() > 0) {
    serial = Serial.readString();
    if (serial == "jos") {
      Serial.println("JOS");
      analogWrite(plus,100);
      digitalWrite(minus, LOW);
      delay(1500);
    }
    if (serial == "stop") {
      digitalWrite(plus, LOW);
      digitalWrite(minus, LOW);

    }
    if (serial == "sus") {
      digitalWrite(plus, LOW);
      analogWrite(minus, 100);
    }
  }
}
