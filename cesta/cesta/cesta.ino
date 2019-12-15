//cesta
#include<SoftwareSerial.h>
#include "Ultrasonic.h"
#define tx 11
#define rx 10
#define btn 2
#define buzzer 3
#define trig 9
#define echo 8
#define cesta1 true // false for cesta2

enum nomeCodigos {ponto, comeco_jogo, fim_jogo, };
#if cesta1 //TODO remover na tabela de codigos os que nao sao usados
int codigos[] = {1, 3, 4};
#else
int codigos[] = {2, 3, 4};
#endif

long unsigned ultimaCesta = 0, tempoAtual;
int cod = 0; //codigos que serao lidos dos receptores
long distancia;
int btnEstado = HIGH;
enum estados {INICIO, EM_ESPERA, JOGO, DEBUG };
estados estado;

SoftwareSerial HC12(tx,rx);
Ultrasonic ultrasonic(trig,echo); //INICIALIZANDO OS PINOS


void setup() {
  HC12.begin(9600);
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(btn, INPUT);
  pinMode(echo, INPUT); //DEFINE O PINO COMO ENTRADA (RECEBE)
  pinMode(trig, OUTPUT); //DEFINE O PINO COMO SAÍDA (ENVIA)
  Ultrasonic ultrasonic(trig,echo); //INICIALIZANDO OS PINOS
  estado = EM_ESPERA; // trocar para INICIO
}

void loop() {
    //Serial.print("estado: ");Serial.println(estado);
    switch(estado) {
      case DEBUG:
        Serial.println("DEBUG");
        if (HC12.available()) {
          Serial.write(HC12.read());
        }
        
      break;
      case INICIO:
      Serial.println("INICIO");
        if(!digitalRead(btn) && btnEstado) {
          btnEstado = LOW;
        } 
        else if(digitalRead(btn) != btnEstado) {
          btnEstado = HIGH;
          estado = EM_ESPERA;
        }  
        break;
      case EM_ESPERA: 
        Serial.println("EM_ESPERA");
        if(HC12.available()){
          cod = HC12.read() - 48;
          if(cod == codigos[comeco_jogo]){
            estado = JOGO;
          }
        }
        break;
      case JOGO:
        Serial.println("JOGO");
        tempoAtual = millis();
        distancia = ultrasonic.Ranging(CM);
        Serial.println(distancia);
        if( (distancia < 10) && (tempoAtual - ultimaCesta > 2000) ) {
          ultimaCesta = tempoAtual;
          //Serial.print("cesta");
          HC12.write(codigos[ponto]+48);
        }
        else if(HC12.available()){
          cod = HC12.read() - 48;
          if(cod == codigos[fim_jogo]) {
            estado = EM_ESPERA;
          }
        }
        break;
    }
}
