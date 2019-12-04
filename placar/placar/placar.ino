#include <LiquidCrystal.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#define syncTime 10000
#define tx 9
#define rx 10
#define buttonInit 8

int pontuacaoA=0, pontuacaoB=0;
int cod = 48;//0
int pres=0, qtdPlayers=0;
long tempoAtual=0, tempo, tempoInicioJogo;

enum nomeCodigos {ponto1, ponto2, pareando1, pareando2, comeco_jogo, fim_jogo, pareado1, pareado2};
int codigos[] = {1, 2, 3, 4, 5, 6, 9, 10};

enum estados {INICIO, CONFIGURAR_REDE, EM_ESPERA, ESCOLHER_DURACAO, CONF_QUNATIDADE_JOGADORES, JOGO, MOSTRAR_PLACAR, DEBUG};
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
  estado = DEBUG; //trocar para INICIO
  //Serial.println(estado);
}


//vai esperar enviem algum dado, se receber então foi pareado
/*
void search(){
  if(HC12.available()){
    cod=HC12.read()-48;
    if(cod == codigos[pareando1] || cod == codigos[pareando2] )
    qtdPlayers++;
    lcd.setCursor(0, 0);
    lcd.print("Jogador:"); lcd.print( (cod % 2) + 1); lcd.print("entrou");
    lcd.setCursor(0, 1);
    lcd.print(qtdPlayers); lcd.print(" players");
  }
}
*/

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
      HC12.write(codigos[pareado1]+48);
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
        lcd.print("Waiting  Players");
        tempoAtual=millis();
        qtdPlayers = 0;
        estado = CONFIGURAR_REDE;
      }
    break;
    case CONFIGURAR_REDE: //pareamento com jogadores
      Serial.println("CONFIGURAR_REDE");
      if(!digitalRead(buttonInit) && !pres){
        pres=!pres;
      }
      else if(digitalRead(buttonInit) && pres){
        pres=!pres;
        tempoAtual = 0;
        estado = INICIO;
      }
      else if ( ((millis() - tempoAtual) <= syncTime) && (qtdPlayers<2) ) {
        //search();
        if(HC12.available()){
          cod=HC12.read()-48;
          //Serial.println(cod);
          if(cod == codigos[pareando1] || cod == codigos[pareando2] ){
            qtdPlayers++;
            Serial.println((cod)%2 == 1 ? codigos[pareado1]+48 : codigos[pareado2]+48);
            HC12.write( (cod)%2 == 1 ? codigos[pareado1]+48 : codigos[pareado2]+48); 
            lcd.setCursor(0, 0);
            lcd.print("Jogador:"); lcd.print( (cod % 2) + 1); lcd.print(" entrou");
            lcd.setCursor(0, 1);
            lcd.print(qtdPlayers); lcd.print(" players");
          }
        }
      }
      else if( ((millis() - tempoAtual) > syncTime) && (qtdPlayers>1) ){
        lcd.setCursor(0, 0);
        lcd.print("Esperando para   ");
        lcd.setCursor(0, 1);
        lcd.print("Comecar jogo     ");
        estado=EM_ESPERA;
        tempoAtual=millis();
      }
      else if(((millis() - tempoAtual) > syncTime)) {
        tempoAtual = 0;
        estado = INICIO;
      } 
    break;
    case EM_ESPERA:
      Serial.println("EM_ESPERA");
      if(!digitalRead(buttonInit) && !pres){
        pres=!pres;
      }
      else if(digitalRead(buttonInit) && pres){
        pres=!pres;
        estado = ESCOLHER_DURACAO;
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
        /*
        lcd.setCursor(0, 1);
        lcd.print("                ");
        */
        lcd.setCursor(0, 1);
        tempo=tempoJogo();
        lcd.print("select: "); lcd.print(tempo); lcd.print(" min");
      }
      else if(!digitalRead(buttonInit) && pres){
        if((millis()-tempoAtual)>= 750){
          estado=JOGO;
          pontuacaoA = pontuacaoB = 0;
          tempo = tempo*60*1000;
          lcd.setCursor(0,0);
          (qtdPlayers == 2) ? lcd.print(" P1   tempo   P2  ") : lcd.print("  Placar  tempo  ");
          lcd.setCursor(0,1);
          lcd.print("                ");
        }
      }
      tempoInicioJogo=millis();
    break;
    case JOGO:
      Serial.println("JOGO");
      tempoAtual = millis();
      if (HC12.available()) {
        cod = HC12.read();
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
      lcd.setCursor( ((qtdPlayers == 1) ? 11 : 6 ), 1);
      lcd.print(gameTime(tempoAtual/1000 - tempoInicioJogo/1000));
      if(tempoAtual - tempoInicioJogo > tempo ) {
        estado = MOSTRAR_PLACAR;
        HC12.write(codigos[fim_jogo]);
      }
    break;
    case MOSTRAR_PLACAR:
      Serial.println("MOSTRAR_PLACAR");
    break;
  }
}
