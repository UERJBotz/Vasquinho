#ifndef ServoIndicador_H
#define ServoIndicador_H
#include "placa.h"
#include <Arduino.h>
#include <ESP32Servo.h>

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
