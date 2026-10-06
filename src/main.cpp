#define USE_WIFI_UTILS 1
// #define USE_RELAY 1

#include "DataFiles.hpp"
#include "Demos.hpp"
#include "TFTUtils.hpp"

Demos demos;
TFTUtils tftUtils;

#ifdef USE_WIFI_UTILS
#include "WiFiUtils.hpp"
WiFiUtils wiFiUtils;
#endif

// Iteration count
static long _count = 0;

void setup() {
  Serial.begin(115200);

  delay(100);
  Serial.printf("\nProject: %s @ %s\n", PROJECT_NAME, PROJECT_INFO);

  Serial.printf("TICK_DELAY: %d\n", TICK_DELAY);

  DataFiles::initDataFiles();
  tftUtils.initTFT();

  // TFT_eSPI *tftRef = &tftUtils.tft;

#ifdef USE_WIFI_UTILS
  wiFiUtils.initWiFi(&tftUtils.tft);
#endif

#ifdef USE_RELAY
  demos.demoRelayInit();
#endif

  demos.demoTempSensorInit();
  // demos.demoDataFiles();

  tftUtils.tft.fillScreen(TFT_BLACK);

  Serial.println("-- Setup done --");
}

void loop() {
  Serial.println();
#if defined(TICK_DELAY) && (TICK_DELAY != 0)
  Serial.println("Loop: " + String(_count));
#endif

  int yp = 10;
  yp += demos.demoTempSensorShow(tftUtils.tft, yp);
#ifdef USE_RELAY
  yp += demos.demoRelayTick(tftUtils.tft, yp);
#endif

  // demos.demoPrint(tftUtils.tft);
  // demos.demoFont(tftUtils.tft);

  // Next loop interation
  _count++;

#if defined(TICK_DELAY) && (TICK_DELAY != 0)
  // Do a delay otherwise
  delay(TICK_DELAY);
#else
  // TODO: Stop a watchdog timeout to run the code once
  Serial.println("\n-- FINISHED --");
  // while(1) yield();
  vTaskDelete(NULL);
#endif
}
