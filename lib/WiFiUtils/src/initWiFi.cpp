#include "WiFiUtils.hpp"

void WiFiUtils::initWiFi() {
  auto tft = this->tft;
  if (tft) tft->println("Initializing WiFi...");

  Serial.println("-- WiFi Initialization started --");

  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();

  const T2DStrings* configs = this->loadWiFiConfigs();
  const TStrings* ssids = this->scanWifi();

  this->connectWiFi(configs, ssids);

#ifdef WIFI_AP_SSID
#ifndef WIFI_AP_PWD
#error "WIFI_AP_PWD is undefined (required for WIFI_AP_SSID)"
#else
  this->startAccessPoint(WIFI_AP_SSID, WIFI_AP_PWD);
#endif
#endif

  // const IPAddress localIP =  WiFi.localIP();
  // const IPAddress accessPointIP =  WiFi.softAPIP();

  // delay(100);

  Serial.println("-- WiFi Initialization finished --");
}
