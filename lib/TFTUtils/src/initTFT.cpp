#include "TFTUtils.hpp"

void TFTUtils::initTFT() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextPadding(10);
}
