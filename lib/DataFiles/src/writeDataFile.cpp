#include "DataFiles.hpp"

// Function to write 2D string array with space encoding
bool DataFiles::writeDataFile(const char* filename, const T2DStrings* dataTable,
                              const int expectedItemsCount) {
  if (!dataTable) {
    return false;
  }

  // Open the file in write mode (creates a new file or overwrites an existing
  // one)
  File file = DataFiles::_openFileForWrite(filename);

  if (!file || file.isDirectory()) {
    Serial.printf("Failed to open %s for write\n", filename);
    return false;
  }

  // Iterate through each row of the 2D vector
  for (size_t i = 0; i < dataTable->size(); i++) {
    String lineBuffer = "";

    // Iterate through each word in the current row
    int dataItemsCount = (*dataTable)[i].size();
    int loopItemsCount =
        expectedItemsCount ? expectedItemsCount : dataItemsCount;
    for (size_t j = 0; j < loopItemsCount; j++) {
      bool isLast = j == loopItemsCount - 1;
      bool hasData = j < dataItemsCount;

      // Copy the word so we don't modify the original array data
      String word = hasData ? (*dataTable)[i][j] : "";

      // Encode inner spaces within the word to "%20"
      word.replace(" ", "%20");

      // Append the processed word to our line buffer
      lineBuffer += word;

      // Add a separator space between words, but not after the last word
      if (!isLast) {
        lineBuffer += " ";
      }
    }

    // Write the completed line to the SD card followed by a newline character
    file.println(lineBuffer);
  }

  file.close();

  return true;
}
