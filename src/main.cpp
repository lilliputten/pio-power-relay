// #define USE_WIFI_UTILS 1
// #define USE_WWW_SERVER 1
// #define USE_RELAY_DEMO 1

#include "DataFiles.hpp"
#include "Demos.hpp"
#include "PowerPlan.hpp"
#include "TFTUtils.hpp"

Demos demos;
TFTUtils tftUtils;

#ifdef USE_WIFI_UTILS
#include "WiFiUtils.hpp"
WiFiUtils wiFiUtils(&tftUtils);
#endif

#ifdef USE_WWW_SERVER
#include "WWWServer.hpp"
WWWServer wwwServer(&tftUtils);
#endif

PowerPlan powerPlan(1, RELAY_CH1, &tftUtils);

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

#ifdef USE_RELAY_DEMO
  demos.demoRelayInit();
#endif

  demos.demoTempSensorInit();
  // demos.demoDataFiles();

#ifdef USE_WWW_SERVER
  wwwServer.initServer();
#endif

  powerPlan.initPowerPlan();

  const char *done = "-- Setup done --";
  Serial.println(done);
  tft.print(done);
  // tft.println("                                             ");

  const uint16_t waitForStart = 10000;
  const uint16_t waitingDelay = 500;
  const char *dotStr = ".";
  for (uint16_t waiting; waiting < waitForStart; waiting += waitingDelay) {
    Serial.print(dotStr);
    tft.print(dotStr);
    delay(waitingDelay);
  }
  const char *okStr = " OK  ";
  Serial.println(okStr);
  tft.println(okStr);
  delay(waitingDelay * 2);

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
#ifdef USE_RELAY_DEMO
  yp += demos.demoRelayTick(tftUtils.tft, yp);
#endif

#ifndef USE_RELAY_DEMO
  powerPlan.nextTick();
  yp += powerPlan.showInfo(yp);
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
