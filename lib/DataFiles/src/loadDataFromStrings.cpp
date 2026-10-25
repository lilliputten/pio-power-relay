#include "DataFiles.hpp"

bool DataFiles::loadDataFromStrings(const TStrings& strings, T2DStrings& result,
                                    const int expectedItemsCount) {
  result.clear();

  if (!strings.size()) {
    return false;
  }

  // result.reserve(strings.size());

  // Read the strings array line by line
  for (String line : strings) {
    line.trim();  // Remove trailing '\r' and whitespace
    // // Trim trailing \r (Windows line endings)
    // if (line.endsWith("\r")) line.remove(line.length() - 1);

    // Skip empty (!) or commented (with leading '#') lines
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

  return true;
}
