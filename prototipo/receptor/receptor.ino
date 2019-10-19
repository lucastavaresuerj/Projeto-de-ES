//receptor
#include<SoftwareSerial.h>
#define tx 10
#define rx 11
#define pinled 13

bool led = false;
int op = 48;

SoftwareSerial HC12(tx,rx);

void setup() {
  HC12.begin(9600);
  Serial.begin(9600);
  pinMode(pinled, OUTPUT);
}

void loop() {
  while(HC12.available()) {
    op = HC12.read();
    if(op == 49) {
      //Serial.write(op);
      led = !led;
      digitalWrite(pinled, led);
    }
  }
}
