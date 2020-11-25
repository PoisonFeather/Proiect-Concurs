#include <Servo.h>

Servo servo;

int pos=180;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ready");
  servo.attach(5);
  delay(15);
  servo.write(0);
  delay(25);
  
}

void loop() { 
 for(int i=0;i<180;i++){
    servo.write(i);
    delay(15);
  }
 for(int j=180;j>0;j--){
    servo.write(j);
    delay(15);
  }
}
