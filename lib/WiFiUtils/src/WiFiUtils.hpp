#ifndef WiFiUtils_DEFINED
#define WiFiUtils_DEFINED

#include <WiFi.h>

#include "DataFiles.hpp"
#include "TFTUtils.hpp"

class WiFiUtils {
 public:
  void initWiFi(TFT_eSPI* tft = nullptr);
  TStrings* scanWifi(TFT_eSPI* tft = nullptr);
  T2DStrings* loadWiFiConfigs(TFT_eSPI* tft = nullptr);
  bool connectWiFi(const T2DStrings* configs, const TStrings* ssids,
                   TFT_eSPI* tft = nullptr);
  bool startAccessPoint(const String& ssid, const String& pwd,
                        TFT_eSPI* tft = nullptr);
};

#endif
