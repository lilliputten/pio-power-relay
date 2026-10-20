#ifndef WWWServer_DEFINED
#define WWWServer_DEFINED

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "PowerStation.hpp"
#include "TFTUtils.hpp"
#include "TempSensor.hpp"
#include "constants.hpp"

#define TServer AsyncWebServer  // WebServer or AsyncWebServer or whatever

class WWWServer {
 private:
  TServer server;

  // TFT_eSPI* tft = nullptr;
  TFTUtils* tftUtils = nullptr;

  // Referenced objects
  PowerStation* powerStation = nullptr;
  TempSensor* tempSensor = nullptr;

  void initRoutes();

 public:
  void startServer() { this->server.begin(); }

  TServer& getServer() { return this->server; }

  void setPowerStation(PowerStation* ref) { this->powerStation = ref; }
  void setTempSensor(TempSensor* ref) { this->tempSensor = ref; }

  void initServer() {
    if (tftUtils) tftUtils->writeLine("Starting WWW Server...");

    if (!this->powerStation) {
      Serial.println(
          YELLOW
          "[WWWServer]: PowerStation reference hasn't been passed!" RESET);
    }
    if (!this->tempSensor) {
      Serial.println(
          YELLOW "[WWWServer]: TempSensor reference hasn't been passed!" RESET);
    }

    this->initRoutes();
  }

  WWWServer(TFTUtils* tftUtils = nullptr, uint16_t port = 80) : server(port) {
    if (tftUtils) {
      this->tftUtils = tftUtils;
    }
  }
};

#endif
