#include "WiFiUtils.hpp"

TStrings* WiFiUtils::scanWifi() {
  auto tft = this->tft;
  if (tft) tft->println("Scanning WiFi networks...");

  Serial.println("-- WiFi Scan started --");

  // Gather found ssids
  // TStrings ssids = {};
  TStrings* ssids = new TStrings();

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.printf("%d networks found\n", n);
    Serial.printf("Nr | %-32.32s | RSSI | CH | Encryption\n", "SSID");
    for (int i = 0; i < n; i++) {
      // Print SSID and RSSI for each network found
      const int n = i + 1;
      Serial.printf("%2d", n);
      Serial.print(" | ");
      const String ssid = WiFi.SSID(i);
      const char* ssidStr = ssid.c_str();
      if (tft) tft->printf("%2d %s\n", n, ssidStr);
      ssids->push_back(ssid);
      Serial.printf("%-32.32s", ssidStr);
      Serial.print(" | ");
      Serial.printf("%4ld", (long)WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2ld", (long)WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:
          Serial.print("open");
          break;
        case WIFI_AUTH_WEP:
          Serial.print("WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          Serial.print("WPA");
          break;
        case WIFI_AUTH_WPA2_PSK:
          Serial.print("WPA2");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          Serial.print("WPA+WPA2");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          Serial.print("WPA2-EAP");
          break;
        case WIFI_AUTH_WPA3_PSK:
          Serial.print("WPA3");
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          Serial.print("WPA2+WPA3");
          break;
        case WIFI_AUTH_WAPI_PSK:
          Serial.print("WAPI");
          break;
        default:
          Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
  }

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  Serial.println("-- WiFi Scan done --");

  return ssids;
}
