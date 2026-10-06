#include "DataFiles.hpp"

// Open the file in read mode (trying to open on an SD Card first, then on
// LittleFS)
File DataFiles::_openFileForRead(const char* filename) {
  File file;

  Serial.printf("Opening %s file for read\n", filename);

#ifdef USE_SD
  // 1. Attempt to open directly from SD Card first
  file = SD.open(filename, FILE_READ);
  if (file) {
    Serial.printf("Successfully opened %s from SD Card\n", filename);
  }
  // 2. If SD fails, attempt to open directly from LittleFS
  else {
#endif
#ifdef USE_LFS
    file = LittleFS.open(filename, FILE_READ);
    if (file) {
      Serial.printf("Successfully opened %s from LittleFS\n", filename);
    }
#endif
#ifdef USE_SD
  }
#endif

  return file;
}
