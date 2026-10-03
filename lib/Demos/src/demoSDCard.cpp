#include <SD.h>

#include "Demos.hpp"

// Iteration count
static long _count = 0;

static const char* filename = "/test.txt";

/** Read a text file from SD and display it on Serial */
void readAndDisplayFile(const char* filename) {
  if (!SD.exists(filename)) {
    Serial.printf("File %s does not exist\n", filename);
    return;
  }

  File file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.printf("Failed to open file for reading (%s)\n", filename);
    return;
  }

  Serial.printf("Reading %s (%u bytes)...\n", filename, file.size());

  // ---- Read line by line ----
  int lineCount = 0;
  while (file.available()) {
    String line = file.readStringUntil('\n');

    // Trim trailing \r (Windows line endings)
    if (line.endsWith("\r")) line.remove(line.length() - 1);

    // Print to Serial
    Serial.println(line);

    lineCount++;
  }

  file.close();
  Serial.printf("Done. %d lines read\n", lineCount);
}

void Demos::demoSDCardInit() {
  Serial.println("SD Card initialization started");

  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialized");

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

  // Read and display the file
  readAndDisplayFile(filename);

  // Write to a file
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    long rand = random(10, 99);
    dataFile.printf("Random data: %d\n", rand);
    dataFile.close();
    Serial.printf("Data written (new random data: %d)\n", rand);
  } else {
    Serial.printf("Failed to open file for writing (%s)\n", filename);
  }
}

int Demos::demoSDCardTick(TFT_eSPI& tft, int yp) {
  // Return line height (0 = nothing has printed)
  return 0;
}
