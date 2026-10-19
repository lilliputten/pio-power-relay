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
#include "Demos.hpp"
#include "TFTUtils.hpp"

Demos demos;
TFTUtils tftUtils;

#ifdef USE_POWER_STATION
#include "PowerStation.hpp"
PowerStation powerStation(&tftUtils);
#endif

#ifdef USE_TEMP_SENSOR
#include "TempSensor.hpp"
TempSensor tempSensor(&tftUtils);
#endif

#ifdef USE_WIFI_UTILS
#include "WiFiUtils.hpp"
WiFiUtils wiFiUtils(&tftUtils);
#endif

#ifdef USE_WWW_SERVER
#include "WWWServer.hpp"
WWWServer wwwServer(&tftUtils);
#endif

// Iteration count
static unsigned long _count = 0;

void setup() {
  Serial.begin(115200);

  TFT_eSPI &tft = tftUtils.getTFT();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(0);
  tft.setTextPadding(10);

  delay(100);

  char projectInfoBuf[80];
  snprintf(projectInfoBuf, sizeof(projectInfoBuf), "%s @ %s\n", PROJECT_NAME,
           PROJECT_INFO);
  Serial.println();
  Serial.print("Project: ");
  Serial.println(projectInfoBuf);
  tft.println(projectInfoBuf);

  Serial.printf("TICK_DELAY: %d\n", TICK_DELAY);

  DataFiles::initDataFiles();

#ifdef USE_RELAY_DEMO
  demos.demoRelayInit();
#endif

#ifdef USE_TEMP_SENSOR
  tempSensor.initTempSensor();
#endif

#ifdef USE_DATA_FILES_DEMO
  demos.demoDataFiles();
#endif

#ifdef USE_POWER_STATION
  powerStation.initPowerStation();
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

  const char *done = "-- Setup done --";
  Serial.println(done);
  tft.print(done);
  // tft.println("                                             ");

  // vTaskDelete(NULL);  // DEBUG: Stop here

  const uint16_t waitForStart = 10000;
  const uint16_t waitingDelay = 500;
  const char *dotStr = ".";
  for (uint16_t waiting; waiting < waitForStart; waiting += waitingDelay) {
    Serial.print(dotStr);
    tft.print(dotStr);
    delay(waitingDelay);
  }
  const char *okStr = " OK ";
  Serial.println(okStr);
  tft.println(okStr);
  delay(waitingDelay * 2);

  tft.fillScreen(TFT_BLACK);
  tft.setTextPadding(10);
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

#ifndef USE_RELAY_DEMO
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
