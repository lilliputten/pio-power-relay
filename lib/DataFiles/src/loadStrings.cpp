#include "DataFiles.hpp"

TStrings* DataFiles::loadStrings(const char* filename) {
  // Initialize an empty array to hold the result
  // TStrings result2D;
  auto* result2D = new TStrings();

  File file = DataFiles::_openFileForRead(filename);

  if (!file || file.isDirectory()) {
    Serial.printf("Failed to open %s for read\n", filename);
    return nullptr;
  }

  // Read the file line by line
  while (file.available()) {
    String line = file.readStringUntil('\n');

    line.trim(); // Remove trailing '\r' and whitespace

    // Add the parsed line
    result2D->push_back(line);
  }

  file.close();
  return result2D;
}
