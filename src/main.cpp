/* See module usage constants (defined in `platformio.ini`):
 *
 * - USE_TEMP_SENSOR - It's required for correct DataFiles initialzation for
 * some reason. (TODO: To investigate the issue?)
 * - USE_POWER_STATION - Power station & power plans
 * - USE_WIFI_UTILS - WiFi network connection and AP server
 * - USE_WWW_SERVER - Web server
 *
 * UNUSED: Demos (see `Demos.hpp`)
 *
 * - SHOW_TEMP_SENSOR_DEMO
 * - USE_DATA_FILES_DEMO
 * - USE_RELAY_DEMO
 * - SHOW_DEMO_LOOP
 */

#include "DataFiles.hpp"
#include "TFTUtils.hpp"
TFTUtils tftUtils;

#ifdef USE_HW_BUTTON
#include "HwButton.hpp"
HwButton hwButton(&tftUtils);
#endif

// NOTE: Required for the normal LittleFS initialization (WTF?)
#ifdef USE_TEMP_SENSOR
#include "TempSensor.hpp"
TempSensor tempSensor(&tftUtils);
#endif

#ifdef SHOW_DEMO_LOOP
#include "Demos.hpp"
Demos demos;
#endif

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

#include "InfoScreens.hpp"
InfoScreens infoScreens(&tftUtils);

// Iteration count
static unsigned long __count = 0;

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

#ifdef USE_HW_BUTTON
  hwButton.initHwButton();
#endif

#ifdef USE_TEMP_SENSOR
  tempSensor.initTempSensor();
#endif

  // NOTE: Beware of some components initialization sequence: it can stuck if
  // DataFiles has been initalized before tempSensor
  DataFiles::initDataFiles();
#ifdef USE_POWER_STATION
  powerStation.initPowerStation();
#endif

#ifdef SHOW_DEMO_LOOP
#ifdef USE_DATA_FILES_DEMO
  demos.demoDataFiles();
#endif
#ifdef USE_RELAY_DEMO
  demos.demoRelayInit();
#endif
#endif

#ifdef USE_WIFI_UTILS
  wiFiUtils.initWiFi();
#endif

  // WWWServer
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

  // InfoScreens
#ifdef USE_POWER_STATION
  infoScreens.setPowerStation(&powerStation);
#endif
#ifdef USE_HW_BUTTON
  infoScreens.setHwButton(&hwButton);
#endif
#ifdef USE_TEMP_SENSOR
  infoScreens.setTempSensor(&tempSensor);
#endif
#ifdef USE_WIFI_UTILS
  infoScreens.setWiFiUtils(&wiFiUtils);
#endif

  infoScreens.initScreens();

  // Done
  tftUtils.writeLine("Setup done");

  tftUtils.delayProgressBar(5000, 500, "OK");

  infoScreens.resetLoop();

  // vTaskDelete(NULL);  // DEBUG: Stop here

  tftUtils.resetScroll();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
}

void loop() {
#ifdef SHOW_DEMO_LOOP  // Loop mode: Demo
  TFT_eSPI &tft = tftUtils.getTFT();
  // tft.fillScreen(TFT_BLACK);

  Serial.println();
#if defined(TICK_DELAY) && (TICK_DELAY != 0)
  Serial.println("Loop: " + String(__count));
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

#else  // Loop mode: Real ticks

  // Serial.println("Real Loop: " + String(__count)); // DEBUG

#ifdef USE_HW_BUTTON
  hwButton.nextTick();
#endif

#ifdef USE_POWER_STATION
  powerStation.nextTick();
#endif

  infoScreens.nextTick();

#endif  // Loop mode

  // Next loop interation
  __count++;

  // Next loop or stop
#if defined(TICK_DELAY) && (TICK_DELAY != 0)
  // Do a delay otherwise
  delay(TICK_DELAY);
#else
  // TODO: Stop a watchdog timeout to run the code once
  Serial.println("\n-- FINISHED --");
  // while(1) yield();
  vTaskDelete(NULL);
#endif  // Next loop or stop
}
