#include "WiFiUtils.hpp"

bool WiFiUtils::startAccessPoint(const String& ssid, const String& pwd) {
  if (tftUtils) tftUtils->writeLine("Starting WiFi AP...");

  if (!WiFi.softAP(ssid, pwd)) {
    if (tftUtils) tftUtils->writeLine("Failed to start Access Point!");
    return false;
    // Handle error here (e.g., retry, blink LED, or restart)
  }

  // Serial.println("Access Point launched successfully");
  auto accessPointIP = WiFi.softAPIP().toString();

  if (tftUtils) tftUtils->writeLine("AP IP: " + accessPointIP);
  if (tftUtils) tftUtils->writeLine("AP SSID: " + ssid);
  if (tftUtils) tftUtils->writeLine("AP PWD: " + pwd);

  return true;
}
