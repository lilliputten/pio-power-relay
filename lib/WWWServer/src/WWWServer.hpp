#ifndef WWWServer_DEFINED
#define WWWServer_DEFINED

#include <ESPAsyncWebServer.h>
// #include <WebServer.h>
#include <WiFi.h>

#include "TFTUtils.hpp"

#define TServer AsyncWebServer  // WebServer or AsyncWebServer or whatever

class WWWServer {
 private:
  TServer server;
  TFT_eSPI* tft = nullptr;

 public:
  void initServer();
  TServer& getServer();

  WWWServer(TFTUtils* tftUtils = nullptr, uint16_t port = 80) : server(port) {
    // Constructor
    if (tftUtils) {
      this->tft = &tftUtils->getTFT();
    }
  }
};

#endif
