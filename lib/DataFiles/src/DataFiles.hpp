#ifndef DataFiles_DEFINED
#define DataFiles_DEFINED

#include <Arduino.h>

// #include "FS.h"
#include <SD.h>
// #include "SPIFFS.h"
// #include "Vector.h" // Optional, but standard std::vector is used here for dynamic memory management
#include <vector>

#define T2DStringsData std::vector<std::vector<String>>

class DataFiles {
 public:
  static void initDataFiles();
  static T2DStringsData* loadDataFile(const char* filename, const int expectedItemsCount = 0);
  static bool writeDataFile(const char* filename, const T2DStringsData* dataTable, const int expectedItemsCount = 0);
  static void printDataToSerial(const T2DStringsData* dataTable);
};

#endif
