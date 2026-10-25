#include "DataFiles.hpp"

bool DataFiles::loadDataFile(const char* filename, T2DStrings& result,
                             const int expectedItemsCount) {
  // Initialize an empty 2D vector to hold the result
  // T2DStrings result;  // = new T2DStrings();

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
    // // Trim trailing \r (Windows line endings)
    // if (line.endsWith("\r")) line.remove(line.length() - 1);

    // Skip empty or commented (with leading '#') lines
    if (line.length() == 0 || line.startsWith("#")) {
      continue;
    }

    std::vector<String> currentLineWords =
        DataFiles::_parseSingleDataString(line, expectedItemsCount);

    // Add the parsed line row to our 2D array if it contains elements
    if (!currentLineWords.empty()) {
      result.push_back(currentLineWords);
    }
  }

  file.close();

  return true;
}
