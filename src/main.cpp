#include "Demos.hpp"
#include "TFTUtils.hpp"
// #include "WiFiUtils.hpp"

TFTUtils tftUtils;
Demos demos;
// WiFiUtils wiFiUtils;

// Iteration count
static long _count = 0;

void setup() {
  Serial.begin(115200);

  delay(100);
  Serial.print("\nProject: ");
  Serial.println(PROJECT_INFO);

  tftUtils.initTFT();

  // wiFiUtils.initWiFi();
  // wiFiUtils.scanWifi();

  demos.demoTempSensorInit();
  demos.demoRelayInit();

  Serial.println("Setup done");
}

void loop() {
  Serial.println("\nLoop: " + String(_count));

  int yp = 10;
  int hTempSensor = demos.demoTempSensorShow(tftUtils.tft, yp);
  int hRelay = demos.demoRelayTick(tftUtils.tft, yp + hTempSensor);

  // // demos.demoPrint(tftUtils.tft);
  // demos.demoFont(tftUtils.tft);

  // Next loop interation
  _count++;

  // while(1) yield(); // TODO: Stop a watchdog timeout to run the code once
  delay(TICK_DELAY);
}
