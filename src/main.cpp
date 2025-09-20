#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
ESP8266WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

char* AP_NAME = "Free WiFi ( CONNECT NOW !!!!!!!)";

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NAME);

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  server.on("/", []()
            {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Get rickrolled</title>
      <style>
        html, body {
          margin: 0;
          padding: 0;
          height: 100%;
          width: 100%;
          background: black;
        }
        img {
          display: block;
          width: 100%;
          height: 100%;
        }
      </style>

      </head>
      <body>
        <img src="/gif" alt="GIF">
      </body>
      </html>
  )rawliteral";
  server.send(200, "text/html", html); });

  server.on("/gif", []() {
    File file = LittleFS.open("/rickroll.gif", "r");
    if (!file) {
      server.send(404, "text/plain", "GIF not found");
      return;
    }
    server.streamFile(file, "image/gif");
    file.close();
  });

  // Captive portal detection endpoints → redirect to "/"
  server.on("/generate_204", []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/fwlink", []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/hotspot-detect.html", []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/ncsi.txt", []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  // Catch-all → redirect to "/"
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
