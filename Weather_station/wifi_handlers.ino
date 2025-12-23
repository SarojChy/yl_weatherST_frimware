void setupWiFi() {
  wifiManagerRunning = true;

  wm.setConnectTimeout(10);
  wm.setConfigPortalTimeout(120);

  // ===== Add instruction message at the top of the portal =====
  WiFiManagerParameter info(
    "<p style='color:blue; font-weight:bold;'>"
    "For any inquiry: <a href='https://youthinnovationlab.org/' target='_blank'>https://youthinnovationlab.org/</a>"
    "</p>"
  );

  // ===== Custom fields =====
  param_device_id  = new WiFiManagerParameter("device_id", "Device ID", device_id, 32);
  param_delay_time = new WiFiManagerParameter("delay", "Delay Time (1000-30000 ms)", delay_time_str, 16);

  wm.addParameter(param_device_id);
  wm.addParameter(param_delay_time);
  wm.addParameter(&info);

  DEBUG_PRINTLN("Starting WiFiManager...");

  // 🔥 Use autoConnect with defined SSID & password
  if (!wm.autoConnect("openmapsensor", "openmap1")) {
    DEBUG_PRINTLN("WiFiManager failed or timed out");
    wifiConnected = false;
  }

  wifiManagerRunning = false;

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;

    strncpy(device_id, param_device_id->getValue(), sizeof(device_id));
    strncpy(delay_time_str, param_delay_time->getValue(), sizeof(delay_time_str));

    saveConfig();
    DEBUG_PRINTLN("WiFi & config saved");
  } else {
    wifiConnected = false;
  }
}

void wifiMonitorTask(void *pvParameters) {
  const TickType_t checkInterval = 10000 / portTICK_PERIOD_MS;

  while (true) {

    if (wifiManagerRunning) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }
    if (WiFi.status() != WL_CONNECTED) {
      wifiConnected = false;
      DEBUG_PRINTLN("WiFi lost, trying reconnect...");

      unsigned long start = millis();
      while (millis() - start < 60000) {
        WiFi.reconnect();
        if (WiFi.status() == WL_CONNECTED) {
          wifiConnected = true;
          DEBUG_PRINTLN("WiFi reconnected");
          break;
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
      }

      // 🔴 If reconnect fails → RESTART (safe & clean)
      if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("Reconnect failed → reboot to portal");
        ESP.restart();
      }
    }

    vTaskDelay(checkInterval);
  }
}
