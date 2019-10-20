//cesta
#include<SoftwareSerial.h>
#include "Ultrasonic.h"
#include "notas.h"
#define txA 11
#define rxA 10
#define txB 6
#define rxB 5
#define switchbtn 2
#define buzzer 3
#define trig 8
#define echo 9

Ultrasonic ultrasonic(trig,echo); //INICIALIZANDO OS PINOS

int estado = 0;
int distancia;
int btn = HIGH;

SoftwareSerial HC12A(txA,rxA);
SoftwareSerial HC12B(txB,rxB);
//SoftwareSerial HC12(txB,rxB);

void setup() {
  HC12A.begin(9600);
  HC12B.begin(9600);
  //HC12.begin(9600);
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(switchbtn, INPUT);
  pinMode(echo, INPUT); //DEFINE O PINO COMO ENTRADA (RECEBE)
  pinMode(trig, OUTPUT); //DEFINE O PINO COMO SAÍDA (ENVIA)
  Ultrasonic ultrasonic(trig,echo); //INICIALIZANDO OS PINOS
}

void loop() {
    Serial.print("estado: ");Serial.println(estado);
    switch(estado) {
      case 0:
        hcsr04();
        if(distancia < 10) {
          Serial.println("cesta A");
          HC12A.write(49);
          //HC12.write(49);
          tone(buzzer,NOTE_C3,1000/4);
          noTone(buzzer);
        }
        else if(!digitalRead(switchbtn) && btn) {
          btn = LOW;
        } 
        else if(digitalRead(switchbtn) != btn) {
          btn = HIGH;
          estado = 1;
        }
        break;
      case 1:
        hcsr04();
        if(distancia < 10) {
          Serial.print("cesta B");
          HC12B.write(50);
          //HC12.write(50);
          tone(buzzer,NOTE_C3,1000/4);
          noTone(buzzer);
        }
        else if(!digitalRead(switchbtn) && btn) {
          btn = LOW;
        } 
        else if(digitalRead(switchbtn) != btn) {
          btn = HIGH;
          estado = 0;
        }
        break;
    }
}

void hcsr04(){
  digitalWrite(trig, LOW); //SETA O PINO 6 COM UM PULSO BAIXO "LOW"
  delayMicroseconds(2); // DELAY DE 2 MICROSSEGUNDOS
  digitalWrite(trig, HIGH); //SETA O PINO 6 COM PULSO ALTO "HIGH"
  delayMicroseconds(10); // DELAY DE 10 MICROSSEGUNDOS
  digitalWrite(trig, LOW); //SETA O PINO 6 COM PULSO BAIXO "LOW" NOVAMENTE
  // FUNÇÃO RANGING, FAZ A CONVERSÃO DO TEMPO DE
  //RESPOSTA DO ECHO EM CENTÍMETROS E ARMAZENA
  //NA VARIÁVEL "distancia"
  distancia = (ultrasonic.Ranging(CM)); // VARIÁVEL GLOBAL RECEBE O VALOR DA DISTÂNCIA MEDIDA
}
