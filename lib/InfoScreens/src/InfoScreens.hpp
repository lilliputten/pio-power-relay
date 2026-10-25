#ifndef InfoScreens_DEFINED
#define InfoScreens_DEFINED

#define InfoScreens_DEBUG

#include <vector>

#include "TFTUtils.hpp"

#ifdef USE_POWER_STATION
#include "PowerPlan.hpp"
#include "PowerStation.hpp"
#endif
#ifdef USE_HW_BUTTON
#include "HwButton.hpp"
#endif
#ifdef USE_TEMP_SENSOR
#include "TempSensor.hpp"
#endif
#ifdef USE_WIFI_UTILS
#include "WiFiUtils.hpp"
#endif

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
#define DEFAULT_UPDATE_TIME 1000
#define MEDIUM_UPDATE_TIME 10000
#define LONG_UPDATE_TIME 60000
#define DEFAULT_SYSTEM_TIME 500

/** Core screen types. Don't forget to add to the used screens list
 * `basicScreens` and translate the code to the name in the function
 * `getScreenTypeName`. */
enum TScreenType {
  DEMO,
  TEMP_SENSOR,
  WIFI_NETWORK,
  WIFI_ACCESS_POINT,
  POWER_CHANNEL,
};
static TScreenType defaultScreen = DEMO;

struct TShowScreen {
  TScreenType type;
  // Update this screen after this time (millis), overrides `defaultUpdateTime`
  TTick updateTime;
  // Show this screen for this time (millis), overrides `defaultShowTime`
  TTick showTime;
};
#define TShowScreens std::vector<TShowScreen>

// Default screen params (set updateTime, showTime if needed)
#define DEFAULT_SHOW_SCREEN_PARAMS 0, 0

static const TShowScreen basicScreens[] = {
// {DEMO, DEFAULT_SHOW_SCREEN_PARAMS},
#ifdef USE_TEMP_SENSOR
    {TEMP_SENSOR, MEDIUM_UPDATE_TIME, 0},
#endif
#ifdef USE_WIFI_UTILS
    {WIFI_NETWORK, LONG_UPDATE_TIME, 0},
    {WIFI_ACCESS_POINT, LONG_UPDATE_TIME, 0},
#endif
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
    case TEMP_SENSOR:
      return "TEMP_SENSOR";
    case WIFI_NETWORK:
      return "WIFI_NETWORK";
    case WIFI_ACCESS_POINT:
      return "WIFI_ACCESS_POINT";
    case POWER_CHANNEL:
      return "POWER_CHANNEL";
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
  // Last system update time
  TTick long ticksSystem = 0;

  size_t currentTick = 0;
  unsigned int currentLoop = 0;

  bool paused = false;

  TTick lastPressed = 0;

  // Show each screen (if custom value hasn't been specified for the screen) for
  // this time
  TTick long defaultShowTime = DEFAULT_SHOW_TIME;
  // Update each screen (if custom value hasn't been specified for the screen)
  // for this time
  TTick long defaultUpdateTime = DEFAULT_UPDATE_TIME;
  // Do system tasks (eg, update header and footer) once in this time period
  TTick long defaultSystemTime = DEFAULT_SYSTEM_TIME;

  bool cursorState = false;

  // Currently displayed screen
  size_t currentScreenIdx = 0;
  // Power plan index ( >=0 if current screen is power plan one)
  int powerPlanIdx = -1;

  // Screen helper references
  TFTUtils* tftUtils = nullptr;
  TFT_eSPI* tft = nullptr;

  // Involved objects' references
#ifdef USE_POWER_STATION
  PowerStation* powerStation = nullptr;
#endif
#ifdef USE_HW_BUTTON
  HwButton* hwButton = nullptr;
#endif
#ifdef USE_TEMP_SENSOR
  TempSensor* tempSensor = nullptr;
#endif
#ifdef USE_WIFI_UTILS
  WiFiUtils* wiFiUtils = nullptr;
#endif

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
    snprintf(strBuf, sizeof(strBuf), "Screen #%d\n", this->currentScreenIdx);
    this->clearLine();
    tft->print(strBuf);
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
    tft->print(strBuf);
    yp += tft->fontHeight();

    tft->setTextColor(TFT_PINK);

    if (this->powerPlanIdx >= 0) {
      tft->setCursor(xp, yp, FONT_XS);
      snprintf(strBuf, sizeof(strBuf), "Power plan: %d\n",
               this->powerPlanIdx + 1);
      this->clearLine();
      tft->print(strBuf);
      yp += tft->fontHeight();
    }

    tft->setCursor(xp, yp, FONT_XS);
    snprintf(strBuf, sizeof(strBuf), "Loop: %d\n", this->currentLoop);
    this->clearLine();
    tft->print(strBuf);
    yp += tft->fontHeight();

    tft->setCursor(xp, yp, FONT_XS);
    snprintf(strBuf, sizeof(strBuf), "Time: %s\n", timeStr.c_str());
    this->clearLine();
    tft->print(strBuf);
    yp += tft->fontHeight();

    if (full) {
      this->clearRestLines();
    }
  }

#ifdef USE_TEMP_SENSOR
  void drawTempSensorScreen(const bool full = false) {
    if (!tft) return;

    // Get object reference
    if (!this->tempSensor) {
      return this->drawDefaultScreen(full);
    }

    // Get data
    const float tempC = this->tempSensor->getTemperature();

    const TPos xp = X_OFFSET;
    TPos yp = this->screenStartY;

    // Get screen
    const TShowScreen& scr = this->screens[this->currentScreenIdx];
    const TScreenType type = scr.type;
    const String name = this->getScreenName();  // TODO: Get plan name?

    tft->setTextColor(TFT_MAGENTA);

    // Display screen name
    tft->setCursor(xp, yp, FONT_SM);
    this->clearLine();
    tft->println(name);
    yp += tft->fontHeight();

    // Display temperature
    tft->setTextColor(TFT_GREEN);

    tft->setCursor(xp, yp, FONT_MD);
    this->clearLine();
    tft->printf("%.1f`C\n", tempC);
    yp += tft->fontHeight();

    // Clean
    if (full) {
      this->clearRestLines();
    }
  }
#endif

#ifdef USE_WIFI_UTILS
  void drawWiFiNetworkScreen(const bool full = false) {
    if (!tft) return;

    // Get object reference
    if (!this->wiFiUtils) {
      return this->drawDefaultScreen(full);
    }

    // Get data
    const String ssid = wiFiUtils->getWiFiClientSSID();
    const String ip = wiFiUtils->getWiFiClientIP();
    const bool isConnected = ssid && ip;

    const TPos xp = X_OFFSET;
    TPos yp = this->screenStartY;

    // Get screen
    const TShowScreen& scr = this->screens[this->currentScreenIdx];
    const TScreenType type = scr.type;
    const String name = this->getScreenName();  // TODO: Get plan name?

    tft->setTextColor(TFT_MAGENTA);

    // Display screen name
    tft->setCursor(xp, yp, FONT_SM);
    this->clearLine();
    tft->println(name);
    yp += tft->fontHeight();

    // Display status/error
    tft->setTextColor(isConnected ? TFT_GREEN : TFT_BLUE);  // green/red!

    tft->setCursor(xp, yp, FONT_SM);
    this->clearLine();
    tft->println(isConnected ? "Connected" : "Not connected");
    yp += tft->fontHeight();

    // Display data
    tft->setTextColor(TFT_PINK);

    tft->setCursor(xp, yp, FONT_XS);
    this->clearLine();
    tft->printf("SSID: %s\n", ssid.c_str());
    yp += tft->fontHeight();

    tft->setCursor(xp, yp);
    this->clearLine();
    tft->printf("IP: %s\n", ip.c_str());
    yp += tft->fontHeight();

    // Clean
    if (full) {
      this->clearRestLines();
    }
  }

  void drawAccessPointScreen(const bool full = false) {
    if (!tft) return;

    // Get object reference
    if (!this->wiFiUtils) {
      return this->drawDefaultScreen(full);
    }

    // Get data

    const String ssid = this->wiFiUtils->getWiFiAPSSID();
    const String pwd = this->wiFiUtils->getWiFiAPPwd();
    const String ip = this->wiFiUtils->getWiFiAPIP();
    const bool isStarted = ssid && ip && pwd;

    const TPos xp = X_OFFSET;
    TPos yp = this->screenStartY;

    // Get screen
    const TShowScreen& scr = this->screens[this->currentScreenIdx];
    const TScreenType type = scr.type;
    const String name = this->getScreenName();  // TODO: Get plan name?

    tft->setTextColor(TFT_MAGENTA);

    // Display screen name
    tft->setCursor(xp, yp, FONT_SM);
    this->clearLine();
    tft->println(name);
    yp += tft->fontHeight();

    // Display status/error
    tft->setTextColor(isStarted ? TFT_GREEN : TFT_BLUE);  // green/red!

    tft->setCursor(xp, yp, FONT_SM);
    this->clearLine();
    tft->println(isStarted ? "Started" : "Not started");
    yp += tft->fontHeight();

    // Display data
    tft->setTextColor(TFT_PINK);

    tft->setCursor(xp, yp, FONT_XS);
    this->clearLine();
    tft->printf("SSID: %s\n", ssid.c_str());
    yp += tft->fontHeight();

    tft->setCursor(xp, yp, FONT_XS);
    this->clearLine();
    tft->printf("Password: %s\n", pwd.c_str());
    yp += tft->fontHeight();

    tft->setCursor(xp, yp, FONT_XS);
    this->clearLine();
    tft->printf("IP: %s\n", ip.c_str());
    yp += tft->fontHeight();

    // Clean
    if (full) {
      this->clearRestLines();
    }
  }
#endif

#ifdef USE_POWER_STATION
  void drawPowerStationScreen(const bool full = false) {
    if (!tft) return;

    // Get object reference
    if (!this->powerStation || this->powerPlanIdx < 0) {
      return this->drawDefaultScreen(full);
    }
    PowerPlan* plan = this->powerStation->getPowerPlanByIdx(this->powerPlanIdx);
    if (!plan) {
      Serial.println(RED "Not found proper power plan for powerPlanIdx=" +
                     String(this->powerPlanIdx) + RESET);
      return;
    }

    const PowerPlanStatus status = plan->getStatus();

    /* status properties:
     *
     * - TPowerPlanId id
     * - TChannelNo channel
     * - unsigned int loopsCount
     * - bool initalState
     * + bool state
     * + TTick ticksDiff
     * - TTick ticks
     * - TTick ticksStarted
     * + size_t slotsCount
     * + size_t slot
     * + TTimeSec slotTimeSec
     * - String slotTextId
     */

    char strBuf[30];

    const TPos xp = X_OFFSET;
    TPos yp = this->screenStartY;

    // Get screen
    const TShowScreen& scr = this->screens[this->currentScreenIdx];
    const TScreenType type = scr.type;
    const String name = this->getScreenName();  // TODO: Get plan name?

    // Display screen name
    tft->setTextColor(TFT_MAGENTA);
    tft->setCursor(xp, yp, FONT_SM);
    // snprintf(strBuf, sizeof(strBuf), "Screen %d\n", (int)type + 1);
    this->clearLine();
    tft->println(name);
    yp += tft->fontHeight();

    String diffTimeStr = TimeUtils::getSecPeriodString(status.ticksDiff / 1000);
    String totalTimeStr = TimeUtils::getSecPeriodString(status.slotTimeSec);
    String timeStr = diffTimeStr + " / " + totalTimeStr;

    // Display state
    tft->setTextColor(TFT_ORANGE);
    tft->setCursor(xp, yp, FONT_SM);
    snprintf(strBuf, sizeof(strBuf), "State: %s\n",
             status.state ? "ON" : "OFF");
    this->clearLine();
    tft->print(strBuf);
    yp += tft->fontHeight();

    // Display other parameters
    tft->setTextColor(TFT_PINK);

    tft->setCursor(xp, yp, FONT_XS);
    snprintf(strBuf, sizeof(strBuf), "Slot: %d/%d (%s)\n", status.slot,
             status.slotsCount, status.slotTextId.c_str());
    this->clearLine();
    tft->print(strBuf);
    yp += tft->fontHeight();

    tft->setCursor(xp, yp, FONT_XS);
    snprintf(strBuf, sizeof(strBuf), "Slot time: %s\n", timeStr.c_str());
    this->clearLine();
    tft->print(strBuf);
    yp += tft->fontHeight();

    tft->setCursor(xp, yp, FONT_XS);
    snprintf(strBuf, sizeof(strBuf), "Plan loop: %d\n", status.loopsCount);
    this->clearLine();
    tft->print(strBuf);
    yp += tft->fontHeight();

    // Clean
    if (full) {
      this->clearRestLines();
    }
  }
#endif

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
    tft->print(strBuf);
    xp += tft->textWidth(strBuf) + 5;

    /* // Print tick
     * snprintf(strBuf, sizeof(strBuf), "tick %d", this->currentTick + 1);
     * tft->setCursor(xp, yp);
     * tft->print(strBuf);
     * xp += tft->textWidth(strBuf) + 5;
     */

    const TShowScreen& scr = this->screens[this->currentScreenIdx];

    const TTick timeDiff = this->ticks - this->ticksStarted;
    String diffTimeStr = TimeUtils::getSecPeriodString(timeDiff / 1000);
    TTick showTime = scr.showTime ? scr.showTime : this->defaultShowTime;
    String rightStr = this->paused
                          ? "> " + diffTimeStr
                          : diffTimeStr + " / " +
                                TimeUtils::getSecPeriodString(showTime / 1000);

    const TPos rightBoxWidth = tft->textWidth(rightStr) + X_OFFSET * 2;
    const TPos rxp = TFT_SCREEN_WIDTH - rightBoxWidth;
    // Serial.printf("[drawHeader] rightBoxWidth=%d\n", rightBoxWidth);
    tft->setCursor(rxp + X_OFFSET, yp);
    tft->fillRect(rxp, yp, rightBoxWidth, fontHeight, HEADER_BG_COLOR);
    tft->print(rightStr);

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

    // Update cursor (always show if paused)
    if (this->cursorState || this->paused) {
      tft->setCursor(xp + X_OFFSET, yp + FOOTER_Y_OFFSET);
      tft->setTextColor(FOOTER_FG_COLOR, FOOTER_BG_COLOR);
      tft->print(statusStr);
    }
    this->cursorState = !this->cursorState;

    // xp += tft->textWidth(statusStr);
    // yp += fontHeight;
  }

  void redrawScreen(const bool full = false) {
    const TShowScreen& scr = this->screens[this->currentScreenIdx];
    const TScreenType type = scr.type;

    // Detect current screen and invoke the dedicated drawing method
    switch (type) {
      // case DEMO:
#ifdef USE_TEMP_SENSOR
      case TEMP_SENSOR:
        return this->drawTempSensorScreen(full);
#endif
#ifdef USE_WIFI_UTILS
      case WIFI_NETWORK:
        return this->drawWiFiNetworkScreen(full);
      case WIFI_ACCESS_POINT:
        return this->drawAccessPointScreen(full);
#endif
#ifdef USE_POWER_STATION
      case POWER_CHANNEL:
        return this->drawPowerStationScreen(full);
#endif
    };
    return this->drawDefaultScreen(full);
  }

  void nextTick() {
    if (!tft) return;

    const TTick ticks = millis();
    this->ticks = ticks;
    const TShowScreen& scr = this->screens[this->currentScreenIdx];

    TTick showTime = scr.showTime ? scr.showTime : this->defaultShowTime;
    TTick updateTime =
        scr.updateTime ? scr.updateTime : this->defaultUpdateTime;
    TTick systemTime = this->defaultSystemTime;

    bool doNext =
        (!showTime || ticks - this->ticksStarted >= showTime) && !this->paused;
    const bool doUpdate =
        !updateTime || ticks - this->ticksUpdated >= updateTime;
    const bool doSystem =
        !systemTime || ticks - this->ticksSystem >= systemTime;

#ifdef USE_HW_BUTTON
    if (this->hwButton) {
      const TTick pressed = this->hwButton->getPressed();
      const size_t sequentialPressesCount =
          this->hwButton->getSequentialPresses();
      if (pressed && pressed != this->lastPressed) {
#ifdef InfoScreens_DEBUG
        Serial.printf("[InfoScreens] Pressed button sequential=%d\n",
                      sequentialPressesCount);
#endif
        if (sequentialPressesCount) {
          // Force screen change by double-clicking
          doNext = true;
          this->paused = false;
        } else {
          // ...Or just pause/resume automatic screen change waiting
          this->paused = !this->paused;
          this->ticksStarted = ticks;
        }
        this->lastPressed = pressed;
      }
    }
#endif

    this->currentTick++;

    if (doNext) {
      this->nextScreen();
      this->ticksStarted = ticks;
    }

    if (doNext || doUpdate) {
      // Show/update screens
      this->redrawScreen(doNext);
      this->ticksUpdated = ticks;
    }
    if (doSystem) {
      this->drawHeader();
      this->drawFooter();
      this->ticksSystem = ticks;
    }
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

#ifdef USE_POWER_STATION
  void setPowerStation(PowerStation* ref) { this->powerStation = ref; }
#endif
#ifdef USE_HW_BUTTON
  void setHwButton(HwButton* ref) { this->hwButton = ref; }
#endif
#ifdef USE_TEMP_SENSOR
  void setTempSensor(TempSensor* ref) { this->tempSensor = ref; }
#endif
#ifdef USE_WIFI_UTILS
  void setWiFiUtils(WiFiUtils* ref) { this->wiFiUtils = ref; }
#endif

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
#ifdef USE_POWER_STATION
    if (!this->powerStation)
      Serial.println(YELLOW "[InfoScreens]: PowerStation absent!" RESET);
#endif
#ifdef USE_HW_BUTTON
    if (!this->hwButton)
      Serial.println(YELLOW "[InfoScreens]: HwButton absent!" RESET);
#endif
#ifdef USE_TEMP_SENSOR
    if (!this->tempSensor)
      Serial.println(YELLOW "[InfoScreens]: TempSensor absent!" RESET);
#endif
#ifdef USE_WIFI_UTILS
    if (!this->wiFiUtils)
      Serial.println(YELLOW "[InfoScreens]: WiFiUtils absent!" RESET);
#endif
    // TODO: Check other required references

    // Add screens for each power plan...
#ifdef USE_POWER_STATION
    this->screens.reserve(basicScreensCount + POWER_CHANNELS_COUNT);
    for (size_t i = 0; i < POWER_CHANNELS_COUNT; i++) {
      this->screens.push_back({POWER_CHANNEL, DEFAULT_SHOW_SCREEN_PARAMS});
    }
#endif

#ifdef InfoScreens_DEBUG
    // DEBUG
    Serial.printf("Screens count=%d basicScreens=%d\n", this->screens.size(),
                  basicScreensCount);
    for (size_t i = 0; i < this->screens.size(); i++) {
      const TShowScreen& scr = this->screens[i];
      const TScreenType type = scr.type;
      const String name = getScreenTypeName(type);
      Serial.printf("%2d. Screen type %d: %s", i, type, name.c_str());
      // It's a power plan screen
      if (i >= basicScreensCount) {
        Serial.printf(" Power plan #%d", i - basicScreensCount);
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
