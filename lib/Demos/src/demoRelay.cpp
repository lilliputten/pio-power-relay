#include "Demos.hpp"

// Iteration count
static long _count = 0;

void Demos::demoRelayInit() {
  Serial.println("Relay init");
  // Initialize pins
  pinMode(RELAY_CH1, OUTPUT);
  // Set initial state to OFF
  digitalWrite(RELAY_CH1, LOW);
}

int Demos::demoRelayTick(TFT_eSPI& tft, int yp) {
  int status = (_count % 2);
  int statusSignal = status ? HIGH : LOW;

  digitalWrite(RELAY_CH1, statusSignal);

  int xp = 10;
  int font1 = FONT_SM;
  int fh = tft.fontHeight(font1);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(xp, yp, font1);

  char charBuf[50];
  snprintf(charBuf, sizeof(charBuf), "Relay: %d - %s", _count,
           status ? "ON" : "OFF");

  Serial.println(charBuf);

  tft.print(charBuf);
  // Place extra spaces to cleanup the previous output text
  tft.println("  ");

  // Next loop interation
  _count++;

  // Return line height
  return fh;
}
