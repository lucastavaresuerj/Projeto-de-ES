//cesta
#include<SoftwareSerial.h>
#include "Ultrasonic.h"
#include "notas.h"
#define tx 11
#define rx 10
#define btn 2
#define buzzer 3
#define trig 8
#define echo 9
#define timeOut 30000
#define timeOutMusica 60000
#define cesta1 true // false for cesta2

enum nomeCodigos {ponto, pareado, comeco_jogo, fim_jogo, pareando};
#if cesta1 //TODO remover na tabela de codigos os que nao sao usados
int codigos[] = {1, 9, 5, 6, 3};
#else
int codigos[] = {2, 10, 5, 6, 4};
#endif

Ultrasonic ultrasonic(trig,echo); //INICIALIZANDO OS PINOS

int cod = 0; //codigos que serao lidos dos receptores
long distancia;
int btnEstado = HIGH;
unsigned long tempoInicio, tempoAtual;
enum estados {INICIO, CONFIGURAR_REDE, EM_ESPERA, JOGO, TOCAR_MUSICA, DEBUG };
estados estado;
SoftwareSerial HC12(tx,rx);

void setup() {
  HC12.begin(9600);
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(btn, INPUT);
  pinMode(echo, INPUT); //DEFINE O PINO COMO ENTRADA (RECEBE)
  pinMode(trig, OUTPUT); //DEFINE O PINO COMO SAÍDA (ENVIA)
  Ultrasonic ultrasonic(trig,echo); //INICIALIZANDO OS PINOS
  estado = DEBUG; // trocar para INICIO
  tempoInicio = millis();
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
          tempoInicio = millis();
          estado = CONFIGURAR_REDE;
        }  
        break;
      case CONFIGURAR_REDE:
        Serial.println("CONFIGURAR_REDE");
        tempoAtual = millis();
        if( HC12.available() ) {
          cod = HC12.read()-48;
          Serial.println(cod);
          if (cod == codigos[pareado]) {
            estado = EM_ESPERA;
          }
        }
        else if(tempoAtual - tempoInicio > timeOut) {
          estado = CONFIGURAR_REDE; // trocar para INICIO
          tempoInicio = millis();
        }
        else {
           HC12.write(codigos[pareando]+48);
        }
        break;
      case EM_ESPERA: //TODO mudar na maquina de estados, agora muda de estado quando o placar manda
        Serial.println("EM_ESPERA");
        if(HC12.available()){
          cod = HC12.read()-48;
          if(cod==codigos[comeco_jogo]){
            estado = JOGO;
          }
        }
        break;
      case JOGO:
        Serial.println("JOGO");
        distancia = ultrasonic.Ranging(CM);
        if(distancia < 10) {
          Serial.print("cesta");
          HC12.write(codigos[ponto]+48);
          tone(buzzer,NOTE_C3,1000/4);
          noTone(buzzer);
        }
        else if(HC12.available()){
          cod = HC12.read()-48;
          if(cod == codigos[fim_jogo]) {
            tempoInicio = millis();
            estado = TOCAR_MUSICA;
          }
        }
        break;
      case TOCAR_MUSICA:
        Serial.println("TOCAR_MUSICA");
        tempoAtual = millis();
        if(!digitalRead(btn) && btnEstado) {
          btnEstado = LOW;
        } 
        else if(digitalRead(btn) != btnEstado) {
          btnEstado = HIGH;
          tempoInicio = millis();
          estado = EM_ESPERA;
        }
        else if(tempoAtual - tempoInicio > timeOutMusica) {
          estado = EM_ESPERA;
        }
        /*
         * 
         * ESPACO RESERVADO PARA TOCAR A MUSICA
         * SEM TRAVAR O SISTEMA
         * COMO?
         * EU NAO SEI AINDA
         * 
         */
        break;
    }
}
