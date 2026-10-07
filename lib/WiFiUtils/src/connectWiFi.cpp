#include <algorithm>  // Required for std::find
#include <vector>

#include "WiFiUtils.hpp"

static const unsigned long wifiTimeout = 10000;  // WiFi connection timeout

static bool _findNetwork(const TStrings* ssids, const String& ssid) {
  // std::find searches sequentially from start to finish
  auto it = std::find(ssids->begin(), ssids->end(), ssid);

  if (it != ssids->end()) {
    // Found it!
    int index = std::distance(ssids->begin(), it);
    // Serial.printf("Found match '%s' at index %d\n", it->c_str(), index);
    return true;
  }

  // Serial.printf("Network %s not found\n", ssid);

  return false;
}

static bool _tryToConnect(const String& ssid, const String& pwd,
                          TFT_eSPI* tft) {
  unsigned long startAttemptTime = millis();

  Serial.printf("Started connection to network %s", ssid);

  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    // CONNECTION FAILED - Diagnose the error
    Serial.println("\nError: Connection failed!");
    Serial.print("Final status code: ");
    Serial.println(WiFi.status());
    return false;
  }

  Serial.println("\nConnected successfully!");
  auto localIP = WiFi.localIP().toString();
  Serial.printf("WiFi Network IP address: %s\n", localIP);
  if (tft) tft->printf("SSID: %s\n", ssid);
  tft->printf("IP: %s\n", localIP);

  return true;
}

bool WiFiUtils::connectWiFi(const T2DStrings* configs, const TStrings* ssids) {
  auto tft = this->tft;
  if (tft) tft->println("Connecting to WiFi...");

  Serial.println("-- WiFi Connection started --");
  if (!configs) {
    Serial.println("-- WiFi Connection failed: no configs passed --");
    return false;
  }
  if (!ssids) {
    Serial.println("-- WiFi Connection failed: no ssids passed --");
    return false;
  }

  const int configsCount = configs->size();

  bool connectedToNetwork = false;

  try {
    for (int i = 0; i < configsCount; i++) {
      const TStrings item = (*configs)[i];
      const String ssid = item[0];
      const String pwd = item[1];
      const bool isFound = _findNetwork(ssids, ssid);
      // Serial.printf("%s %s: %s\n", isFound ? "Found" : "Not found", ssid,
      // pwd);
      if (isFound) {
        // Try to connect to the network
        if (_tryToConnect(ssid, pwd, tft)) {
          connectedToNetwork = true;
          break;
        }
      }
    }

  } catch (const std::exception& e) {
    Serial.printf("connectWiFi: Exception: %s\n", e.what());
  } catch (...) {
    Serial.println("connectWiFi: Unknown exception");
  }

  Serial.println("-- WiFi Connection finished --");

  return connectedToNetwork;
}
