#include "WiFiUtils.hpp"

bool WiFiUtils::startAccessPoint(const String& ssid, const String& pwd) {
  auto tft = this->tft;
  if (tft) tft->println("Starting WiFi AP...");

  Serial.println("-- WiFi AP initialization started --");

  if (!WiFi.softAP(ssid, pwd)) {
    Serial.println("Error: Failed to start Access Point!");
    return false;
    // Handle error here (e.g., retry, blink LED, or restart)
  }

  // Serial.println("Access Point launched successfully");
  auto accessPointIP = WiFi.softAPIP().toString();

  const char* addrTemplate = "AP IP: %s\n";
  const char* ssidTemplate = "AP SSID: %s\n";
  const char* pwdTemplate = "AP PWD: %s\n";

  Serial.printf(addrTemplate, accessPointIP);
  Serial.printf(ssidTemplate, ssid);
  Serial.printf(pwdTemplate, pwd);

  if (tft) {
    tft->printf(addrTemplate, accessPointIP);
    tft->printf(ssidTemplate, ssid);
    tft->printf(pwdTemplate, pwd);
  }

  Serial.println("-- WiFi AP initialization finished --");

  return true;
}
