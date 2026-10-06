#include "DataFiles.hpp"

void DataFiles::printDataToSerial(const T2DStrings* dataTable) {
  if (!dataTable) {
    return;
  }
  int rowNo = 0;
  for (const auto& row : *dataTable) {
    Serial.print(String(rowNo++) + ": ");
    for (size_t i = 0; i < row.size(); i++) {
      auto word = row[i];
      // word.replace(" ", "%20");
      Serial.print(word);
      if (i < row.size() - 1) {
        Serial.print(" | ");
      }
    }
    Serial.println();
  }
}
