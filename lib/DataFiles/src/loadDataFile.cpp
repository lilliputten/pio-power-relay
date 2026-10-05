#include "DataFiles.hpp"

T2DStringsData* DataFiles::loadDataFile(const char* filename, const int expectedItemsCount) {
  // Initialize an empty 2D vector to hold the result
  // T2DStringsData result2D;
  auto* result2D = new T2DStringsData();

  if (!SD.exists(filename)) {
    Serial.printf("File %s does not exist\n", filename);
    return nullptr;
  }

  // Open the file in read mode
  File file = SD.open(filename, FILE_READ);
  if (!file || file.isDirectory()) {
    Serial.printf("Failed to open file for reading: %s\n", filename);
    return nullptr;
  }

  // Read the file line by line
  while (file.available()) {
    String line = file.readStringUntil('\n');

    line.trim(); // Remove trailing '\r' and whitespace
    // if (line.endsWith("\r")) line.remove(line.length() - 1); // Trim trailing \r (Windows line endings)

    // Skip empty or commented (with leading '#') lines
    if (line.length() == 0 || line.startsWith("#")) {
      continue;
    }

    std::vector<String> currentLineWords;
    int startIndex = 0;
    int spaceIndex = line.indexOf(' ');
    // int itemsCount = 0;

    // Split the line by spaces
    while (spaceIndex != -1) {
      String word = line.substring(startIndex, spaceIndex);
      word.trim();
      word.replace("%20", " ");
      if (word.length() > 0) {
        currentLineWords.push_back(word);
      }
      startIndex = spaceIndex + 1;
      spaceIndex = line.indexOf(' ', startIndex);
    }

    // Add the last word of the line
    String lastWord = line.substring(startIndex);
    lastWord.trim();
    lastWord.replace("%20", " ");
    if (lastWord.length() > 0) {
      currentLineWords.push_back(lastWord);
    }

    while (currentLineWords.size() < expectedItemsCount) {
      currentLineWords.push_back("");
    }

    // Add the parsed line row to our 2D array if it contains elements
    if (!currentLineWords.empty()) {
      result2D->push_back(currentLineWords);
    }
  }

  file.close();
  return result2D;
}
