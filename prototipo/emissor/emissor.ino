//emissor
#include<SoftwareSerial.h>
#define tx 10
#define rx 11
#define pinbtn 4

bool btn = HIGH;

SoftwareSerial HC12(tx,rx);

void setup() {
  HC12.begin(9600);
  Serial.begin(9600);
  pinMode(pinbtn, INPUT);
}

void loop() {
  if(!digitalRead(pinbtn) && btn) {
    btn = LOW;
  } else if(digitalRead(pinbtn) != btn) {
    //Serial.println("btn");
    HC12.write(49);
    btn = HIGH;
  }
}
