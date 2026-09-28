#ifndef ServoDirecao_H
#define ServoDirecao_H



#include <ESP32Servo.h>
#include <IRrecv.h>     // decode_results (mesmo tipo usado por ModuloStart/SeletorEstrategia)
#include "placa.h"


#define IR_CMD_SERVO_ESQ   0xCD0   // <-- TROQUE pelo código real da seta ESQUERDA
#define IR_CMD_SERVO_DIR   0x2D0   // <-- TROQUE pelo código real da seta DIREITA


#define SERVO_ANGULO_CIMA   0    // posição inicial/neutra ("pra cima")
#define SERVO_ANGULO_ESQ  -90
#define SERVO_ANGULO_DIR   90

Servo servoDirecao;
int   anguloServo        = SERVO_ANGULO_CIMA; // ângulo REALMENTE aplicado no servo agora
int   direcaoSelecionada = SERVO_ANGULO_CIMA; // o que foi escolhido no controle, ainda não aplicado

// Converte o ângulo relativo (-90..0..+90) pro ângulo físico que
// Servo.write() espera (0..180). Se na sua montagem física 90° não
// for exatamente "pra cima", ajuste esse offset (o "90 +").
void _aplicarAnguloServo() {
  servoDirecao.write(90 + anguloServo);
}

// Chame UMA VEZ no setup(), depois de Serial.begin() e DEPOIS de motor.begin()
void setupServo() {
  // Trava o ESP32Servo só no timer 3 (canais 6/7), longe dos timers que o
  // motor já ocupou. allocateTimer() com um único número deixa só esse timer
  // liberado. (Na prática o core dá canais 0-3 pro motor = timers 0 e 1.)
  ESP32PWM::allocateTimer(3);
  servoDirecao.setPeriodHertz(50);                     // servo padrão: 50 Hz
  int canal = servoDirecao.attach(S5, 500, 2400);      // pulso min/max em µs (SG90 e similares)

  anguloServo        = SERVO_ANGULO_CIMA;
  direcaoSelecionada = SERVO_ANGULO_CIMA;
  _aplicarAnguloServo(); // garante a posição inicial "pra cima" assim que liga

  Serial.print("[Servo] attach no pino ");
  Serial.print(S5);
  Serial.print(" -> ");
  Serial.print(canal != 0 ? "OK" : "FALHOU (confira pino/fiação/alimentacao do servo)");
  Serial.print(" | canal LEDC: ");
  Serial.println(canal); // esperado: 6 ou 7. Se aparecer 0-3, está disputando canal com o motor
}


void selecionarDirecaoServo(decode_results* resultado) {
  if (!resultado) return;

  uint64_t codigo = resultado->value;
  if (codigo == 0xFFFFFFFFFFFFFFFFULL || codigo == 0) return; // repeat/inválido, ignora

  if (codigo == IR_CMD_SERVO_ESQ) {
    direcaoSelecionada = SERVO_ANGULO_ESQ;
    Serial.println("[Servo] direcao selecionada: ESQUERDA (só aplica quando o juiz mandar iniciar)");
  }
  else if (codigo == IR_CMD_SERVO_DIR) {
    direcaoSelecionada = SERVO_ANGULO_DIR;
    Serial.println("[Servo] direcao selecionada: DIREITA (só aplica quando o juiz mandar iniciar)");
  }
}


void aplicarDirecaoServoSelecionada() {
  anguloServo = direcaoSelecionada;
  _aplicarAnguloServo();
  Serial.print("[Servo] INICIAR recebido -> movendo direto para: ");
  Serial.println(anguloServo);
}

#endif
