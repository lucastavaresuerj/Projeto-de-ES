#include <LiquidCrystal.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#define syncTime 10000
#define tx 9
#define rx 10
#define buttonInit 8

int pontuacaoA=0, pontuacaoB=0;
int cod = 48;//0
int pres=false, qtdPlayers=0;
long tempoAtual=0, tempo, tempoInicioJogo;

enum nomeCodigos {ponto1, ponto2, comeco_jogo, fim_jogo};
int codigos[] = {1, 2, 3, 4};

enum estados {INICIO, ESCOLHER_DURACAO, CONF_QUANTIDADE_JOGADORES, JOGO, MOSTRAR_PLACAR, DEBUG};
estados estado;

SoftwareSerial HC12(tx,rx);
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // set up the LCD's number of columns and rows:
  HC12.begin(9600);
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(buttonInit, INPUT);
  estado = INICIO; //trocar para INICIO
  //Serial.println(estado);
}

int tempoJogo(){
  static int duracao[]={2, 3, 5, 10, 15}, duracaoEscolhida=-1;
  static int duracaoSize = sizeof(duracao)/sizeof(duracao[0]);
  duracaoEscolhida = (duracaoEscolhida + 1) % duracaoSize; 
  // sim eu sei que e a mesma coisa de (++it)%=tm, mas tava quase te mandando mensagem para perguntar oq era tm
  //static const int tm = 5;
  //(++it)%=tm;
  return duracao[duracaoEscolhida];
  
}

String gameTime(long tempoTanscorrido){
  tempoTanscorrido = tempo/1000 - tempoTanscorrido;
  static long seg, mn;
  if(tempoTanscorrido<60){
    return String(String(" ") + String(tempoTanscorrido) + String("  ")); 
  }
  mn = tempoTanscorrido/60;
  seg = tempoTanscorrido%60;
  return String( (mn<10 ? String("0" + String(mn)) : String(mn) ) + ":" + (seg<10 ? String("0" + String(seg)) : String(seg) ));
}

void loop() {
  switch(estado) {
    case DEBUG:
      Serial.println("DEBUG");
      HC12.write("DEBUG");
      break;
    case INICIO:
      Serial.println("INICIO");//WELLCOME
      if(tempoAtual == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Welcome to      ");
        lcd.setCursor(0, 1);
        lcd.print("Basketball Game ");
      }
      if(!digitalRead(buttonInit) && !pres){
        pres=!pres;
      }
      else if(digitalRead(buttonInit) && pres){
        pres=!pres;
        lcd.clear();
        lcd.print("Set Players ");
        tempoAtual=millis();
        qtdPlayers = 0;
        estado = CONF_QUANTIDADE_JOGADORES;
      }
      break;
    case CONF_QUANTIDADE_JOGADORES:
      if(!digitalRead(buttonInit) && !pres){
        pres=!pres;
        tempoAtual = millis();
      }
      else if(digitalRead(buttonInit) && pres){
        pres=!pres;
        qtdPlayers = qtdPlayers%2 + 1;
        lcd.setCursor(0, 1);
        lcd.print("select: "); lcd.print(qtdPlayers); 
      }
      else if( (!digitalRead(buttonInit) && pres) && ((millis()-tempoAtual)>= 750) ){
        pres=!pres;
        estado=ESCOLHER_DURACAO;
        lcd.clear();
        lcd.print("  SET TIME GAME  ");
      }
      break;
    case ESCOLHER_DURACAO: //duracao da partida
      Serial.println("ESCOLHER_DURACAO"); 
      //verifica se foi pressionado, sim->muda de estado, não->permanece e muda duração
      if(!digitalRead(buttonInit) && !pres){
        pres=!pres;
        tempoAtual = millis();
      }
      else if(digitalRead(buttonInit) && pres){
        pres=!pres;
        lcd.setCursor(0, 1);
        tempo=tempoJogo();
        lcd.print("select: "); lcd.print(tempo); lcd.print(" min");
      }
      else if(!digitalRead(buttonInit) && pres){
        if((millis()-tempoAtual)>= 750){
          pres=!pres;
          estado=JOGO;
          pontuacaoA = pontuacaoB = 0;
          tempo = tempo*60*1000;
          lcd.setCursor(0,0);
          (qtdPlayers == 2) ? lcd.print(" P1   TIME   P2 ") : lcd.print("  SCORE  TIME  ");
          lcd.setCursor(0,1);
          lcd.print("                ");
          HC12.write(codigos[comeco_jogo] + 48);
        }
      }
      tempoInicioJogo=millis();
    break;
    case JOGO:
      Serial.println("JOGO");
      tempoAtual = millis();
      if (HC12.available()) {
        cod = HC12.read() - 48;
        if(cod == codigos[ponto1]) {
          pontuacaoA++;
        }
        else if(cod == codigos[ponto2]) {
          pontuacaoB++;
        }
        if(qtdPlayers == 1) {
          lcd.setCursor(5,1);
          lcd.print(pontuacaoA + pontuacaoB);
          lcd.setCursor(11,1);
          
        }
        else{
          lcd.setCursor(1,1);
          lcd.print(pontuacaoA);
          lcd.setCursor(14,1);
          lcd.print(pontuacaoB);
        }
      }
      lcd.setCursor( ((qtdPlayers == 1) ? 9 : 6 ), 1);
      lcd.print(gameTime(tempoAtual/1000 - tempoInicioJogo/1000));
      if(tempoAtual - tempoInicioJogo > tempo ) {
        HC12.write(codigos[fim_jogo] + 48);
        tempoAtual = millis();
        estado = MOSTRAR_PLACAR;
      }
    break;
    case MOSTRAR_PLACAR:
      Serial.println("MOSTRAR_PLACAR");
      if(millis() - tempoAtual > 20000) {
        tempoAtual = 0;
        estado = INICIO;
      }
    break;
  }
}
