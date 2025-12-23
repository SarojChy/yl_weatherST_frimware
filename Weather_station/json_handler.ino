
String buildJSON() {
  JsonDocument doc;
  doc["device_id"]   = device_id;
  doc["status"]      = delay_time_str;
  doc["timestamp"]   = millis();
  String jsonOutput;
  if (serializeJson(doc, jsonOutput) == 0) {
    DEBUG_PRINTLN("Failed to write to string");
    return "";
  }
  DEBUG_PRINTLN(jsonOutput);
  return jsonOutput;
}
