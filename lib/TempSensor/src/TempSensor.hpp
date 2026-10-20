#ifndef TempSensor_DEFINED
#define TempSensor_DEFINED

// #define TempSensor_DEBUG

#include <DallasTemperature.h>
#include <OneWire.h>

#include "TFTUtils.hpp"

// Setup a oneWire instance to communicate with any OneWire devices
static OneWire oneWire(TEMP_SENSOR_CH);

class TempSensor {
 private:
  TFTUtils* tftUtils = nullptr;

  // Dallas Temperature sensor
  DallasTemperature sensors;

 public:
  const float getTemperature() {
    sensors.requestTemperatures();
    const float temperatureC = sensors.getTempCByIndex(0);
    return temperatureC;
  }

  // Display current status on a tft screen
  int showInfo(int yp) {
    auto tft = this->tftUtils ? &tftUtils->tft : nullptr;
    if (!tft) return 0;

    int xp = 10;
    // int font1 = FONT_SM;
    // int fh = tft->fontHeight(font1);
    int font2 = FONT_MD;
    int fh2 = tft->fontHeight(font2);

    const float temperatureC = this->getTemperature();

    // Note different degree symbols for console and tft fonts
    Serial.printf("%.1fºC\n", temperatureC);

    tft->setCursor(xp, yp, font2);
    tft->setTextColor(TFT_MAGENTA, TFT_BLACK);

    tft->printf("%.1f`C  \n", temperatureC);

    return fh2;
  }

  void initTempSensor() {
    if (tftUtils) tftUtils->writeLine("Starting Temp Sensor...");

    // Start the DS18B20 sensor
    this->sensors.begin();
  }

  // Constructor
  TempSensor(TFTUtils* tftUtils = nullptr) : sensors(&oneWire) {
    if (tftUtils) {
      this->tftUtils = tftUtils;
    }
  }
};

#endif  // TempSensor_DEFINED
