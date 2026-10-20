#include <LittleFS.h>

#include "WWWServer.hpp"

#ifdef ASYNCWEBSERVER_REGEX
#define USE_REGEX_URLS ASYNCWEBSERVER_REGEX
#endif

// HTML page
const char* htmlPage = R"rawliteral(
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

int getRequestIdParam(TRequest* request) {
#ifdef USE_REGEX_URLS
  // Get the first captured regex group (the id)
  String idStr = request->pathArg(0);
  int id = idStr.toInt();
#else
  // Retrieve the 'id' parameter if it's present from the url query param
  if (!request->hasParam("id")) {
    String json = "{\"error\":\"missing 'id' parameter\"}";
    request->send(400, "application/json", json);
    return -1;
  }
  int id = request->getParam("id")->value().toInt();
#endif
  return id;
}

PowerPlan* getPowerPlanFromRequest(TRequest* request,
                                   PowerStation* powerStation) {
  int id = getRequestIdParam(request);
  if (id == -1) return nullptr;

  // // Look up the plan for this id (your own storage/registry)
  PowerPlan* powerPlan = powerStation->findPowerPlanById(id);
  if (!powerPlan) {
    String json = "{\"error\":\"plan " + String(id) + " not found\"}";
    request->send(404, "application/json", json);
    return nullptr;
  }

  return powerPlan;
}

void WWWServer::initRoutes() {
  TServer& s = this->server;
  PowerStation* powerStation = this->powerStation;
  TempSensor* tempSensor = this->tempSensor;

  // Root route demo (will be replaced by the SPA)
  s.on("/", HTTP_GET, [](TRequest* request) {
    String html = String(htmlPage);
    html.replace("%IP%", WiFi.localIP().toString());
    request->send(200, "text/html", html);
  });

  // JSON API endpoint
  s.on("/api/demo", HTTP_GET, [](TRequest* request) {
    String json = "{\"demo\":true,\"temp\":25.5,\"humidity\":60}";
    request->send(200, "application/json", json);
  });

  /** Get the device status: i.e. all the possible data */
  s.on("/api/status", HTTP_GET, [tempSensor](TRequest* request) {
    JsonDocument doc;
    doc["ok"] = true;
    doc["powerChannelsCount"] = POWER_CHANNELS_COUNT;
    if (tempSensor) {
      doc["temp"] = tempSensor->getTemperature();
    }
    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });

  /** Get power plans count */
  s.on("/api/power/plans-count", HTTP_GET, [powerStation](TRequest* request) {
    String json =
        "{\"ok\":true,\"count\":" + String(POWER_CHANNELS_COUNT) + "}";
    request->send(200, "application/json", json);
  });

  /** Send power plan status data to the client by id */
  s.on(
#ifdef USE_REGEX_URLS
      "^\\/api\\/power\\/plan\\/([0-9]+)\\/status$",
#else
    "/api/power/plan/status",
#endif
      HTTP_GET, [powerStation](TRequest* request) {
        // Look up the plan for the passed id
        PowerPlan* powerPlan = getPowerPlanFromRequest(request, powerStation);
        if (!powerPlan) {
          return;
        }

        // Get prepared json data
        auto doc = powerPlan->getStatusJson();

        // Serialize and send
        String json;
        serializeJson(doc, json);
        doc["ok"] = true;
        request->send(200, "application/json", json);
      });

  /** Send power plan slots data to the client by id */
  s.on(
#ifdef USE_REGEX_URLS
      "^\\/api\\/power\\/plan\\/([0-9]+)\\/slots$",
#else
    "/api/power/plan/slots",
#endif
      HTTP_GET, [powerStation](TRequest* request) {
        // Look up the plan for the passed id
        PowerPlan* powerPlan = getPowerPlanFromRequest(request, powerStation);
        if (!powerPlan) {
          return;
        }

        // Get prepared json data
        auto doc = powerPlan->getSlotsJson();

        // Serialize and send
        String json;
        serializeJson(doc, json);
        doc["ok"] = true;
        request->send(200, "application/json", json);
      });

  /* Update power plan slots data with clients' data
   * Example payload:
   * {"id":1,"slots":[{"timeSec":5,"textId":"Test"}]}
   */
  s.on(
#ifdef USE_REGEX_URLS
      "^\\/api\\/power\\/plan\\/([0-9]+)\\/slots$",
#else
      "/api/power/plan/slots",
#endif
      HTTP_POST,
      [](AsyncWebServerRequest* request) {
        // Response is sent from the body callback below; this handler can stay
        // empty
      },
      nullptr,  // no upload handler
      [powerStation](AsyncWebServerRequest* request, uint8_t* data, size_t len,
                     size_t index, size_t total) {
        int id = getRequestIdParam(request);
        if (id == -1) return;

        // Accumulate the body across chunks (ESPAsyncWebServer may deliver
        // it in pieces)
        static String body;
        if (index == 0) body = "";
        body.reserve(total);
        body.concat((const char*)data, len);

        Serial.printf("Got update: id=%d, len=%d index=%d total=%d\n", id, len,
                      index, total);

        // Wait until the full body has arrived
        if (index + len != total) return;

        // Parse json
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, body);
        body = "";  // free the buffer as soon as we're done with it

        Serial.printf("Got full data\n");

        if (err) {
          const auto errCode = err.code();
          JsonDocument errorDoc;
          errorDoc["error"] = "Invalid json";
          errorDoc["errorCode"] = err.code();
          errorDoc["errorDetails"] = err.c_str();
          String json;
          serializeJson(errorDoc, json);
          request->send(400, "application/json", json);
          Serial.printf("Parsing slots data error %s: %s", err.code(),
                        err.c_str());
          return;
        }

        // Validate required fields
        if (/* !doc["id"].is<int>() || */ !doc["slots"].is<JsonArray>()) {
          const String errStr = "expected {slots: object[]}";
          String json = "{\"error\":\"" + errStr + "\"}";
          request->send(400, "application/json", json);
          return;
        }
        /* // Retrieve the id from the data (obsolete: it's passed in the url
         * and parsed from `request->pathArg` int id = doc["id"].as<int>();
         */

        // // 3. Look up the plan for this id (your own storage/registry)
        PowerPlan* powerPlan = powerStation->findPowerPlanById(id);
        if (!powerPlan) {
          String json = "{\"error\":\"plan " + String(id) + " not found\"}";
          request->send(404, "application/json", json);
          return;
        }

        const auto jsonArray = doc["slots"].as<JsonArray>();
        Serial.printf("[initServer] Received slots: id=%d count=%d", id,
                      jsonArray.size());

        const String errorCode = powerPlan->updateSlotsFromJsonArray(jsonArray);
        if (errorCode && errorCode != "OK") {
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
  s.onNotFound([](TRequest* request) {
    request->send(404, "text/html", "<h1>Error</h1><p>Page not found</p>");
  });
}
