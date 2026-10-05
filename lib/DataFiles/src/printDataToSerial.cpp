#include "DataFiles.hpp"

void DataFiles::printDataToSerial(const T2DStringsData* dataTable) {
  int rowNo = 0;
  for (const auto& row : *dataTable) {
    Serial.print(String(rowNo++) + ": ");
    for (size_t i = 0; i < row.size(); i++) {
      String word = row[i];
      // word.replace(" ", "%20");
      Serial.print(word + (i < row.size() - 1 ? " | " : ""));
    }
    Serial.println();
  }
}
