#ifndef WWWServer_DEFINED
#define WWWServer_DEFINED

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "PowerPlan.hpp"
#include "TFTUtils.hpp"

#define TServer AsyncWebServer  // WebServer or AsyncWebServer or whatever

class WWWServer {
 private:
  TServer server;
  TFT_eSPI* tft = nullptr;

 public:
  void initServer();
  void initRoutes(PowerPlan* powerPlan);
  void startServer();

  TServer& getServer();

  WWWServer(TFTUtils* tftUtils = nullptr, uint16_t port = 80) : server(port) {
    // Constructor
    if (tftUtils) {
      this->tft = &tftUtils->getTFT();
    }
  }
};

#endif
