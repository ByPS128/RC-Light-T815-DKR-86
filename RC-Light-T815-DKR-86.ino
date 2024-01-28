#include <Arduino.h>
#include "MainApp.h"

MainApp mainApp;

void setup() {
  mainApp.init();
}

void loop() {
  mainApp.update();
}
