// #define USE_WIFI_UTILS 1

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

  tftUtils.initTFT();

#ifdef WIFI_CONFIGS
  // WiFi Configs iteration demo (array expected)
  Serial.println("WiFi configs:");
  const char* lines[] = WIFI_CONFIGS;
  const int lineCount = sizeof(lines) / sizeof(lines[0]);
  for (int i = 0; i < lineCount; i++) {
    Serial.println(lines[i]);
  }
#endif

#ifdef USE_WIFI_UTILS
  wiFiUtils.initWiFi();
  wiFiUtils.scanWifi();
#endif

  demos.demoTempSensorInit();
  demos.demoRelayInit();
  demos.demoSDCardInit();

  Serial.println("Setup done");
}

void loop() {
  Serial.println("\nLoop: " + String(_count));

  int yp = 10;
  yp += demos.demoTempSensorShow(tftUtils.tft, yp);
  yp += demos.demoRelayTick(tftUtils.tft, yp);
  // yp += demos.demoSDCardTick(tftUtils.tft, yp);

  // // demos.demoPrint(tftUtils.tft);
  // demos.demoFont(tftUtils.tft);

  // Next loop interation
  _count++;

#if !defined(TICK_DELAY) || (TICK_DELAY == 0)
  // TODO: Stop a watchdog timeout to run the code once
  // while(1) yield();
  vTaskDelete(NULL);
#else
  // Do a delay otherwise
  delay(TICK_DELAY);
#endif
}
