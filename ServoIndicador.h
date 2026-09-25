#ifndef ServoIndicador_H
#define ServoIndicador_H

#include <Arduino.h>
#include <ESP32Servo.h>

// ── Pino do servo ────────────────────────────────────────────
// Ajuste aqui se o fio de sinal do servo estiver em outro pino.
// GPIO13 foi escolhido por estar livre nesse projeto (motor usa
// 4/18/19/23, sensores usam 14/33/34/35, IR usa 15, boot/LED usam 0/2).
// ─────────────────────────────────────────────────────────────

#define SERVO_CENTRO   90   // repouso / neutro
#define SERVO_ESQUERDA 0    // "-90" relativo ao centro
#define SERVO_DIREITA  180  // "+90" relativo ao centro

Servo servoIndicador;

// Chame uma vez no setup()
void setupServo() {
  servoIndicador.setPeriodHertz(50);            // servo padrão (analógico) usa 50Hz
  servoIndicador.attach(SERVO_PIN, 500, 2400);  // largura de pulso min/max em µs
  servoIndicador.write(SERVO_CENTRO);           // começa centralizado
}

// graus: -90, 0 ou 90 (relativo ao centro) — ver convenção acima
void servoMoverRelativo(int grausRelativoAoCentro) {
  int angulo = constrain(SERVO_CENTRO + grausRelativoAoCentro, 0, 180);
  servoIndicador.write(angulo);
}

// angulo: 0 a 180, direto na escala real do servo
void servoMoverAbsoluto(int angulo) {
  servoIndicador.write(constrain(angulo, 0, 180));
}

#endif // ServoIndicador_H
