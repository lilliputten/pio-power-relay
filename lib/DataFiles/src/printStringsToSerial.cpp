#include "DataFiles.hpp"

void DataFiles::printStringsToSerial(const TStrings* strings) {
  int rowNo = 0;
  for (const auto& row : *strings) {
    Serial.print(String(rowNo++) + ": ");
    Serial.println(row);
  }
}
