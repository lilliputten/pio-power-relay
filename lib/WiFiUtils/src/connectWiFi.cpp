#include <algorithm>  // Required for std::find
#include <vector>

#include "WiFiUtils.hpp"

static const unsigned long wifiTimeout = 10000;  // WiFi connection timeout

static bool __findNetwork(const TStrings& ssids, const String& ssid) {
  // std::find searches sequentially from start to finish
  auto it = std::find(ssids.begin(), ssids.end(), ssid);

  if (it != ssids.end()) {
    // Found it!
    int index = std::distance(ssids.begin(), it);
    // Serial.printf("Found match '%s' at index %d\n", it->c_str(), index);
    return true;
  }

  // Serial.printf("Network %s not found\n", ssid);

  return false;
}

static bool __tryToConnect(const String& ssid, const String& pwd,
                           TFTUtils* tftUtils) {
  unsigned long startAttemptTime = millis();

  const String str = "Trying " + ssid + "...";
  if (tftUtils) tftUtils->tft.print(str);
  Serial.print(str);

  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    if (tftUtils) tftUtils->tft.print(".");
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    // CONNECTION FAILED - Diagnose the error
    Serial.println("\nError: Connection failed!");
    Serial.print("Final status code: ");
    Serial.println(WiFi.status());
    WiFi.disconnect();
    return false;
  }

  return true;
}

bool WiFiUtils::connectWiFi(const T2DStrings& configs, const TStrings& ssids) {
  if (tftUtils) tftUtils->writeLine("Connecting to WiFi...");

  if (!configs.size()) {
    if (tftUtils) tftUtils->writeLine("No configs");
    return false;
  }
  if (!ssids.size()) {
    if (tftUtils) tftUtils->writeLine("No SSIDs");
    return false;
  }

  const int configsCount = configs.size();

  bool connectedToNetwork = false;

  try {
    for (int i = 0; i < configsCount; i++) {
      const TStrings item = configs[i];
      const String ssid = item[0];
      const String pwd = item[1];
      const bool isFound = __findNetwork(ssids, ssid);
      // Serial.printf("%s %s: %s\n", isFound ? "+" : "-", ssid, pwd);
      if (isFound) {
        // Try to connect to the network
        if (__tryToConnect(ssid, pwd, tftUtils)) {
          connectedToNetwork = true;
          Serial.printf("\nConnected to %s\n", ssid);
          this->localSSID = ssid;
          const auto localIP = WiFi.localIP();
          this->localIPAddr = localIP.toString();
          // Serial.printf("IP Address: %s\n", localIP);
          // if (tftUtils) tftUtils->writeLine("SSID: " + ssid);
          if (tftUtils) tftUtils->writeLine("OK IP: " + this->localIPAddr);
          // delay(2000);
          break;
        }
      }
    }

  } catch (const std::exception& e) {
    Serial.printf("connectWiFi: Exception: %s\n", e.what());
  } catch (...) {
    Serial.println("connectWiFi: Unknown exception");
  }

  // if (connectedToNetwork) {
  //   delay(2000);
  // }

  return connectedToNetwork;
}
