#ifndef InfoScreens_DEFINED
#define InfoScreens_DEFINED

#include <vector>

#include "PowerPlan.hpp"
#include "PowerStation.hpp"
#include "TempSensor.hpp"
#include "TimeUtils.hpp"
#include "constants.hpp"
// TODO: Add other modules

#define Y_OFFSET 5
#define X_OFFSET 5

#define BG_COLOR TFT_BLACK

class InfoScreens {
 private:
  bool inited = false;

  TTick long ticks = 0;
  TTick long ticksStarted = 0;

  TFTUtils* tftUtils = nullptr;
  TFT_eSPI* tft = nullptr;

  PowerStation* powerStation = nullptr;
  TempSensor* tempSensor = nullptr;

 public:
  void showTestScreen() {
    if (!tft) return;

    char strBuf[20];

    const TPos xp = X_OFFSET;
    TPos yp = Y_OFFSET;

    tft->setCursor(xp, yp, FONT_MD);
    tft->setTextColor(TFT_MAGENTA);

    long rand = random(1, 200);

    snprintf(strBuf, sizeof(strBuf), "Test - %d\n", rand);
    this->clearLine();
    this->print(strBuf);
    yp += tft->fontHeight();

    tft->setCursor(xp, yp, FONT_SM);
    tft->setTextColor(TFT_GOLD);

    const TTick ticksDiff = this->ticks - this->ticksStarted;
    const TTimeSec diffSec = ticksDiff / 1000;
    String timeStr = TimeUtils::getSecPeriodString(diffSec, 0);

    snprintf(strBuf, sizeof(strBuf), "Time: %s\n", timeStr.c_str());
    this->clearLine();
    this->print(strBuf);
    yp += tft->fontHeight();
  }

  void tick() {
    if (!tft) return;

    this->ticks = millis();
    const TTick ticksDiff = this->ticks - this->ticksStarted;

    // Show/update screens
    this->showTestScreen();
  }

  void setPowerStation(PowerStation* ref) { this->powerStation = ref; }
  void setTempSensor(TempSensor* ref) { this->tempSensor = ref; }

  void clearScreen() {
    if (tft) tft->fillScreen(BG_COLOR);
  }

  void clearRestLines() {
    if (tft) {
      const TPos yp = tft->getCursorY();
      const TPos height = tft->fontHeight();
      tft->fillRect(0, yp, TFT_SCREEN_WIDTH, TFT_SCREEN_HEIGHT - yp, BG_COLOR);
    }
  }

  void clearLine() {
    if (tft) {
      const TPos yp = tft->getCursorY();
      const TPos height = tft->fontHeight();
      tft->fillRect(0, yp, TFT_SCREEN_WIDTH, height, BG_COLOR);
    }
  }

  void print(String str) {
    Serial.print(str);
    if (tft) tft->print(str);
  }

  void initScreens() {
    if (tftUtils) tftUtils->writeLine("Starting Screens...");

    if (!this->powerStation)
      Serial.println(YELLOW "[InfoScreens]: PowerStation absent!" RESET);
    if (!this->tempSensor)
      Serial.println(YELLOW "[InfoScreens]: TempSensor absent!" RESET);
    // TODO: Check other required references

    this->ticksStarted = millis();

    this->inited = true;
  }

  // Constructor
  InfoScreens(TFTUtils* tftUtils = nullptr) {
    if (tftUtils) {
      this->tftUtils = tftUtils;
      this->tft = &tftUtils->tft;
    }
  }
};

#endif  // InfoScreens_DEFINED
