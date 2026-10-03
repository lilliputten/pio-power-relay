#include "WiFiUtils.hpp"

void WiFiUtils::initWiFi() {
  // Set WiFi to station mode and disconnect from an AP if it was previously
  // connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}
