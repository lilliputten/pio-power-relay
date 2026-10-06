#include "WiFiUtils.hpp"

// WiFi config row items (SSID, Password)
const int expectedItemsCount = 2;

T2DStrings* WiFiUtils::loadWiFiConfigs(TFT_eSPI* tft) {
#ifndef WIFI_CONFIGS_FILE
#error "WIFI_CONFIGS_FILE is undefined"
#endif

  if (tft) tft->println("Loading WiFi configs...");

  Serial.println("-- Loading WiFi configs started --");

  T2DStrings* data =
      DataFiles::loadDataFile(WIFI_CONFIGS_FILE, expectedItemsCount);

  try {
    if (data) {
      Serial.printf("Found wifi configs file (%s)\n", WIFI_CONFIGS_FILE);
    } else {
      Serial.printf("No wifi configs file (%s) found\n", WIFI_CONFIGS_FILE);
#ifdef WIFI_CONFIGS
      // WiFi Configs iteration demo (array expected)
      Serial.println("Using predefined configs");
      const TStrings lines = WIFI_CONFIGS;
      data = DataFiles::loadDataFromStrings(&lines, expectedItemsCount);
#endif
    }

    DataFiles::printDataToSerial(data);

  } catch (const std::exception& e) {
    Serial.printf("Exception: %s\n", e.what());
  } catch (...) {
    Serial.println("Unknown exception");
  }

  Serial.println("-- Loading WiFi configs finished --");

  return data;
}
