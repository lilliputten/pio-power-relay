#include "TFTUtils.hpp"

void TFTUtils::initTFT() {
  tft.init();
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(TFT_BLACK);
  tft.setTextPadding(10);
}
