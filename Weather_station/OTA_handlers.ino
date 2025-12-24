String UpdateServerUrl = "https://opensensormap.dev.techcolab.org/api/v1/displays/firmware/?version_check=";
String firmwareServerUrl = "https://opensensormap.dev.techcolab.org/api/v1/displays/firmware/?version_check=";

bool updateAvailableOTA() {
  if (WiFi.status() != WL_CONNECTED) {
    return false; // WiFi not connected
  }
  // Read firmware version from NVS
  prefs.begin("config", true);
  String current_version = prefs.getString("firmware_ver", "0.0.0");
  prefs.end();
  DEBUG_PRINTLN(current_version);
  // Build request payload
  String newUpdateServerUrl = UpdateServerUrl + current_version;
  DEBUG_PRINTLN(newUpdateServerUrl);
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.begin(client, newUpdateServerUrl);
  https.addHeader("Content-Type", "application/json");
  int httpResponseCode = -1;
  bool result = false;
  httpResponseCode = https.GET();
  DEBUG_PRINTLN("Attempt - HTTP response: " + String(httpResponseCode));
  String response = https.getString();
  DEBUG_PRINTLN(response);
  if (httpResponseCode == 200) {
    result = true;
  }
  else {
    DEBUG_PRINTLN("Error sending data. Response code: " + String(httpResponseCode));
    result = false;
  }
  delay(500);
  https.end();
  return result;
}


void performOTA() {
  if (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINTLN("WiFi not connected, OTA aborted");
    return;
  }
  //Read current firmware version from NVS
  prefs.begin("config", true);
  String current_version = prefs.getString("firmware_ver", "0.0.0");
  prefs.end();
  // Build request payload
  String newFirmwareServerUrl = firmwareServerUrl + current_version;
  DEBUG_PRINTLN(newFirmwareServerUrl);

  DEBUG_PRINTLN("Current firmware version: " + current_version);
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.setTimeout(60000);
  https.setReuse(false);
  DEBUG_PRINTLN("Starting OTA download...");
  if (!https.begin(client, newFirmwareServerUrl)) {
    DEBUG_PRINTLN("HTTPS begin failed");
    return;
  }

  //Send GET request
  int httpResponseCode = https.GET();
  DEBUG_PRINTLN("HTTP response code: " + String(httpResponseCode));
  String response = https.getString();
  DEBUG_PRINTLN(response);

  if (httpResponseCode != HTTP_CODE_OK) { //200
    DEBUG_PRINTLN("Firmware download failed");
    https.end();
    return;
  }

  //Get firmware size
  int contentLength = https.getSize();
  DEBUG_PRINTLN("Firmware size: " + String(contentLength));
  Update.onProgress([](size_t progress, size_t total) {
    if (total == 0) return;

    static int last_percent = -1;
    int percent = (progress * 100) / total;

    if (percent != last_percent) {
      DEBUG_PRINTLN("OTA Progress: " + String(percent) + "%");
      last_percent = percent;
    }
  });


  bool canBegin = false;

  if (contentLength > 0) {
    canBegin = Update.begin(contentLength);
  } else {
    DEBUG_PRINTLN("Unknown firmware size, using UPDATE_SIZE_UNKNOWN");
    canBegin = Update.begin(UPDATE_SIZE_UNKNOWN);
  }

  if (!canBegin) {
    DEBUG_PRINTLN("Not enough space for OTA");
    https.end();
    return;
  }

  // Stream firmware to flash
  WiFiClient *stream = https.getStreamPtr();
  size_t written = Update.writeStream(*stream);

  DEBUG_PRINTLN("Written bytes: " + String(written));

  if (contentLength > 0 && written != (size_t)contentLength) {
    DEBUG_PRINTLN("Incomplete firmware write");

    https.end();
    return;
  }

  //Finish OTA
  if (!Update.end()) {
    DEBUG_PRINTLN("OTA failed: " + String(Update.errorString()));
    https.end();
    return;
  }

  if (!Update.isFinished()) {
    DEBUG_PRINTLN("OTA not finished properly");
    https.end();
    return;
  }

  DEBUG_PRINTLN("OTA successful, rebooting...");
  https.end();
  delay(1000);
  ESP.restart();
}

void otaCheck() {
  if (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINTLN("WiFi not connected, OTA aborted");
    return;
  }

  bool update_needed = updateAvailableOTA();
  if (update_needed) {
    performOTA();
  }
}
