int sendDataToServer(String httpRequestData) {
  if (WiFi.status() != WL_CONNECTED) {
    return -1; // WiFi not connected
  }
  String serverUrl = "https://webhook.site/be5dd016-6a4e-4560-a71a-9b944f66173f";
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.begin(client, serverUrl);
  https.addHeader("Content-Type", "application/json");

  int httpResponseCode = -1;

  for (int attempt = 1; attempt <= 3; attempt++) {
    httpResponseCode = https.POST(httpRequestData);
    DEBUG_PRINTLN("Attempt " + String(attempt) + " - Http POST response: " + String(httpResponseCode));
    String response = https.getString();
    DEBUG_PRINTLN(response);

    if (httpResponseCode == 200) {
      break;
    } else {
      DEBUG_PRINTLN("Error sending data. Response code: " + String(httpResponseCode));
      delay(500);
    }
  }
  https.end(); 
  return httpResponseCode;
}
