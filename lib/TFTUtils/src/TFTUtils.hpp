#ifndef TFTUtils_DEFINED
#define TFTUtils_DEFINED

#define USE_TFT_SCROLL
// NOTE: It requires a proper order of includes

// #define TFTUtils_DEBUG

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library, see configuration in `User_Setup.h`

#ifdef USE_TFT_SCROLL
// #include <TFT_eSPI_Scroll.h>
#include "TFTScroll.hpp"
#endif

#include "constants.hpp"

#define FONT_XS \
  1  // 26x10-16 (rows count depends on line height, see below, in range 12-16)
#define FONT_XS_SIZE 11
#define FONT_SM 2  // 20x8
#define FONT_SM_SIZE 16
#define FONT_MD 4  // 11x5
#define FONT_MD_SIZE 26
#define FONT_LG 6        // 6x3 3 lines per screen
#define FONT_LG_SIZE 40  // 3 lines per screen
#define FONT_LCD 7       // 5x2
#define FONT_LCD_SIZE 60
#define FONT_XL 8  // 3x1
#define FONT_XL_SIZE 80

static const String spaces = "..............................";

class TFTUtils {
 private:
#ifdef USE_TFT_SCROLL
  bool scrollInited = false;
  // unsigned int scrollCount = 0;
  TFTScroll scroll = TFTScroll();
#endif

 public:
  TFT_eSPI tft = TFT_eSPI();

  TFT_eSPI& getTFT() { return this->tft; }
  // TFTScroll& getScroll() { return this->scroll; }

  bool initScroll() {
#ifdef USE_TFT_SCROLL
    const TFTScroll_Error error = scroll.init(&tft, 1, 1);
    if (error != NO_ERROR) {
      Serial.println(RED "[TFTUtils] Scroll failed: " + String(error) + RESET);
      return false;
    };
    // scrollCount = 0;
    scrollInited = true;
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, FONT_XS);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // scroll.setColor(TFT_BLACK, TFT_WHITE);
    return true;
#else
    return false;
#endif
  }

  void resetScroll() {
#ifdef USE_TFT_SCROLL
    scroll.reset();
    // scrollCount = 0;
    scrollInited = false;
#endif
  }

  void delayProgressBar(const uint16_t waitTime = 10000,
                        const uint16_t delayStep = 500,
                        const String okStr = " OK ",
                        const String startStr = "") {
#ifdef USE_TFT_SCROLL
    tft.setCursor(0, scroll.cursor);
#endif

    if (startStr) {
      Serial.print(startStr);
      tft.print(startStr);
    }

    const char* dotStr = ".";
    for (uint16_t waiting; waiting < waitTime; waiting += delayStep) {
      Serial.print(dotStr);
      tft.print(dotStr);
      delay(delayStep);
    }
    const char* okCStr = okStr.c_str();
    Serial.println(okCStr);
#ifdef USE_TFT_SCROLL
    scroll.write(okStr);
#else
    tft.println(okCStr);
#endif
    delay(delayStep * 2);
  }

  size_t getLinesCount() {
    const size_t fontHeight = this->getFontHeight();
    return TFT_SCREEN_HEIGHT / fontHeight;
  }

  size_t getFontWidth() {
    // Considering a monospace font
    return this->tft.textWidth("A");
  }
  size_t getFontHeight() { return this->tft.fontHeight(); }
  size_t getLineWidthInChars() {
    // Considering a monospace font
    const size_t fontWidth = getFontWidth();
    return TFT_SCREEN_WIDTH / fontWidth;
  }

  // size_t getScrollY() {
  //   const size_t linesCount = this->getLinesCount();
  //   if (this->scrollCount >= linesCount) {
  //     return linesCount - 1;
  //   }
  //   return this->scrollCount;
  // }

  void writeLine(const String str = "", const bool onlyTTF = false) {
    if (!onlyTTF) {
      Serial.println(str);
    }
#ifdef USE_TFT_SCROLL
    if (!this->scrollInited) {
      if (!this->initScroll()) {
        tft.println(str);
        return;
      }
    }
    // const size_t strLen = str.length();
    // const size_t strWidthPx = tft.textWidth(str);
    // const size_t scrWidthPx = TFT_IS_ROTATED ? TFT_HEIGHT : TFT_WIDTH;
    // const size_t extraLines = strWidthPx / scrWidthPx;
    // const size_t lastLinePx = strWidthPx % scrWidthPx;
    // const size_t fontWidth = getFontWidth();
    // const size_t fontHeight = this->getFontHeight();
    // const size_t lastLineChars = lastLinePx / fontWidth;
    // const size_t fullLines = extraLines + 1;
    this->scroll.write(str);
    // this->scrollCount += fullLines;
    // return fullLines;
#else
    tft.println(str);
#endif
  }

  TFTUtils() {
    // Constructor
    // Serial.println("TFTUtils constructor");
    this->initTFT();
  }

 private:
  void initTFT() {
    this->tft.init();
    this->tft.setRotation(TFT_ROTATION);
    this->tft.fillScreen(TFT_BLACK);
    // this->tft.setTextPadding(10);
  }
};

#endif
