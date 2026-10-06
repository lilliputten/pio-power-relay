#include <SD.h>

#include "DataFiles.hpp"
#include "Demos.hpp"

// Iteration count
static long _count = 0;

static const char* filename = "/test.txt";

void Demos::demoDataFiles() {
  // Test reading pre-uploaded files
  Serial.println("\n-- Read LFS/SD strings file: --");
  TStrings* lfsStrings = DataFiles::loadStrings("/lfs-test.txt");
  DataFiles::printStringsToSerial(lfsStrings);

  Serial.println("\n-- Read LFS/SD data file: --");
  T2DStrings* lfsData = DataFiles::loadDataFile("/test.txt");
  DataFiles::printDataToSerial(lfsData);

  // Test writing and re-reading
  long rand = random(10, 99);
  const T2DStrings testData = {
      {"Device Setup " + String(rand), "ESP32 Node 1", "Status OK"},
      {"Sensor Reading", "Temperature 24.5 C", "Humidity 60%"},
      {"System Log", "WiFi Connected", "RSSI -45dBm"},
      {"Command", "Reboot Device", "Delay 500ms"}};

  Serial.println("\n-- Expected file structure on SD card: --");
  DataFiles::printDataToSerial(&testData);

  if (DataFiles::writeDataFile(filename, &testData)) {
    Serial.printf("-- Data written (with new random data: %d) --\n", rand);
  } else {
    Serial.printf("-- Failed to open file for writing (%s) --\n", filename);
  }

  Serial.println("\n-- Re-read just written data: --");
  T2DStrings* readData = DataFiles::loadDataFile(filename);
  DataFiles::printDataToSerial(readData);
}
