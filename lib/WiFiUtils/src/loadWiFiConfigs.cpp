#include "WiFiUtils.hpp"

// WiFi config row items (SSID, Password)
const int expectedItemsCount = 2;

T2DStrings WiFiUtils::loadWiFiConfigs() {
#ifndef WIFI_CONFIGS_FILE
#error "WIFI_CONFIGS_FILE is undefined"
#endif

  // Serial.println("-- Loading WiFi configs started --");

  if (tftUtils) tftUtils->writeLine("Loading WiFi configs...");

  T2DStrings data;

  bool success =
      DataFiles::loadDataFile(WIFI_CONFIGS_FILE, data, expectedItemsCount);

  try {
    if (success) {
      Serial.printf("Found wifi configs file (%s)\n", WIFI_CONFIGS_FILE);
    } else {
      Serial.printf("No wifi configs file (%s) found\n", WIFI_CONFIGS_FILE);
#ifdef WIFI_CONFIGS
      // WiFi Configs iteration demo (array expected)
      Serial.println("Using predefined configs");
      const TStrings lines = WIFI_CONFIGS;
      success = DataFiles::loadDataFromStrings(lines, data, expectedItemsCount);
#endif
    }

    // DataFiles::printDataToSerial(data);

    if (tftUtils) {
      int rowNo = 0;
      for (const auto& row : data) {
        tftUtils->writeLine(String(++rowNo) + ": " + row[0]);
      }
    }

  } catch (const std::exception& e) {
    Serial.printf("Exception: %s\n", e.what());
  } catch (...) {
    Serial.println("Unknown exception");
  }

  // Serial.println("-- Loading WiFi configs finished --");

  return data;
}
