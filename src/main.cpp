#include "Demos.hpp"
#include "TFTUtils.hpp"
// #include "WiFiUtils.hpp"

TFTUtils tftUtils;
Demos demos;
// WiFiUtils wiFiUtils;

// Iteration count
static long count = 0;

void setup() {
  Serial.begin(115200);

  delay(100);

  tftUtils.initTFT();

  // wiFiUtils.initWiFi();
  // wiFiUtils.scanWifi();

  demos.demoTempSensorInit();
  demos.demoRelayInit();

  Serial.println("Setup done");
}

void loop() {
  count++;
  Serial.println("Loop count: " + String(count));

  int yp = 10;
  int hTempSensor = demos.demoTempSensorShow(tftUtils.tft, yp);
  int hRelay = demos.demoRelayTick(tftUtils.tft, yp + hTempSensor);

  // // demos.demoPrint(tftUtils.tft);
  // demos.demoFont(tftUtils.tft);

  // while(1) yield(); // We must yield() to stop a watchdog timeout.
  delay(5000);
}
