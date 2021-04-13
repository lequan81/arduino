#include "blinkLed.h"

void blinkLed(int led_State, unsigned long lastTime, unsigned long interval) {
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    if (led_State == LOW) {
      led_State = HIGH; // OFF
      lastTime = currentTime;
      interval = 3350;
    } else if (led_State == HIGH) {
      led_State = LOW;  // ON
      lastTime = currentTime;
      interval = 150;
    }
    digitalWrite(BUILTIN_LED, led_State);
  }
}
