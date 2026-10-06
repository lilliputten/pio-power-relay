#include "DataFiles.hpp"

// Open the file in write mode (creates a new file or overwrites an existing
// one)
File DataFiles::_openFileForWrite(const char* filename) {
  File file;

  Serial.printf("Opening %s file for write\n", filename);

#ifdef USE_SD
  // 1. Attempt to open directly from SD Card first
  file = SD.open(filename, FILE_WRITE);
  if (file) {
    Serial.printf("Successfully opened %s from SD Card\n", filename);
  } else {
#endif
#ifdef USE_LFS
    // 2. If SD fails, attempt to open directly from LittleFS
    file = LittleFS.open(filename, FILE_WRITE);
    if (file) {
      Serial.printf("Successfully opened %s from LittleFS\n", filename);
    }
#endif
#ifdef USE_SD
  }
#endif

  return file;
}
