#ifndef placa_H
#define placa_H
/* ! os sensores do meio na frente tão ligados com esses "pinos" (não usar) ! */
#define S0 VP
#define S1 VIN
/* ! ^^^^ ! */

#define S2 34  //lateral direita
#define S3 35  //frontal direita
#define S4 32
#define S5 33
#define S6 25  //servo
#define S7 27  //frontal esquerda
#define S8 14  //lateral esquerda

#define IR_PIN 15
#define LED_STRIP 2

#define D26 26
#define BTN2 12

#define MA1 19
#define MA2 18
#define MB1 4
#define MB2 23

#define SEsq  S8
#define SFEsq S7
#define SFDir S3
#define SDir  S2

void setupPortas() {
  pinMode(S8, INPUT);
  pinMode(S7, INPUT);
  pinMode(S3, INPUT);
  pinMode(S2, INPUT);

  pinMode(SERVO_PIN, OUTPUT);
  pinMode(LED_STRIP, OUTPUT);
}

#endif