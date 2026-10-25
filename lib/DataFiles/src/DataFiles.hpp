#ifndef DataFiles_DEFINED
#define DataFiles_DEFINED

#define USE_LFS  // Use LittleFS
// #define USE_SD // Use SD Card

#include <Arduino.h>

#ifdef USE_LFS
#include <LittleFS.h>
#endif

#ifdef USE_SD
#include <SD.h>
#endif

#include <vector>

#define TStrings std::vector<String>
#define T2DStrings std::vector<TStrings>

class DataFiles {
 private:
  // Helpers
  static const TStrings _parseSingleDataString(
      const String line, const int expectedItemsCount = 0);
  static bool _sdFileExists(const char* filename);
  static File _openFileForRead(const char* filename);
  static File _openFileForWrite(const char* filename);

 public:
  // Interface
  static void initDataFiles();
  static bool loadDataFile(const char* filename, T2DStrings& result,
                           const int expectedItemsCount = 0);
  static bool loadStrings(const char* filename, TStrings& result);
  static bool writeDataFile(const char* filename, const T2DStrings& dataTable,
                            const int expectedItemsCount = 0);
  static bool loadDataFromStrings(const TStrings& strings, T2DStrings& result,
                                  const int expectedItemsCount = 0);
  bool writeStrings(const char* filename, const TStrings& strings);
  static void printDataToSerial(const T2DStrings& dataTable);
  static void printStringsToSerial(const TStrings& strings);
};

#endif
