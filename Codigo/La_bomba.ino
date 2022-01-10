#include <LiquidCrystal.h>
#include <Servo.h>


#define FIO_LR 22
#define FIO_VD 23
#define FIO_RX 24
#define LED_VM 3
#define LED_AM 4
#define LED_AZ 5
#define SERVOMOTOR 6
#define BUZZ 13

LiquidCrystal lcd(42, 44, 46, 48, 50, 52);
Servo porta;

int valor_tecla = 0;
int senha = 0;
int led_sete = 0, led_cinco = 0, led_um = 0;
boolean aux = 0;
int digito_um = 0, digito_dois = 0;
int cor = 0;
int fio_lr = 0, fio_vd = 0, fio_rx = 0;
int tempo = 60;

void setup() {
  randomSeed(analogRead(A7));
  
  porta.attach(SERVOMOTOR);
  delay(15);
  porta.write(80);
  delay(150);
  
  pinMode(A0, OUTPUT);        //Pinos ligados aos pinos 1, 2 e 3 do teclado - Linhas
  pinMode(A1, OUTPUT);        //Pinos ligados aos pinos 1, 2 e 3 do teclado - Linhas
  pinMode(A2, OUTPUT);        //Pinos ligados aos pinos 1, 2 e 3 do teclado - Linhas
  pinMode(A3, INPUT);         //Pinos ligados aos pinos 5, 6 e 7 do teclado - Colunas
  pinMode(A4, INPUT);         //Pinos ligados aos pinos 5, 6 e 7 do teclado - Colunas
  pinMode(A5, INPUT);         //Pinos ligados aos pinos 5, 6 e 7 do teclado - Colunas
  pinMode(FIO_LR, INPUT);     // Fio Laranja
  pinMode(FIO_VD, INPUT);     // Fio Verde
  pinMode(FIO_RX, INPUT);     // Fio Roxo
  pinMode(LED_VM, OUTPUT);    // LED VM
  pinMode(LED_AM, OUTPUT);    // LED AM
  pinMode(LED_AZ, OUTPUT);    // LED Az
  pinMode(BUZZ, OUTPUT);      // BUZZER
  Serial.begin(9600);
  // Configuração do timer1 
  noInterrupts(); 
  TCCR1A = 0;                        // Confira timer para operação normal pinos OC1A e OC1B desconectados
  TCCR1B = 0;                        // Limpa registrador
  TCCR1B |= (1<<CS10)|(1 << CS12);   // Configura prescaler para 1024: CS12 = 1 e CS10 = 1
  TCNT1 = 0xC2F7;                    // Incia timer com valor para que estouro ocorra em 1 segundo  65536-(16MHz/1024/1Hz) = 49911 = 0xC2F7
  TIMSK1 |= (1 << TOIE1);            // Habilita a interrupção do TIMER1
  
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    La Bomba    ");
  porta.attach(SERVOMOTOR);
  delay(15);
  porta.write(85);
  delay(150);
  digitalWrite(LED_VM, LOW);
  digitalWrite(LED_AM, LOW);
  digitalWrite(LED_AZ, LOW);
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("    Aperte 1    ");
  lcd.setCursor(0,1);
  lcd.print("  para Iniciar  ");
}
 
void loop() {
  while(le_teclado() !=  1) {
    // Espera tecla 1 ser presionada
  }
  interrupts();
  tone(BUZZ, 500, 500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Olhe o Codigo  ");
  aux = 0;

  senha = random(13,99);
  //senha = map(senha, 0, 1023, 13,99);
  separa_num();
  Serial.println(senha);
  pisca_led(1,led_sete);
  pisca_led(2,led_cinco);
  pisca_led(3,led_um);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Digite o Codigo:");

  
  while (aux == 0) {
    digito_um = le_teclado();  
  }
  lcd.setCursor(0,1);
  lcd.print(digito_um);
  aux = 0;
  
  while (aux == 0) {
    digito_dois = le_teclado();  
  }
  lcd.setCursor(1,1);
  lcd.print(digito_dois);
    
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Verificando.");
  delay(500);
  lcd.setCursor(0,0);
  lcd.print("Verificando..");
  delay(500);
  lcd.setCursor(0,0);
  lcd.print("Verificando...");
  
  int valor_user = (digito_um * 10) + digito_dois;
  if (valor_user == senha){
    lcd.setCursor(0,0);
    lcd.print("     Correto     ");
    tone(BUZZ, 500, 100);
    delay(2000);
    porta.write(10);  
    delay(500);  
    lcd.setCursor(0,0);
    lcd.print("Encontre a Cor   ");
    cor = random(1,3);
    if (cor == 1) {
      digitalWrite(LED_VM, HIGH);
      digitalWrite(LED_AM, HIGH);
      digitalWrite(LED_AZ, LOW);
    }
    else if (cor == 2) {
      digitalWrite(LED_VM, HIGH);
      digitalWrite(LED_AM, LOW);
      digitalWrite(LED_AZ, HIGH);
    }
    else if (cor == 3) {
      digitalWrite(LED_VM, LOW);
      digitalWrite(LED_AM, HIGH);
      digitalWrite(LED_AZ, HIGH);
    }
    
    while(le_fios() == 0) {
    }
    Serial.println(fio_lr);
    Serial.println(fio_rx);
    Serial.println(fio_vd);
    if ((cor == 1) && (fio_lr != 0)) {
      correto();
    }
    else {
      incorreto();
    }
    if ((cor == 2) && (fio_rx != 0)) {
      correto();
    }
    else {
      incorreto();
    }
    if ((cor == 3) && (fio_vd != 0)) {
      correto();
    }
    else {
      incorreto();
    }
  }
  else {
    incorreto();
  } 
}

ISR(TIMER1_OVF_vect)                              //interrupção do TIMER1 
{
  TCNT1 = 0xC2F7;                                 // Renicia TIMER
  lcd.setCursor(14,1);
  lcd.print(tempo);
  tempo--;
  if (tempo < 1) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Fim do Tempo");
    tone(BUZZ, 1000, 1000);
  }
  
}
void separa_num() {
  int resto = (senha % 10);
  if (resto == 0 ) {
    senha++;
  }
  if (senha < 36) {
    led_sete = random(1,2);  
  }
  else if (senha < 57) {
    led_sete = random(2,5);
  }
  else if (senha < 71) {
    led_sete = random(4,8);
  }
  else if (senha < 90) {
    led_sete = random(6,9);
  }
  else {
    led_sete = 9;
  }

  int resto_senha = (senha - (led_sete * 7));
  if (resto_senha < 11) {
    led_cinco = 1;  
  }
  else if (resto_senha < 16) {
    led_cinco = random(1,2);
  }
  else if (resto_senha < 21) {
    led_cinco = random(1,3);
  }
  else if (resto_senha < 31) {
    led_cinco = random(2,4);
  }
  else {
    led_cinco = random(3,6);
  }

  int resto_resto_senha = (senha - ((led_sete * 7) + (led_cinco *5)));
  if (resto_resto_senha > 15) {
    led_sete++;
  }  
  resto_resto_senha = (senha - ((led_sete * 7) + (led_cinco *5)));
  if (resto_resto_senha > 12){
    led_cinco = led_cinco + 2;
  }
  else if (resto_resto_senha > 8){
    led_cinco++;
  }
  resto_resto_senha = (senha - ((led_sete * 7) + (led_cinco *5)));
  led_um = resto_resto_senha;

  int verifica = (led_sete * 7) + (led_cinco *5) + led_um;
  if ( senha == verifica) {
    Serial.println("Ok");
  }
  else {
    Serial.println("Nao Ok");
  }
}
  
void correto() {
  lcd.setCursor(0,0);
  lcd.print("Bomba Desarmada!");
  tone(BUZZ, 500, 500);
}

void incorreto() {
  noInterrupts();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    Incorreto    ");
  lcd.setCursor(0,1);
  lcd.print("    Bummmm...    ");
  tone(BUZZ, 1000, 1000);
  porta.write(80);
  delay(150);
}

int le_fios() {
  int verdade = 0;
  fio_lr = digitalRead(FIO_LR);
  fio_vd = digitalRead(FIO_VD);
  fio_rx = digitalRead(FIO_RX);
  if ((fio_lr != 0) || (fio_vd != 0) || (fio_rx != 0)) {
    verdade = 1;
  }
  return verdade;
}

int le_teclado() {
  int tecla = 0;
  for (int ti = 1; ti<4; ti++) {  //Alterna o estado dos pinos das linhas
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    if (ti == 1){
      digitalWrite(A0, HIGH);
    }
    if (ti == 2){
      digitalWrite(A1, HIGH);
    }
    if (ti == 3){
      digitalWrite(A2, HIGH);
    }
    if (digitalRead(A3) == HIGH) {  //Verifica se alguma tecla da coluna 1 foi pressionada
      if (ti == 1){
        tecla = 1;
        aux = 1;
      }
      if (ti == 2){
        tecla = 4;
        aux = 1;
      }
      if (ti == 3){
        tecla = 7;
        aux = 1;
      }
      while(digitalRead(A3) == HIGH){}
    }
    if (digitalRead(A4) == HIGH) {  //Verifica se alguma tecla da coluna 2 foi pressionada   
      if (ti == 1){
        tecla = 2;
        aux = 1;
      }
      if (ti == 2){
        tecla = 5;
        aux = 1;
      }
      if (ti == 3){
        tecla = 8;
        aux = 1;
      }
      while(digitalRead(A4) == HIGH){};
    }
    if (digitalRead(A5) == HIGH) { //Verifica se alguma tecla da coluna 3 foi pressionada
      if (ti == 1){
        tecla = 3;
        aux = 1;
      }
      if (ti == 2){
        tecla = 6;
        aux = 1;
      }
      if (ti == 3){
        tecla = 9;
        aux = 1;
      }
      while(digitalRead(A5) == HIGH){}
    }
  }
  delay(10);
  return tecla;
}

void pisca_led(int led, int numero) {
  if (led == 1) {
    for (int i = 0; i < numero; i++){
      digitalWrite(LED_VM, HIGH);
      delay(1000);
      digitalWrite(LED_VM, LOW);
      delay(1000);
    } 
  }
  if (led == 2) {
    for (int i = 0; i < numero; i++){
      digitalWrite(LED_AM, HIGH);
      delay(1000);
      digitalWrite(LED_AM, LOW);
      delay(1000);
    } 
  }
  if (led == 3) {
    for (int i = 0; i < numero; i++){
      digitalWrite(LED_AZ, HIGH);
      delay(1000);
      digitalWrite(LED_AZ, LOW);
      delay(1000);
    } 
  }
}
