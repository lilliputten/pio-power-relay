// TFT Screen
#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library, see configuration in `User_Setup.h`

#include "TFTUtils.hpp"

// Temp sensor
#include <DallasTemperature.h>
#include <OneWire.h>

#define TFT_GREY 0x5AEB  // New color
// #define TFT_BROWN 0x38E0

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 1000

class Demos {
 public:
  void demoPrint(TFT_eSPI &tft);
  void demoFont(TFT_eSPI &tft);
  void demoEnv();
  // TempSensor
  void demoTempSensorInit();
  int demoTempSensorShow(TFT_eSPI &tft, int yp);
  // Relay
  void demoRelayInit();
  int demoRelayTick(TFT_eSPI &tft, int yp);
  // SD Card
  void demoSDCardInit();
  int demoSDCardTick(TFT_eSPI &tft, int yp);
};
