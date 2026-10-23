#ifndef WiFiUtils_DEFINED
#define WiFiUtils_DEFINED

#include <WiFi.h>

#include "DataFiles.hpp"
#include "TFTUtils.hpp"

class WiFiUtils {
 private:
  TFTUtils* tftUtils = nullptr;

  String localSSID = "";
  String localIPAddr = "";
  String accessPointIPAddr = "";

 public:
  const String getWiFiClientSSID() const {
    // return WiFi.SSID();
    return this->localSSID;
  }
  const String getWiFiClientIP() const {
    // const auto ipAddr = WiFi.localIP();
    // return ipAddr.toString();
    return this->localIPAddr;
  }
  const String getWiFiAPSSID() const { return WIFI_AP_SSID; }
  const String getWiFiAPPwd() const { return WIFI_AP_PWD; }
  const String getWiFiAPIP() const {
    // return WiFi.softAPSSID();
    return this->accessPointIPAddr;
  }

  void initWiFi();
  TStrings scanWifi();
  T2DStrings loadWiFiConfigs();
  bool connectWiFi(const T2DStrings& configs, const TStrings& ssids);
  bool startAccessPoint(const String& ssid, const String& pwd);

  WiFiUtils(TFTUtils* tftUtils = nullptr) {
    // Constructor
    if (tftUtils) {
      this->tftUtils = tftUtils;
      // this->tft = &tftUtils->getTFT();
    }
  }
};

#endif
