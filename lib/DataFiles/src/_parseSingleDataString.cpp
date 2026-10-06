#include "DataFiles.hpp"

const TStrings DataFiles::_parseSingleDataString(const String line,
                                                 const int expectedItemsCount) {
  TStrings currentLineWords;
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

  return currentLineWords;
}
