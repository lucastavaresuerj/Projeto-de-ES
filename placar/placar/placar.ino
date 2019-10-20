//receptor
#include<SoftwareSerial.h>
#define tx 10
#define rx 11
#define pinledA 3
#define pinledB 4

bool ledA = false;
bool ledB = false;
int op = 48;//0

SoftwareSerial HC12(tx,rx);

void setup() {
  HC12.begin(9600);
  Serial.begin(9600);
  pinMode(pinledA, OUTPUT);
  pinMode(pinledB, OUTPUT);
}

void loop() {
  while(HC12.available()) {
    op = HC12.read();
    digitalWrite(op-48+2,HIGH);
    Serial.println(op);
    switch(op) {
      case 49:
        ledA = HIGH;
        ledB = LOW;
        digitalWrite(pinledB,LOW);
        break;
      case 50:
        ledB = HIGH;
        ledA = LOW;
        digitalWrite(pinledA,LOW);
        break;
    }
    
  }
}
