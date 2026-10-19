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
  TFT_eSPI* tft = nullptr;

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
    auto tft = this->tft;
    if (!tft) return 0;

    int xp = 10;
    // int font1 = FONT_SM;
    int font2 = FONT_MD;
    // int fh = tft->fontHeight(font1);
    int fh2 = tft->fontHeight(font2);
    // int yp2 = yp + fh;

    // tft->setTextSize(1);

    // tft->setTextColor(TFT_GREEN, TFT_BLACK);
    // tft->setCursor(xp, yp, font1);

    // long rand = random(10, 99);

    const float temperatureC = this->getTemperature();

    // Note different degree symbols for console and tft fonts
    Serial.printf("%.1fºC\n", temperatureC);

    tft->setCursor(xp, yp, font2);
    tft->setTextColor(TFT_MAGENTA, TFT_BLACK);

    tft->printf("%.1f`C  \n", temperatureC);

    return fh2;
  }

  void initTempSensor() {
    auto tft = this->tft;

    Serial.printf("-- Temp Sensor Initialization started --\n");
    if (tft) tft->printf("Temp Sensor\n");

    // Start the DS18B20 sensor
    this->sensors.begin();

    Serial.printf("-- Temp Sensor Initialization done --\n");
  }

  // Constructor
  TempSensor(TFTUtils* tftUtils = nullptr) : sensors(&oneWire) {
    if (tftUtils) {
      this->tft = &(tftUtils->tft);
    }
  }
};

#endif  // TempSensor_DEFINED
