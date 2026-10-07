#define USE_WIFI_UTILS 1
// #define USE_RELAY 1

#include "DataFiles.hpp"
#include "Demos.hpp"
#include "TFTUtils.hpp"
#include "WWWServer.hpp"

Demos demos;
TFTUtils tftUtils;
WWWServer wwwServer(&tftUtils);

#ifdef USE_WIFI_UTILS
#include "WiFiUtils.hpp"
WiFiUtils wiFiUtils(&tftUtils);
#endif

// Iteration count
static long _count = 0;

void setup() {
  Serial.begin(115200);

  TFT_eSPI tft = tftUtils.getTFT();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(0);
  tft.setTextPadding(10);

  delay(100);
  Serial.printf("\nProject: %s @ %s\n", PROJECT_NAME, PROJECT_INFO);

  Serial.printf("TICK_DELAY: %d\n", TICK_DELAY);

  DataFiles::initDataFiles();
  tftUtils.initTFT();

#ifdef USE_WIFI_UTILS
  wiFiUtils.initWiFi();
#endif

#ifdef USE_RELAY
  demos.demoRelayInit();
#endif

  demos.demoTempSensorInit();
  // demos.demoDataFiles();

  wwwServer.initServer();

  const char *done = "-- Setup done --";
  Serial.println(done);
  tft.print(done);
  tft.println("                                             ");

  delay(30000);

  tft.fillScreen(TFT_BLACK);
  tft.setTextPadding(10);
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
