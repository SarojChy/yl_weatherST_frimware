
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "pin_config.h"
#include "controls.h"
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

WiFiManager wm;
/* Custom parameters */
char device_id[32] = "";
char delay_time_str[16] = "1000";  // default in ms
WiFiManagerParameter *param_device_id;
WiFiManagerParameter *param_delay_time;
Preferences prefs;

void setup() {
  Serial.begin(115200);
  delay(500);
  loadConfig();
  setupWiFi();

  xTaskCreatePinnedToCore(ledTask, "ledTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(buttonTask, "buttonTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(wifiMonitorTask, "wifiMonitorTask", 4096, NULL, 1, NULL, 0);

  if (wifiConnected) {
    DEBUG_PRINT("Device ID  : "); DEBUG_PRINTLN(device_id);
    DEBUG_PRINT("Delay Time : "); DEBUG_PRINT(delay_time_str); DEBUG_PRINTLN(" ms");
  }
}

void loop() {
  if (wifiConnected) {
    int delay_time = atoi(delay_time_str);
    displayWifiCredentials();
    sendDataToServer(buildJSON());
    delay(delay_time);
  }
}
