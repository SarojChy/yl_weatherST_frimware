void ledTask(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);

  while (true) {
    if (wifiConnected) {
      digitalWrite(LED_PIN, HIGH);   // Solid ON
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else {
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(250 / portTICK_PERIOD_MS);
      digitalWrite(LED_PIN, LOW);
      vTaskDelay(250 / portTICK_PERIOD_MS);
    }
  }
}
