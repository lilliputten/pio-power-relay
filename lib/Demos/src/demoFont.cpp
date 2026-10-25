#include "Demos.hpp"

#define BIG_WAIT 1000

void Demos::demoFont(TFT_eSPI &tft) {
  unsigned long targetTime = millis();

  // Fill the while screen
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.drawString("01:45678901234567890123456789", 0, 0 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("02:45678901234567890123456789", 0, 1 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("03:45678901234567890123456789", 0, 2 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("04:45678901234567890123456789", 0, 3 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("05:45678901234567890123456789", 0, 4 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("06:45678901234567890123456789", 0, 5 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("07:45678901234567890123456789", 0, 6 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("08:45678901234567890123456789", 0, 7 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("09:45678901234567890123456789", 0, 8 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("10:45678901234567890123456789", 0, 9 * FONT_XS_SIZE, FONT_XS);
  tft.drawString("11:45678901234567890123456789", 0, 10 * FONT_XS_SIZE,
                 FONT_XS);
  tft.drawString("12:45678901234567890123456789", 0, 11 * FONT_XS_SIZE,
                 FONT_XS);
  tft.drawString("13:45678901234567890123456789", 0, 12 * FONT_XS_SIZE,
                 FONT_XS);

  delay(WAIT);

  // First we test them with a background colour set
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
  tft.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 12, 2);
  tft.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
  tft.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
  int xpos = 0;
  xpos += tft.drawString("xyz{|}~", 0, 64, 2);
  tft.drawChar(127, xpos, 64, 2);  // Degree character
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
  tft.drawString("/0123456789:;", 0, 26, 4);
  tft.drawString("<=>?@ABCDE", 0, 52, 4);
  tft.drawString("FGHIJKLMNO", 0, 78, 4);
  tft.drawString("PQRSTUVWX", 0, 104, 4);
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.drawString("YZ[\\]^_`abc", 0, 0, 4);
  tft.drawString("defghijklmno", 0, 26, 4);
  tft.drawString("pqrstuvwxyz", 0, 52, 4);
  xpos = 0;
  xpos += tft.drawString("{|}~", 0, 78, 4);
  tft.drawChar(127, xpos, 78, 4);
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);

  tft.drawString("012345", 0, 0, 6);
  tft.drawString("6789", 0, 40, 6);
  tft.drawString("apm-:.", 0, 80, 6);
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);

  tft.drawString("0123", 0, 0, 7);
  tft.drawString("4567", 0, 60, 7);
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.drawString("890:.", 0, 0, 7);
  tft.drawString("", 0, 60, 7);
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  tft.drawString("01", 0, 0, 8);
  delay(WAIT);

  tft.drawString("23", 0, 0, 8);
  delay(WAIT);

  tft.drawString("45", 0, 0, 8);
  delay(WAIT);

  tft.drawString("67", 0, 0, 8);
  delay(WAIT);

  tft.drawString("89", 0, 0, 8);
  delay(WAIT);

  tft.drawString("0:.", 0, 0, 8);
  delay(WAIT);

  tft.setTextColor(TFT_MAGENTA);
  tft.drawNumber(millis() - targetTime, 0, 100, 4);
  delay(BIG_WAIT);

  // Now test them with transparent background
  targetTime = millis();

  tft.setTextSize(1);
  tft.fillScreen(TFT_BROWN);
  tft.setTextColor(TFT_GREEN);

  tft.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
  tft.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
  tft.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
  tft.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
  xpos = 0;
  xpos += tft.drawString("xyz{|}~", 0, 64, 2);
  tft.drawChar(127, xpos, 64, 2);
  delay(WAIT);

  tft.fillScreen(TFT_BROWN);
  tft.setTextColor(TFT_GREEN);

  tft.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
  tft.drawString("/0123456789:;", 0, 26, 4);
  tft.drawString("<=>?@ABCDE", 0, 52, 4);
  tft.drawString("FGHIJKLMNO", 0, 78, 4);
  tft.drawString("PQRSTUVWX", 0, 104, 4);

  delay(WAIT);
  tft.fillScreen(TFT_BROWN);
  tft.drawString("YZ[\\]^_`abc", 0, 0, 4);
  tft.drawString("defghijklmno", 0, 26, 4);
  tft.drawString("pqrstuvwxyz", 0, 52, 4);
  xpos = 0;
  xpos += tft.drawString("{|}~", 0, 78, 4);
  tft.drawChar(127, xpos, 78, 4);
  delay(WAIT);

  tft.fillScreen(TFT_BROWN);
  tft.setTextColor(TFT_BLUE);

  tft.drawString("012345", 0, 0, 6);
  tft.drawString("6789", 0, 40, 6);
  tft.drawString("apm-:.", 0, 80, 6);
  delay(WAIT);

  tft.fillScreen(TFT_BROWN);
  tft.setTextColor(TFT_RED);

  tft.drawString("0123", 0, 0, 7);
  tft.drawString("4567", 0, 60, 7);
  delay(WAIT);

  tft.fillScreen(TFT_BROWN);
  tft.drawString("890:.", 0, 0, 7);
  tft.drawString("", 0, 60, 7);
  delay(WAIT);

  tft.fillScreen(TFT_BROWN);
  tft.setTextColor(TFT_YELLOW);

  tft.drawString("0123", 0, 0, 8);
  delay(WAIT);

  tft.fillScreen(TFT_BROWN);
  tft.drawString("4567", 0, 0, 8);
  delay(WAIT);

  tft.fillScreen(TFT_BROWN);
  tft.drawString("890:.", 0, 0, 8);
  delay(WAIT);

  tft.setTextColor(TFT_MAGENTA);

  tft.drawNumber(millis() - targetTime, 0, 100, 4);
  delay(BIG_WAIT);
}
