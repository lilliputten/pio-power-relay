#include "DataFiles.hpp"

bool DataFiles::loadStrings(const char* filename, TStrings& result) {
  // Initialize an empty array to hold the result
  // TStrings result;

  File file = DataFiles::_openFileForRead(filename);

  if (!file || file.isDirectory()) {
    Serial.printf("Failed to open %s for read\n", filename);
    return false;
  }

  result.clear();

  // Read the file line by line
  while (file.available()) {
    String line = file.readStringUntil('\n');

    line.trim();  // Remove trailing '\r' and whitespace

    // Add the parsed line
    result.push_back(line);
  }

  file.close();

  return true;
}
