/*
**  Author: Aniket Biswas
**  Github: https://github.com/xunicatt
**
**  Patched by igor@lilliputten at 2026.10.20
**
**  Library to add Vertical text Scrolling functionality to TFT_eSPI library by
*bodmer.
**  Header File: TFTScroll.h
**  TFT_eSPI: https://github.com/Bodmer/TFT_eSPI
*/

#include "TFTScroll.hpp"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "1bit.h"

// #define TFTScroll_DEBUG

const String TFTScroll::spaces = "                              ";
//                                123456789012345678901234567890
// It should be longer than maximin line length (it's 26 for FONT_SM and
// FONT_XS)

// Constructor
TFTScroll::TFTScroll() { this->cursor = 0; }

// Destructor
TFTScroll::~TFTScroll() {
  buffer.clear();
  sprite->deleteSprite();
  delete sprite;
}

/*
** @name init
** @brief Initializes the class with neccessary datas.
** @param TFT_eSPI*
** @param text_font
** @param bg_color
** @param fg_color
*/
TFTScroll_Error TFTScroll::init(TFT_eSPI *tft_espi, uint16_t text_font,
                                uint8_t color_depth) {
  if (tft_espi == NULL) return ERROR_NO_TFT;

  tft = tft_espi;
  sprite = new TFT_eSprite(tft);
  sprite->setColorDepth(color_depth);
  this->width = tft->width();
  this->height = tft->height();

  if (!sprite->createSprite(width, height)) {
    tft = NULL;
    return ERROR_SPRITE_MEMORY_ALLOCATION_FAILED;
  }

  sprite->setTextFont(text_font);
  this->font_height = sprite->fontHeight(text_font);
  sprite->fillScreen(bg_color = COLOR_BLACK_1B);
  sprite->setTextColor(COLOR_WHITE_1B);

  return NO_ERROR;
}

int TFTScroll::getCursorLine() { return cursor / font_height; }

/*
** @name write
** @brief Writes a line to the screen while scrolling.
** @param text
*/
TFTScroll_Error TFTScroll::write(const String text) {
  // if init is not called then return
  if (!tft) return ERROR_NO_TFT;
  // computes the text-width and text-height
  int text_height = font_height, text_width = sprite->textWidth(text);
  if (text_width > width)
    text_height = ceil(text_width / (float)width) * font_height;

  // if the text cannot be fitted in the screen then we shift the texts in
  // screen to make room
  if (cursor + text_height >= height) {
    shift(text_height);
  }

  String padded = text;
  // Pad with spaces if shorter than max
  const size_t lineWidth = getLineWidthInChars();
  if (padded.length() < lineWidth) {
    const String emptyStr =
        TFTScroll::spaces.substring(0, lineWidth - padded.length());
    padded += emptyStr;
  }

  // after shifting, previous string is filled with a rect
  // then text gets printed to the screen
  sprite->fillRect(0, cursor, width, text_height, bg_color);
  sprite->setCursor(0, cursor);
  sprite->print(padded);
  sprite->pushSprite(0, 0);

  // stroing current string in buffer vector
  buffer.push_back(padded);

  // preparing the cursor for the next line
  cursor += text_height;

#ifdef TFTScroll_DEBUG
  Serial.println("-- TFTScroll_DEBUG");
  Serial.printf("cursor: %d\n", cursor);
  Serial.printf("cursorLine: %d\n", getCursorLine());
  Serial.printf("font_height: %d\n", font_height);
  Serial.printf("text_height: %d\n", text_height);
  Serial.printf("text_width: %d\n", text_width);
  Serial.printf("height: %d\n", height);
  Serial.printf("width: %d\n", width);
#endif

  return NO_ERROR;
}

/*
** @name shift
** @brief Shift the whole screen by -1.
** @param text_height
*/
void TFTScroll::shift(int text_height) {
  int line_width, line_height;

  // making the room for the new string at the bottom
  // by removing texts from the top of the screen
  for (String &line : buffer) {
    if (cursor + text_height < height) break;

    line_height = (ceil(sprite->textWidth(line) / (float)width) * font_height);
    buffer.erase(buffer.begin());

    cursor -= line_height;
  }

  int extra_cursor = 0;

  // reprinting the whole text buffer after covering the previous texts
  // with rect
  for (String &line : buffer) {
    line_height = font_height;
    line_width = sprite->textWidth(line);
    if (line_width > width)
      line_height = (ceil(line_width / (float)width) * font_height);

    sprite->fillRect(0, extra_cursor, (line_width < width) ? line_width : width,
                     line_height, bg_color);
    sprite->setCursor(0, extra_cursor);
    sprite->print(line);

    extra_cursor += line_height;
  }
}

/*
** @name setColor
** @brief Sets an background and foreground color.
** @brief Color depth must be set accordingly.
** @param background_color
** @param foreground_color
*/
void TFTScroll::setColor(uint16_t background_color, uint16_t forground_color) {
  sprite->fillScreen(bg_color = background_color);
  sprite->setTextColor(forground_color);
}

/*
** @name reset
** @brief Resets scroll
*/
void TFTScroll::reset() {
  buffer.clear();
  sprite->deleteSprite();
  delete sprite;
  cursor = 0;
}
