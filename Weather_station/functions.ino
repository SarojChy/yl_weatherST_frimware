void displayWifiCredentials() {
  DEBUG_PRINTLN("===============");
  DEBUG_PRINT("Device ID  : "); DEBUG_PRINTLN(device_id);
  DEBUG_PRINT("Delay Time : "); DEBUG_PRINT(delay_time_str); DEBUG_PRINTLN(" ms");
  DEBUG_PRINT("SSID       : "); DEBUG_PRINTLN(WiFi.SSID());
  DEBUG_PRINT("IP Address : "); DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINTLN("===============");
}
void saveConfig() {
  int validatedDelay = validateDelay(delay_time_str);
  snprintf(delay_time_str, sizeof(delay_time_str), "%d", validatedDelay);
  prefs.begin("config", false);
  prefs.putString("device_id", device_id);
  prefs.putString("delay", delay_time_str);
  prefs.end();
}

void loadConfig() {
  prefs.begin("config", true);   // read-only
  prefs.getString("device_id", device_id, sizeof(device_id));
  prefs.getString("delay", delay_time_str, sizeof(delay_time_str));
  prefs.end();
}
int validateDelay(const char *value) {
  int d = atoi(value);   // convert string → int
  if (d < 1000 || d > 30000) {
    DEBUG_PRINTLN("Invalid delay, using default 1000 ms");
    return 1000;
  }
  return d;
}
