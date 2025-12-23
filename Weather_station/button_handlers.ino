void buttonTask(void *pvParameters) {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  unsigned long pressStart = 0;
  bool pressed = false;

  while (true) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      if (!pressed) {
        pressStart = millis();
        pressed = true;
      } else if (millis() - pressStart >= 5000) {
        DEBUG_PRINTLN("Resetting WiFi & Device Config");
        prefs.begin("config", false);
        prefs.clear();
        prefs.end();
        wm.resetSettings();  
        delay(1000);
        ESP.restart();
      }
    } else {
      pressed = false;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
