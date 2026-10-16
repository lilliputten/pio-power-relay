#include <LittleFS.h>

#include "WWWServer.hpp"

// HTML page
const char *htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <link rel="stylesheet" href="/styles.css" />
</head>
<body>
  <h1>ESP32 Web Server</h1>
  <p>Hello from ESP32!</p>
  <p>IP Address: %IP%</p>
</body>
</html>
)rawliteral";

#define TRequest AsyncWebServerRequest

void WWWServer::initRoutes(PowerPlan *powerPlan) {
  TServer &s = this->server;

  // Root route
  s.on("/", HTTP_GET, [](TRequest *request) {
    String html = String(htmlPage);
    html.replace("%IP%", WiFi.localIP().toString());
    request->send(200, "text/html", html);
  });

  // JSON API endpoint
  s.on("/api/data", HTTP_GET, [](TRequest *request) {
    String json = "{\"temp\":25.5,\"humidity\":60}";
    request->send(200, "application/json", json);
  });

  /** Send power plan slots data to the client by id
   */
  s.on("/api/power-plan/slots", HTTP_GET, [powerPlan](TRequest *request) {
    // 1. Check that the 'id' parameter is present
    if (!request->hasParam("id")) {
      String json = "{\"error\":\"missing 'id' parameter\"}";
      request->send(400, "application/json", json);
      return;
    }

    // 2. Parse it
    int id = request->getParam("id")->value().toInt();

    // // 3. Look up the plan for this id (your own storage/registry)
    // PowerPlan* powerPlan = findPowerPlanById(id);
    if (!powerPlan) {
      String json = "{\"error\":\"plan not found\"}";
      request->send(404, "application/json", json);
      return;
    }

    // 4. Serialize and send
    String json = powerPlan->getSlotsJson();
    request->send(200, "application/json", json);
  });

  /* Update power plan slots data with clients' data
   * Example payload:
   * {"id":1,"slots":[{"timeSec":5,"textId":"Test"}]}
   */
  s.on(
      "/api/power-plan/slots", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        // Response is sent from the body callback below; this handler can stay
        // empty
      },
      nullptr,  // no upload handler
      [powerPlan](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                  size_t index, size_t total) {
        // Accumulate the body across chunks (ESPAsyncWebServer may deliver
        // it in pieces)
        static String body;
        if (index == 0) body = "";
        body.reserve(total);
        body.concat((const char *)data, len);

        // Wait until the full body has arrived
        if (index + len != total) return;

        // Parse json
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, body);
        body = "";  // free the buffer as soon as we're done with it

        if (err) {
          const auto errCode = err.code();
          JsonDocument errorDoc;
          errorDoc["error"] = "Invalid json";
          errorDoc["errorCode"] = err.code();
          errorDoc["errorExplanation"] = err.c_str();
          String json;
          serializeJson(errorDoc, json);
          request->send(400, "application/json", json);
          Serial.printf("Parsing slots data error %s: %s", err.code(),
                        err.c_str());
          return;
        }

        // Validate required fields
        if (!doc["id"].is<int>() || !doc["slots"].is<JsonArray>()) {
          String json = "{\"error\":\"expected {id: int, slots: object[]}\"}";
          request->send(400, "application/json", json);
          return;
        }

        int id = doc["id"].as<int>();

        const auto jsonArray = doc["slots"].as<JsonArray>();
        Serial.printf("[initServer] Received slots: id=%d count=%d", id,
                      jsonArray.size());

        const String errorCode = powerPlan->updateSlotsFromJsonArray(jsonArray);
        if (errorCode || errorCode != "OK") {
          JsonDocument errorDoc;
          errorDoc["error"] = "Updating data error";
          errorDoc["errorCode"] = errorCode;
          const String json = errorDoc.as<String>();
          request->send(400, "application/json", json);
          body = "";
          return;
        }

        String json = "{\"ok\":true}";
        request->send(200, "application/json", json);
      });

  // Static files
  s.serveStatic("/", LittleFS, "/www/").setCacheControl("max-age=86400");

  // Handle 404
  s.onNotFound([](TRequest *request) {
    request->send(404, "text/html", "<h1>Error</h1><p>Page not found</p>");
  });
}

void WWWServer::initServer() {
  // __initRoutes(this->server);
}

void WWWServer::startServer() { this->server.begin(); }
