#include "DataFiles.hpp"

const char* totalTemplate = "Total Partition Size: %.2f %s (%u B)\n";
const char* usedTemplate = "Storage Space Used:   %.2f %s (%u B)\n";
const char* freeTemplate = "Remaining Free Space: %.2f %s (%u B)\n";

#ifdef USE_SD
static void printSDInfo() {
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

  size_t total = SD.cardSize();
  size_t used = SD.usedBytes();
  size_t freeSpace = total - used;

  const float fraction = 1024.0 * 1024.0;  // MBs
  const String unitStr = "MB";

  // Print out the data formatted to the Serial monitor
  Serial.printf(totalTemplate, (float)total / fraction, unitStr, total);
  Serial.printf(usedTemplate, (float)used / fraction, unitStr, used);
  Serial.printf(freeTemplate, (float)freeSpace / fraction, unitStr, freeSpace);

  // Percentage Used calculation
  float usePercentage = ((float)used / (float)total) * 100.0;
  Serial.printf("Storage Utilization:  %.2f%%\n", usePercentage);
}

static void initSD() {
  Serial.println("-- SD Card initialization started --");

  if (!SD.begin(SD_CS)) {
    Serial.println("-- SD Card initialization failed! --");
    return;
  }

  // Print SD card info
  printSDInfo();

  Serial.println("-- SD Card initialized --");
}
#endif

#ifdef USE_LFS
static void printLittleFSInfo() {
  // Get raw byte metrics
  size_t total = LittleFS.totalBytes();
  size_t used = LittleFS.usedBytes();
  size_t freeSpace = total - used;

  const float fraction = 1024.0;  // KBs
  const String unitStr = "KB";

  // Print out the data formatted to the Serial monitor
  Serial.printf(totalTemplate, (float)total / fraction, unitStr, total);
  Serial.printf(usedTemplate, (float)used / fraction, unitStr, used);
  Serial.printf(freeTemplate, (float)freeSpace / fraction, unitStr, freeSpace);

  // Percentage Used calculation
  float usePercentage = ((float)used / (float)total) * 100.0;
  Serial.printf("Storage Utilization:  %.2f%%\n", usePercentage);
}

static void initLittleFS() {
  Serial.println("-- LittleFS initialization started --");

  if (!LittleFS.begin(true)) {
    Serial.println("-- LittleFS initialization failed! --");
    return;
  }

  // Print LittleFS info
  printLittleFSInfo();

  Serial.println("-- LittleFS initialized --");
}
#endif

void DataFiles::initDataFiles() {
#ifdef USE_SD
  initSD();
#endif
#ifdef USE_LFS
  initLittleFS();
#endif
}
