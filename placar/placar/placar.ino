//Multifunction Module - Display
#include <TimerOne.h>                     // Bibliotec TimerOne 
#include <Wire.h>                         // Biblioteca Wire 
#include <MultiFuncShield.h>              // Biblioteca Multifunction shield

int pontuacaoA=0, pontuacaoB=0;
//receptor
#include<SoftwareSerial.h>
#define tx 10
#define rx 11
#define pinledA 3
#define pinledB 2//mudei do pin 4 para o 12 green light

int op = 48;//0

SoftwareSerial HC12(tx,rx);

void setup() {
  Timer1.initialize();                    // inicializa o Timer 1
  MFS.initialize(&Timer1);                // initializa a biblioteca Multi função
  
  HC12.begin(9600);
  Serial.begin(9600);
  pinMode(pinledA, OUTPUT);
  pinMode(pinledB, OUTPUT);
}

void loop() {
  while(HC12.available()) {
    op = HC12.read();
    Serial.write(op);
    if(op==49){
      MFS.write((int)(++pontuacaoA));
      digitalWrite(pinledA, op==49);  
    }
    else{
      MFS.write((int)(++pontuacaoB));
      digitalWrite(pinledB, op==50);  
    }
    if((pontuacaoA==120) || (pontuacaoB==120)){
      MFS.write("OVER");
      break;
    }
  }
}

//https://www.cohesivecomputing.co.uk/hackatronics/arduino-multi-function-shield/bonus-sketches/
//https://blog.eletrogate.com/guia-completo-do-shield-multi-funcoes-para-arduino/#Aplica%C3%A7%C3%A3o%20para%20Display%207%20Segmentos

//Sketch > Incluir Biblioteca > Gerenciar Bibliotecas -- TimerOne
