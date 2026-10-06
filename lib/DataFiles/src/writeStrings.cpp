#include "DataFiles.hpp"

// Function to write string array
bool DataFiles::writeStrings(const char* filename, const TStrings* strings) {
  if (!strings) {
    return false;
  }

  // Open the file in write mode (creates a new file or overwrites an existing
  // one)
  File file = DataFiles::_openFileForWrite(filename);

  if (!file || file.isDirectory()) {
    Serial.printf("Failed to open %s for write\n", filename);
    return false;
  }

  // Iterate through each row of the strings vector
  for (size_t i = 0; i < strings->size(); i++) {
    String row = (*strings)[i];
    file.println(row);
  }

  file.close();

  return true;
}
