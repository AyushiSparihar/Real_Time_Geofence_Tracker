#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// —— GPS setup ——  
SoftwareSerial gpsSerial(D7, D6);  // GPS TX → D7, RX → D6
TinyGPSPlus gps;

// —— Wi-Fi credentials ——  
const char* ssid     = "Redmi 9A";
const char* password = "Ayushi98";

// —— Geofence params ——  
// double homeLat = 26.23063895374904;
// double homeLng = 78.20721611364098;
double homeLat = 26.230157;
double homeLng = 78.210917;
double radius  = 0.1;  // km

// —— Web server on port 80 ——  
ESP8266WebServer server(80);

void handleRoot() {
  String html =
    "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<meta http-equiv='refresh' content='5'>"
    "<title>GPS Geofence</title>"
    "<style>"
    "body{font-family:'Segoe UI',sans-serif; background:#f0f2f5; color:#333; margin:0; padding:20px;}"
    "h1{color:#007bff;}"
    ".card{background:#fff; border-radius:10px; padding:20px; box-shadow:0 2px 8px rgba(0,0,0,0.1); max-width:500px; margin:auto;}"
    ".status{font-size:1.2em; font-weight:bold;}"
    ".inside{color:green;}"
    ".outside{color:red;}"
    "</style></head><body><div class='card'>"
    "<h1>📍 Geofence Tracker</h1>";

  if (gps.location.isValid()) {
    double lat = gps.location.lat();
    double lng = gps.location.lng();
    html += "<p><strong>Latitude:</strong> " + String(lat, 6) + "</p>";
    html += "<p><strong>Longitude:</strong> " + String(lng, 6) + "</p>";

    double dist = TinyGPSPlus::distanceBetween(lat, lng, homeLat, homeLng) / 1000.0;
    if (dist > radius) {
      html += "<p class='status outside'>⚠️ Outside geofence!</p>";
    } else {
      html += "<p class='status inside'>✅ Inside geofence.</p>";
    }
  } else {
    html += "<p class='status'>🔄 Waiting for GPS fix…</p>";
  }

  html += "</div></body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected! IP = " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started at http://" + WiFi.localIP().toString());
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
  server.handleClient();
}

