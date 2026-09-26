#ifndef ServoDirecao_H
#define ServoDirecao_H

// ============================================================
//  Controle do servo (pino S5 = GPIO33) pelas setas ESQUERDA/DIREITA
//  do MESMO controle remoto usado pra escolher a estratégia.
//
//  Biblioteca: "ESP32Servo" (madhephaestus / Kevin Harrington)
//    Instalar em: Sketch > Incluir Biblioteca > Gerenciar Bibliotecas
//                 > busque "ESP32Servo"
//    Repo: https://github.com/madhephaestus/ESP32Servo
//    É a biblioteca de servo recomendada pro ESP32 atualmente: mantida
//    ativamente e compatível com o core arduino-esp32 v3+ (o mesmo core
//    que o SumoDrive.h já usa, com ledcAttach()). A alternativa antiga
//    (jkb-git/ESP32Servo) está marcada OBSOLETA pelo próprio autor e
//    não funciona direito nesse core novo — não use essa.
//    O GPIO33 (nosso S5) está inclusive na lista de pinos recomendados
//    pela própria biblioteca.
//
//  Orçamento de canais PWM (LEDC) no ESP32 WROOM: o motor (SumoDrive.h)
//  já usa 4 canais; o ESP32Servo pega mais 1 pra esse servo = 5 de 16
//  disponíveis no chip. Bem longe do limite, sem risco de esgotar canal
//  (foi exatamente esse tipo de esgotamento que causou o problema do LED
//  antes — aqui não deve se repetir).
// ============================================================

#include <ESP32Servo.h>
#include <IRrecv.h>     // decode_results (mesmo tipo usado por ModuloStart/SeletorEstrategia)
#include "placa.h"

// ─── Configure aqui os códigos das SETAS do seu controle ─────
// Pegue os valores reais no Serial Monitor (linha "[IR] 0x..."):
// ligue o robô, aponte o controle e aperte cada seta — mesmo processo
// que vocês já usaram pra descobrir os códigos das estratégias (0-6).
#define IR_CMD_SERVO_ESQ   0x2D0   // <-- TROQUE pelo código real da seta ESQUERDA
#define IR_CMD_SERVO_DIR   0xCD0   // <-- TROQUE pelo código real da seta DIREITA
// ────────────────────────────────────────────────────────────

#define SERVO_PASSO_GRAUS   90   // quantos graus cada aperto de seta move o servo — calibre aqui
#define SERVO_ANGULO_MIN   -90
#define SERVO_ANGULO_MAX    90

Servo servoDirecao;
int   anguloServo = 0; // ângulo RELATIVO: 0 = posição inicial ("pra cima"), varia de -90 a +90

// Converte o ângulo relativo (-90..+90, 0 = centro/"pra cima") pro ângulo
// físico que Servo.write() espera (0..180). Se na sua montagem física
// 90° não for exatamente "pra cima", ajuste esse offset (o "90 +").
void _aplicarAnguloServo() {
  servoDirecao.write(90 + anguloServo);
}

// Chame UMA VEZ no setup(), depois de Serial.begin()
void setupServo() {
  int canal = servoDirecao.attach(S5);
  anguloServo = 0;
  _aplicarAnguloServo(); // garante a posição inicial "pra cima" assim que liga

  Serial.print("[Servo] attach no pino ");
  Serial.print(S5);
  Serial.print(" -> ");
  Serial.println(canal != 0 ? "OK" : "FALHOU (confira pino/fiação/alimentacao do servo)");
}

// ------------------------------------------------------------
// Chame SÓ durante moduloStart.desligado() (mesma fase em que a
// estratégia é escolhida), passando o MESMO decode_results* que
// já foi lido de moduloStart.ultimoResultado() nesse ciclo do loop().
//
// IMPORTANTE: NÃO chame moduloStart.ultimoResultado() de novo aqui.
// Esse método só entrega um resultado novo UMA vez; chamando de novo,
// a segunda leitura vem vazia e o servo (ou a estratégia, dependendo
// da ordem) simplesmente para de responder ao controle sem avisar.
// Capture o resultado uma vez no loop() e passe o mesmo ponteiro
// pros dois: seletorEstrategia.atualizar(resultado) e atualizarServo(resultado).
// ------------------------------------------------------------
void atualizarServo(decode_results* resultado) {
  if (!resultado) return;

  uint64_t codigo = resultado->value;
  if (codigo == 0xFFFFFFFFFFFFFFFFULL || codigo == 0) return; // repeat/inválido, ignora

  if (codigo == IR_CMD_SERVO_ESQ) {
    anguloServo = constrain(anguloServo - SERVO_PASSO_GRAUS, SERVO_ANGULO_MIN, SERVO_ANGULO_MAX);
    _aplicarAnguloServo();
    Serial.print("[Servo] <- angulo: "); Serial.println(anguloServo);
  }
  else if (codigo == IR_CMD_SERVO_DIR) {
    anguloServo = constrain(anguloServo + SERVO_PASSO_GRAUS, SERVO_ANGULO_MIN, SERVO_ANGULO_MAX);
    _aplicarAnguloServo();
    Serial.print("[Servo] -> angulo: "); Serial.println(anguloServo);
  }
}

#endif
