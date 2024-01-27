#include <Arduino.h>
#include "PwmLedBrightnessControl.h"
#include "LedController.h"

LedSetup ledSetup;
LedController ledController;

void setup() {
  Serial.begin(9600);
  ledController.init();
  ledSetup.init(&ledController, 3, 5, 10);
}

void loop() {
  ledSetup.update();

  delay(10); // Malá pauza pro snížení zatížení procesoru
}
