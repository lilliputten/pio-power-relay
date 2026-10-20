#define USE_WIFI_UTILS
#define USE_WWW_SERVER
#define USE_POWER_STATION
#define USE_TEMP_SENSOR
#define SHOW_TEMP_SENSOR_DEMO

// #define USE_DATA_FILES_DEMO
// #define USE_RELAY_DEMO

#ifndef POWER_CHANNELS_COUNT
#error "Constant POWER_CHANNELS_COUNT must be defined!"
#endif

#include "DataFiles.hpp"
#include "TFTUtils.hpp"
TFTUtils tftUtils;

// NOTE: Required for the normal LittleFS initialization (WTF?)
#ifdef USE_TEMP_SENSOR
#include "TempSensor.hpp"
TempSensor tempSensor(&tftUtils);
#endif

#include "Demos.hpp"

Demos demos;

#ifdef USE_POWER_STATION
#include "PowerStation.hpp"
PowerStation powerStation(&tftUtils);
#endif

#ifdef USE_WIFI_UTILS
#include "WiFiUtils.hpp"
WiFiUtils wiFiUtils(&tftUtils);
#endif

#ifdef USE_WWW_SERVER
#include "WWWServer.hpp"
WWWServer wwwServer(&tftUtils);
#endif

// #include <TFT_eSPI_Scroll.h>
// #include <colors/1bit.h>

// Iteration count
static unsigned long _count = 0;

void setup() {
  Serial.begin(115200);

  TFT_eSPI &tft = tftUtils.getTFT();
  // TFTScroll &scroll = tftUtils.getScroll();

  tftUtils.initScroll();

  Serial.println();
  const String projectInfoStr = PROJECT_NAME " @ " PROJECT_INFO;
  tftUtils.writeLine(projectInfoStr);

  // tftUtils.writeLine(
  //     "123456789012345678901234567890");  // 26 chars of 30 are visible

  tftUtils.writeLine("TICK_DELAY: " + String(TICK_DELAY));
  tftUtils.writeLine("Screen: " + String(tftUtils.getLineWidthInChars()) + "x" +
                     String(tftUtils.getLinesCount()));

  /* // DEBUG: Test scrolling
   * delay(TICK_DELAY);
   * for (int i = 0; i < 15; i++) {
   *   long rand = random(1, 1100);
   *   const String str = "Count: " + String(i) + " - " + String(rand);
   *   tftUtils.writeLine(str);
   *   delay(100);
   * }
   * tftUtils.delayProgressBar(5000, 500, "Started");
   * tftUtils.writeLine("Finished");
   * delay(TICK_DELAY);
   */

  // vTaskDelete(NULL);  // DEBUG: Stop here

#ifdef USE_TEMP_SENSOR
  tempSensor.initTempSensor();
#endif

  // NOTE: Beware of some components initialization sequence: it can stuck if
  // DataFiles has been initalized before tempSensor
  DataFiles::initDataFiles();
#ifdef USE_DATA_FILES_DEMO
  demos.demoDataFiles();
#endif

#ifdef USE_POWER_STATION
  powerStation.initPowerStation();
#endif
#ifdef USE_RELAY_DEMO
  demos.demoRelayInit();
#endif

#ifdef USE_WIFI_UTILS
  wiFiUtils.initWiFi();
#endif

#ifdef USE_WWW_SERVER
#ifdef USE_POWER_STATION
  wwwServer.setPowerStation(&powerStation);
#endif
#ifdef USE_TEMP_SENSOR
  wwwServer.setTempSensor(&tempSensor);
#endif
  wwwServer.initServer();
  wwwServer.startServer();
#endif

  tftUtils.writeLine("Setup done");

  tftUtils.delayProgressBar(5000, 500, "OK");

  // vTaskDelete(NULL);  // DEBUG: Stop here

  tftUtils.resetScroll();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
}

void loop() {
  TFT_eSPI &tft = tftUtils.getTFT();
  tft.fillScreen(TFT_BLACK);

  Serial.println();
#if defined(TICK_DELAY) && (TICK_DELAY != 0)
  Serial.println("Loop: " + String(_count));
#endif

  int yp = 10;

#ifdef SHOW_TEMP_SENSOR_DEMO
#ifndef USE_TEMP_SENSOR
#error "USE_TEMP_SENSOR must be defined to use SHOW_TEMP_SENSOR_DEMO"
#endif
  yp += tempSensor.showInfo(yp);
#endif

#ifdef USE_RELAY_DEMO
  yp += demos.demoRelayTick(tftUtils.tft, yp);
#endif

#ifdef USE_POWER_STATION
  powerStation.nextTick();
  yp += powerStation.showInfo(yp);
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
