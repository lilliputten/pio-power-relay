#ifndef WiFiUtils_DEFINED
#define WiFiUtils_DEFINED

#include <WiFi.h>

#include "DataFiles.hpp"
#include "TFTUtils.hpp"

class WiFiUtils {
 private:
  TFT_eSPI* tft = nullptr;

 public:
  void initWiFi();
  TStrings* scanWifi();
  T2DStrings* loadWiFiConfigs();
  bool connectWiFi(const T2DStrings* configs, const TStrings* ssids);
  bool startAccessPoint(const String& ssid, const String& pwd);

  WiFiUtils(TFTUtils* tftUtils = nullptr) {
    // Constructor
    if (tftUtils) {
      this->tft = &tftUtils->getTFT();
    }
  }
};

#endif
