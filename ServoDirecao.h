#ifndef ServoDirecao_H
#define ServoDirecao_H

// ============================================================
//  Controle do servo (pino S5 = GPIO33) pelas setas ESQUERDA/DIREITA
//  do MESMO controle remoto usado pra escolher a estratégia.
//
//  COMPORTAMENTO (igual ao da seleção de estratégia):
//    - Durante moduloStart.desligado() (fase de escolha), apertar
//      ESQUERDA ou DIREITA só REGISTRA qual direção foi escolhida —
//      é uma seleção direta (seta ESQ = -90°, seta DIR = +90°), não
//      um incremento gradual. O servo NÃO se move nesse momento.
//    - O servo só se move de fato — num movimento direto pra posição
//      escolhida — no instante em que o "2" (INICIAR) do start do
//      juiz é recebido (o mesmo instante em que o combate começa).
//    - Se nenhuma direção for escolhida antes do INICIAR, o servo
//      permanece "pra cima" (posição inicial).
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
//  ATENÇÃO — PWM COMPARTILHADO COM O MOTOR (LEDC):
//  O motor (SumoDrive.h) usa ledcAttach() direto do core do ESP32; o
//  ESP32Servo tem uma contabilidade PRÓPRIA de canais/timers, que começa
//  no timer 0 e NÃO enxerga os canais do motor. Como cada timer do LEDC
//  é compartilhado por um PAR de canais (0/1, 2/3, 4/5...), se o servo
//  cair num timer do motor ele pode mudar a frequência/resolução do PWM
//  do motor. Por isso o setupServo() trava o ESP32Servo só no timer 3
//  (canais 6/7) com ESP32PWM::allocateTimer(3), e o .ino imprime a
//  frequência do PWM do motor antes/depois do servo pra conferir.
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

// ------------------------------------------------------------
// Chame SÓ durante moduloStart.desligado() (mesma fase em que a
// estratégia é escolhida), passando o MESMO decode_results* que já
// foi lido de moduloStart.ultimoResultado() nesse ciclo do loop().
// Só REGISTRA a direção — não move o servo ainda.
//
// IMPORTANTE: NÃO chame moduloStart.ultimoResultado() de novo aqui.
// Esse método só entrega um resultado novo UMA vez; chamando de novo,
// a segunda leitura vem vazia e o servo (ou a estratégia, dependendo
// da ordem) simplesmente para de responder ao controle sem avisar.
// Capture o resultado uma vez no loop() e passe o mesmo ponteiro pros
// dois: seletorEstrategia.atualizar(resultado) e selecionarDirecaoServo(resultado).
// ------------------------------------------------------------
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

// ------------------------------------------------------------
// Chame UMA VEZ, exatamente no instante em que o combate começa
// (o mesmo instante do "2"/INICIAR do juiz — no .ino isso é a borda
// emCombateAgora && !emCombateAnterior, junto do resetSeekAndDestroy()).
// Move o servo de vez, direto, pra direção que foi selecionada.
// ------------------------------------------------------------
void aplicarDirecaoServoSelecionada() {
  anguloServo = direcaoSelecionada;
  _aplicarAnguloServo();
  Serial.print("[Servo] INICIAR recebido -> movendo direto para: ");
  Serial.println(anguloServo);
}

#endif
