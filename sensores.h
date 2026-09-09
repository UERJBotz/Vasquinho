#ifndef Sensores_H
#define Sensores_H

//ultra t
// ultima atualização 16/11/2025
#define JEsq 35
#define JFEsq 33
#define JFDir 14
#define JDir 34

// caçadora
// #define JEsq 33
// #define JFEsq 27
// #define JFDir 26
// #define JDir 32

void setupSensores() {
  pinMode(JEsq, INPUT);
  pinMode(JFEsq, INPUT);
  pinMode(JFDir, INPUT);
  pinMode(JDir, INPUT);
}

#endif
