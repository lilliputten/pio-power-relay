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

static void __initRoutes(TServer &server) {
  // Root route
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = String(htmlPage);
    html.replace("%IP%", WiFi.localIP().toString());
    request->send(200, "text/html", html);
  });

  // JSON API endpoint
  server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"temp\":25.5,\"humidity\":60}";
    request->send(200, "application/json", json);
  });

  // Static files
  server.serveStatic("/", LittleFS, "/www/").setCacheControl("max-age=86400");

  // Handle 404
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/html", "<h1>Error</h1><p>Page not found</p>");
  });
}

void WWWServer::initServer() {
  Serial.println("-- Web Server Initialization started --");

  TServer &server = this->server;

  __initRoutes(server);

  server.begin();

  Serial.println("-- Web Server Initialization done --");
}
