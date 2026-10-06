#include "WiFiUtils.hpp"

bool WiFiUtils::startAccessPoint(const String& ssid, const String& pwd,
                                 TFT_eSPI* tft) {
  if (tft) tft->println("Starting WiFi access point...");

  Serial.println("-- WiFi AP initialization started --");

  if (!WiFi.softAP(ssid, pwd)) {
    Serial.println("Error: Failed to start Access Point!");
    return false;
    // Handle error here (e.g., retry, blink LED, or restart)
  }

  // Serial.println("Access Point launched successfully");
  const IPAddress accessPointIP = WiFi.softAPIP();
  Serial.printf("WiFi AP IP Address: %s\n", accessPointIP.toString().c_str());
  Serial.printf("WiFi AP SSID: %s\n", ssid);
  Serial.printf("WiFi AP Password: %s\n", pwd);

  Serial.println("-- WiFi AP initialization finished --");

  return true;
}
