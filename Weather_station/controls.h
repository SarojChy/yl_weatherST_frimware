#define DEBUG 1  // Set to 0 to disable all debug prints

#if DEBUG
#define DEBUG_PRINT(x)    Serial.print(x)
#define DEBUG_PRINTLN(x)  Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

bool useGSM = true;  
bool useWiFi = false;  
bool useBoth =false;

volatile bool wifiManagerRunning = false;
bool wifiConnected = false;
