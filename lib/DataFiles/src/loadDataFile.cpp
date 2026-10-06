#include "DataFiles.hpp"

T2DStrings* DataFiles::loadDataFile(const char* filename,
                                    const int expectedItemsCount) {
  // Initialize an empty 2D vector to hold the result
  T2DStrings* result2D = new T2DStrings();

  File file = DataFiles::_openFileForRead(filename);

  if (!file || file.isDirectory()) {
    Serial.printf("Failed to open %s for read\n", filename);
    return nullptr;
  }

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
      result2D->push_back(currentLineWords);
    }
  }

  file.close();

  return result2D;
}
