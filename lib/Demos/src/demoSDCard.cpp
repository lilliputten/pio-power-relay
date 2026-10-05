#include <SD.h>

#include "DataFiles.hpp"
#include "Demos.hpp"

// Iteration count
static long _count = 0;

static const char* filename = "/test.txt";

/* [>* Read a text file from SD and display it on Serial <]
 * void readAndDisplayFile(const char* filename) {
 *   if (!SD.exists(filename)) {
 *     Serial.printf("File %s does not exist\n", filename);
 *     return;
 *   }
 *
 *   File file = SD.open(filename, FILE_READ);
 *   if (!file) {
 *     Serial.printf("Failed to open file for reading (%s)\n", filename);
 *     return;
 *   }
 *
 *   Serial.printf("Reading %s (%u bytes)...\n", filename, file.size());
 *
 *   // ---- Read line by line ----
 *   int lineCount = 0;
 *   while (file.available()) {
 *     String line = file.readStringUntil('\n');
 *
 *     // Trim trailing \r (Windows line endings)
 *     if (line.endsWith("\r")) line.remove(line.length() - 1);
 *
 *     // Print to Serial
 *     Serial.printf("<%s>\n", line.c_str());
 *
 *     lineCount++;
 *   }
 *
 *   file.close();
 *   Serial.printf("Done. %d lines read\n", lineCount);
 * }
 */

void Demos::demoSDCardInit() {
  long rand = random(10, 99);
  const T2DStringsData testData = {
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

  T2DStringsData* readData = DataFiles::loadDataFile(filename);
  Serial.println("\n-- Re-read data: --");
  DataFiles::printDataToSerial(readData);
}

int Demos::demoSDCardTick(TFT_eSPI& tft, int yp) {
  // Return line height (0 = nothing has printed)
  return 0;
}
