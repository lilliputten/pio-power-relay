#include "Demos.hpp"

void Demos::demoEnv() {
  // Test project parameters...
  Serial.println("TFT_WIDTH: " + String(TFT_WIDTH));
  Serial.println("TFT_HEIGHT: " + String(TFT_HEIGHT));

  Serial.println("TFT_DC: " + String(TFT_DC));
  Serial.println("TFT_RST: " + String(TFT_RST));
  Serial.println("TFT_CS: " + String(TFT_CS));
}
