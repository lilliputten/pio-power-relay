#include "Demos.hpp"

// Iteration count
static long _count = 0;

void Demos::demoRelayInit() {
  // TODO
}

int Demos::demoRelayTick(TFT_eSPI &tft, int yp) {
  int xp = 10;
  int font1 = FONT_SM;
  int fh = tft.fontHeight(font1);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(xp, yp, font1);

  _count++;

  Serial.print("Relay: ");
  Serial.println(_count);

  tft.print("Relay: ");
  tft.print(_count);
  // Place extra spaces to cleanup the previous output text
  tft.println("   ");

  return fh;
}
