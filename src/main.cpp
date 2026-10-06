#define USE_WIFI_UTILS 1
// #define USE_RELAY 1

#include "DataFiles.hpp"
#include "Demos.hpp"
#include "TFTUtils.hpp"

Demos demos;
TFTUtils tftUtils;

#ifdef USE_WIFI_UTILS
// #ifndef WIFI_CONFIGS
//   #error "Build halted: WIFI_CONFIGS is missing!"
// #endif
#include "WiFiUtils.hpp"
WiFiUtils wiFiUtils;
#endif

// Iteration count
static long _count = 0;

void setup() {
  Serial.begin(115200);

  delay(100);
  Serial.printf("\nProject: %s @ %s\n", PROJECT_NAME, PROJECT_INFO);

  tftUtils.initTFT();

  Serial.printf("TICK_DELAY: %d\n", TICK_DELAY);

#ifdef WIFI_CONFIGS
  // WiFi Configs iteration demo (array expected)
  Serial.println("WiFi configs:");
  const TStrings lines = WIFI_CONFIGS;
  const int lineCount = lines.size();  // sizeof(lines) / sizeof(lines[0]);
  for (int i = 0; i < lineCount; i++) {
    Serial.println(lines[i]);
  }
#endif

  DataFiles::initDataFiles();

#ifdef USE_WIFI_UTILS
  wiFiUtils.initWiFi();
  // wiFiUtils.scanWifi();
#endif

  demos.demoTempSensorInit();
  // demos.demoDataFiles();
#ifdef USE_RELAY
  demos.demoRelayInit();
#endif

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
