#include "DataFiles.hpp"

void DataFiles::initDataFiles() {
  Serial.println("-- SD Card initialization started --");

  if (!SD.begin(SD_CS)) {
    Serial.println("-- SD Card initialization failed! --");
    return;
  }

  // Print card info
  uint8_t cardType = SD.cardType();
  Serial.print("Card Type: ");
  switch (cardType) {
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SDSC");
      break;
    case CARD_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }
  Serial.printf("Card Size: %llu MB\n", SD.cardSize() / (1024 * 1024));

  Serial.println("-- SD Card initialized --");
}

