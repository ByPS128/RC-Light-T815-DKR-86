#include <Arduino.h>
#include <EEPROM.h>
#include "MainApp.h"

MainApp mainApp;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.print(F("EEPROM size: "));
  Serial.println(EEPROM.length());

  Serial.println(F("Booting"));
  mainApp.init();
}

void loop() {
  Serial.print(F("Free memory: "));
  Serial.println(freeMemory());

  mainApp.update();
}

// Funkce pro zjištění volné paměti
int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
