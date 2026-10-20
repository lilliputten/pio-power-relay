#ifndef InfoScreens_DEFINED
#define InfoScreens_DEFINED

#define InfoScreens_DEBUG

#include <vector>

#include "PowerPlan.hpp"
#include "PowerStation.hpp"
#include "TempSensor.hpp"
#include "TimeUtils.hpp"
#include "constants.hpp"
// TODO: Add other modules

#define Y_OFFSET 4
#define X_OFFSET 4

#define BG_COLOR TFT_BLACK
#define HEADER_BG_COLOR TFT_DARKGREY
#define HEADER_FG_COLOR TFT_WHITE
#define FOOTER_BG_COLOR TFT_BLACK
#define FOOTER_FG_COLOR TFT_DARKGREY
#define HEADER_FONT FONT_XS
#define FOOTER_FONT FONT_XS
#define HEADER_Y_OFFSET 3
#define FOOTER_Y_OFFSET 3

#define DEFAULT_SHOW_TIME 5000
#define DEFAULT_UPDATE_TIME 2000

enum TScreenType {
  DEMO,
  TEMP_SENSOR,
  POWER_CHANNEL,
};
static TScreenType defaultScreen = DEMO;

struct TShowScreen {
  TScreenType type;
  // Show this screen for this time (millis), overrides `defaultShowTime`
  TTick showTime;
  // Update this screen after this time (millis), overrides `defaultUpdateTime`
  TTick updateTime;
};
#define DEFAULT_SHOW_SCREEN_PARAMS 0, 0
#define TShowScreens std::vector<TShowScreen>

static const TShowScreen basicScreens[] = {
    {DEMO, DEFAULT_SHOW_SCREEN_PARAMS},
    {TEMP_SENSOR, DEFAULT_SHOW_SCREEN_PARAMS},
};
static const size_t basicScreensCount =
    sizeof(basicScreens) / sizeof(TShowScreen);
// NOTE: All screens after the `basicScreensCount` are considered as power plan
// ones (one per channel, totally as many as POWER_CHANNELS_COUNT). See
// initialization in the `initScreens` method.

static String getScreenTypeName(TScreenType type) {
  switch (type) {
    case DEMO:
      return "DEMO";
      break;
    case TEMP_SENSOR:
      return "TEMP_SENSOR";
      break;
    case POWER_CHANNEL:
      return "POWER_CHANNEL";
      break;
  };
  return "UNKNOWN";
}

class InfoScreens {
 private:
  bool inited = false;

  TShowScreens screens;

  /** Start position for the regular screen's content */
  TPos screenStartY = Y_OFFSET;

  // Last recorded ticks (just for logging)
  TTick long ticks = 0;
  // The current screen (`currentScreenIdx`) has been displayed since this
  TTick long ticksStarted = 0;
  // Last screen update time
  TTick long ticksUpdated = 0;

  size_t currentTick = 0;
  unsigned int currentLoop = 0;

  // Show each screen (if custom value hasn't been specified for the screen) for
  // this time
  TTick long defaultShowTime = DEFAULT_SHOW_TIME;
  // Update each screen (if custom value hasn't been specified for the screen)
  // for this time
  TTick long defaultUpdateTime = DEFAULT_UPDATE_TIME;

  // Currently displayed screen
  size_t currentScreenIdx = 0;
  // Power plan index ( >=0 if current screen is power plan one)
  int powerPlanIdx = -1;

  // Screen helper references
  TFTUtils* tftUtils = nullptr;
  TFT_eSPI* tft = nullptr;

  // Involved objects' references
  PowerStation* powerStation = nullptr;
  TempSensor* tempSensor = nullptr;

 private:
  int getPowerPlanIdx() {
    if (this->currentScreenIdx >= basicScreensCount) {
      return this->currentScreenIdx - basicScreensCount;
    }
    return -1;
  }

  String getScreenName() {
    const TShowScreen& scr = this->screens[this->currentScreenIdx];
    const TScreenType type = scr.type;
    String name = getScreenTypeName(type);
    name.replace("_", " ");
    if (this->powerPlanIdx >= 0) {
      name += " " + String(this->powerPlanIdx + 1);
    }
    return name;
  }

 public:
  void drawDefaultScreen(const bool full = false) {
    if (!tft) return;

    char strBuf[30];

    const TPos xp = X_OFFSET;
    TPos yp = this->screenStartY;

    long rand = random(1, 200);

    const TShowScreen& scr = this->screens[this->currentScreenIdx];
    const TScreenType type = scr.type;
    const String name = this->getScreenName();

    tft->setTextColor(TFT_MAGENTA);

    tft->setCursor(xp, yp, FONT_MD);
    snprintf(strBuf, sizeof(strBuf), "Screen %d\n", (int)type + 1);
    this->clearLine();
    this->print(strBuf, true);
    yp += tft->fontHeight();

    const TTick timeDiff = this->ticks - this->ticksStarted;
    String diffTimeStr = TimeUtils::getSecPeriodString(timeDiff / 1000);
    TTick showTime = scr.showTime ? scr.showTime : this->defaultShowTime;
    String totalTimeStr = TimeUtils::getSecPeriodString(showTime / 1000);
    String timeStr = diffTimeStr + " / " + totalTimeStr;

    tft->setTextColor(TFT_ORANGE);

    tft->setCursor(xp, yp, FONT_SM);
    snprintf(strBuf, sizeof(strBuf), "%s\n", name.c_str());
    this->clearLine();
    this->print(strBuf, true);
    yp += tft->fontHeight();

    tft->setTextColor(TFT_PINK);

    if (this->powerPlanIdx >= 0) {
      tft->setCursor(xp, yp, FONT_XS);
      snprintf(strBuf, sizeof(strBuf), "Power plan: %d\n",
               this->powerPlanIdx + 1);
      this->clearLine();
      this->print(strBuf, true);
      yp += tft->fontHeight();
    }

    tft->setCursor(xp, yp, FONT_XS);
    snprintf(strBuf, sizeof(strBuf), "Loop: %d\n", this->currentLoop);
    this->clearLine();
    this->print(strBuf, true);
    yp += tft->fontHeight();

    tft->setCursor(xp, yp, FONT_XS);
    snprintf(strBuf, sizeof(strBuf), "Time: %s\n", timeStr.c_str());
    this->clearLine();
    this->print(strBuf, true);
    yp += tft->fontHeight();

    if (full) {
      this->clearRestLines();
    }
  }

  void drawHeader() {
    if (!tft) return;

    const TColor bgColor = HEADER_BG_COLOR;

    TPos xp = X_OFFSET;
    TPos yp = /* Y_OFFSET + */ HEADER_Y_OFFSET;

    tft->setTextColor(HEADER_FG_COLOR, HEADER_BG_COLOR);

    tft->setTextFont(HEADER_FONT);
    tft->setCursor(xp, yp);

    const TPos fontHeight = tft->fontHeight();

    const TPos height = fontHeight + /* Y_OFFSET + */ HEADER_Y_OFFSET * 2;
    // Serial.printf("[drawHeader] fontHeight: %d\n", fontHeight);
    // Serial.printf("[drawHeader] height: %d\n", height);
    tft->fillRect(0, 0, TFT_SCREEN_WIDTH, height, HEADER_BG_COLOR);

    char strBuf[20];

    snprintf(strBuf, sizeof(strBuf), "Screen %d/%d", this->currentScreenIdx + 1,
             this->screens.size());
    this->print(strBuf, true);
    xp += tft->textWidth(strBuf) + 5;

    /* // Print tick
     * snprintf(strBuf, sizeof(strBuf), "tick %d", this->currentTick + 1);
     * tft->setCursor(xp, yp);
     * this->print(strBuf, true);
     * xp += tft->textWidth(strBuf) + 5;
     */

    const TShowScreen& scr = this->screens[this->currentScreenIdx];

    const TTick timeDiff = this->ticks - this->ticksStarted;
    String diffTimeStr = TimeUtils::getSecPeriodString(timeDiff / 1000);
    TTick showTime = scr.showTime ? scr.showTime : this->defaultShowTime;
    String totalTimeStr = TimeUtils::getSecPeriodString(showTime / 1000);
    String rightStr = diffTimeStr + " / " + totalTimeStr;

    const TPos rightBoxWidth = tft->textWidth(rightStr) + X_OFFSET * 2;
    const TPos rxp = TFT_SCREEN_WIDTH - rightBoxWidth;
    // Serial.printf("[drawHeader] rightBoxWidth=%d\n", rightBoxWidth);
    tft->setCursor(rxp + X_OFFSET, yp);
    tft->fillRect(rxp, yp, rightBoxWidth, fontHeight, HEADER_BG_COLOR);
    this->print(rightStr, true);

    // yp += tft->fontHeight();
  }

  void drawFooter() {
    if (!tft) return;

    tft->setTextFont(FOOTER_FONT);

    const TPos fontHeight = tft->fontHeight();

    static const String statusStr = ">>>";

    const TPos yp = TFT_SCREEN_HEIGHT - fontHeight - FOOTER_Y_OFFSET * 2;
    const TPos rightBoxWidth = tft->textWidth(statusStr) + X_OFFSET * 2;
    const TPos xp = TFT_SCREEN_WIDTH - rightBoxWidth;
    tft->fillRect(xp, yp, rightBoxWidth, fontHeight + FOOTER_Y_OFFSET * 2,
                  FOOTER_BG_COLOR);
    const auto isOddTick = this->currentTick % 2;
    if (!isOddTick) {
      tft->setCursor(xp + X_OFFSET, yp + FOOTER_Y_OFFSET);
      tft->setTextColor(FOOTER_FG_COLOR, FOOTER_BG_COLOR);
      this->print(statusStr, true);
    }

    // xp += tft->textWidth(statusStr);
    // yp += fontHeight;
  }

  void redrawScreen(const bool full = false) {
    // TODO: Detect current screen and invoke the dedicated drawing method
    this->drawDefaultScreen(full);
  }

  void tick() {
    if (!tft) return;

    const TTick ticks = millis();
    this->ticks = ticks;
    const TShowScreen& scr = this->screens[this->currentScreenIdx];

    TTick showTime = scr.showTime ? scr.showTime : this->defaultShowTime;
    TTick updateTime =
        scr.updateTime ? scr.updateTime : this->defaultUpdateTime;

    const bool doNext = !showTime || ticks - this->ticksStarted >= showTime;
    const bool doUpdate =
        !updateTime || ticks - this->ticksUpdated >= updateTime;

    this->currentTick++;

    if (doNext) {
      this->nextScreen();
      this->ticksStarted = ticks;
    }

    this->drawHeader();
    if (doNext || doUpdate) {
      this->redrawScreen(doNext);
      this->ticksUpdated = ticks;
    }
    this->drawFooter();

    // Show/update screens
  }

  void resetLoop() {
    this->currentLoop = 0;
    this->currentScreenIdx = 0;
    this->powerPlanIdx = -1;
    this->ticks = 0;
    this->ticksStarted = 0;
    this->ticksUpdated = 0;
    this->currentTick = 0;
  }

  void nextScreen() {
    if (++this->currentScreenIdx >= this->screens.size()) {
      this->currentScreenIdx = 0;
      this->currentLoop++;
    }
    this->powerPlanIdx = (int)this->currentScreenIdx - basicScreensCount;
  }

  void setPowerStation(PowerStation* ref) { this->powerStation = ref; }
  void setTempSensor(TempSensor* ref) { this->tempSensor = ref; }

  void clearScreen() {
    if (tft) tft->fillScreen(BG_COLOR);
  }

  void clearRestLines(const TColor bgColor = BG_COLOR) {
    if (tft) {
      const TPos yp = tft->getCursorY();
      const TPos height = tft->fontHeight();
      tft->fillRect(0, yp, TFT_SCREEN_WIDTH, TFT_SCREEN_HEIGHT - yp, bgColor);
    }
  }

  void clearLine(const TColor bgColor = BG_COLOR) {
    if (tft) {
      const TPos yp = tft->getCursorY();
      const TPos height = tft->fontHeight();
      tft->fillRect(0, yp, TFT_SCREEN_WIDTH, height, bgColor);
    }
  }

  void print(String str, const bool onlyTFT = false) {
    if (!onlyTFT) Serial.print(str);
    if (tft) tft->print(str);
  }

  void initScreens() {
    if (tftUtils) tftUtils->writeLine("Starting Screens...");

    // Init settings

    this->screenStartY = Y_OFFSET + (tft ? tft->fontHeight(HEADER_FONT) : 16) +
                         HEADER_Y_OFFSET * 2;

    // Check object references
    if (!this->powerStation)
      Serial.println(YELLOW "[InfoScreens]: PowerStation absent!" RESET);
    if (!this->tempSensor)
      Serial.println(YELLOW "[InfoScreens]: TempSensor absent!" RESET);
    // TODO: Check other required references

    // Add screens for each power plan...
    this->screens.reserve(basicScreensCount + POWER_CHANNELS_COUNT);
    for (size_t i = 0; i < POWER_CHANNELS_COUNT; i++) {
      this->screens.push_back({POWER_CHANNEL, DEFAULT_SHOW_SCREEN_PARAMS});
    }

#ifdef InfoScreens_DEBUG
    // DEBUG
    Serial.printf("[InfoScreens] Screens count=%d basicScreens=%d\n",
                  this->screens.size(), basicScreensCount);
    for (size_t i = 0; i < this->screens.size(); i++) {
      const TShowScreen& scr = this->screens[i];
      const TScreenType type = scr.type;
      const String name = getScreenTypeName(type);
      Serial.printf("[InfoScreens] %-3d. Screen %d (%s)", i, type,
                    name.c_str());
      // It's a power plan screen
      if (powerPlanIdx >= 0) {
        Serial.printf(" Power plan #%d", this->powerPlanIdx + 1);
      }
      Serial.println();
    }
#endif

    this->resetLoop();

    this->inited = true;
  }

  // Constructor
  InfoScreens(TFTUtils* tftUtils = nullptr)
      : screens(basicScreens, basicScreens + basicScreensCount) {
    if (tftUtils) {
      this->tftUtils = tftUtils;
      this->tft = &tftUtils->tft;
    }
  }
};

#endif  // InfoScreens_DEFINED
