// =====================================================================
//  Beekeeper (ESP-01 side) - DRAFT, sprint 2
//
//  Joins your WiFi, listens on its serial pins for "CSV,..." lines
//  (later sent by the receiving Maduino), keeps the last 50, and serves
//  them as a web page at http://beekeeper.local (or its IP address).
//  It also gets real time from the internet (NTP), so every reading is
//  stamped with the time it arrived, even if Huneycomb has no clock.
//
//  For now you can test it by typing lines into the Serial Monitor
//  through the UNO, e.g.:  CSV,1,,0,22.5,31.0,812.4,-1,nan,3300
//  Type ?  to print the WiFi status.
//
//  ---- One-time IDE setup -------------------------------------------
//  File > Preferences > Additional boards manager URLs, add:
//    https://arduino.esp8266.com/stable/package_esp8266com_index.json
//  Tools > Board > Boards Manager: install "esp8266 by ESP8266 Community"
//  Tools > Board: "Generic ESP8266 Module"
//        Flash Size: "1MB (FS:none ...)"   Upload Speed: 115200
//        Reset Method: "no dtr (aka ck)"
//  Copy secrets.example.h to secrets.h (same folder) and fill it in.
//
//  ---- Uploading through the UNO (RESET jumpered to GND) ------------
//  1. Move the ESP-01's GPIO0 wire from 3.3V to GND (flash mode).
//  2. Power-cycle the ESP-01 (pull its VCC wire for a second).
//  3. Port = the UNO's COM port. Click Upload.
//  4. After "Done uploading": GPIO0 back to 3.3V, power-cycle again.
//  5. Serial Monitor at 9600 baud, line ending "Newline".
//  Note: this replaces the ESP-01's AT-command firmware.
// =====================================================================

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h>
#include "secrets.h"            // defines WIFI_SSID and WIFI_PASS

#define LINK_BAUD  9600         // must match the Maduino in sprint 3
#define HOSTNAME   "beekeeper"
#define TZ_INFO    "MST7MDT,M3.2.0,M11.1.0"   // Mountain time with DST
#define MAX_ROWS   50

struct Row {
  time_t when;                  // arrival time (0 = clock not set yet)
  String line;
};

Row rows[MAX_ROWS];
uint8_t head = 0, count = 0;    // ring buffer of recent readings
String inBuf;
ESP8266WebServer server(80);

// Debug lines start with "#" so the Maduino can ignore them later.
void logln(const String& s) { Serial.print(F("# ")); Serial.println(s); }

String fmtTime(time_t t) {
  if (t < 1700000000) return F("(no clock yet)");
  struct tm tmv;
  localtime_r(&t, &tmv);
  char b[24];
  strftime(b, sizeof b, "%Y-%m-%d %H:%M:%S", &tmv);
  return String(b);
}

void storeLine(const String& line) {
  rows[head].when = time(nullptr);
  rows[head].line = line;
  head = (head + 1) % MAX_ROWS;
  if (count < MAX_ROWS) count++;
}

// i = 0 is the newest row
const Row& rowAt(uint8_t i) { return rows[(head + MAX_ROWS - 1 - i) % MAX_ROWS]; }

void printStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    logln(String("WiFi OK: ") + WiFi.localIP().toString() + "  RSSI " + String(WiFi.RSSI()) + " dBm");
    logln(String("Open http://" HOSTNAME ".local or http://") + WiFi.localIP().toString());
  } else {
    logln(F("WiFi not connected"));
  }
  logln(String("Clock: ") + fmtTime(time(nullptr)) + "   stored readings: " + String(count));
}

// ---------------------------------------------------------------------
// Web pages
// ---------------------------------------------------------------------
void handleRoot() {
  String h;
  h.reserve(4000);
  h += F("<!DOCTYPE html><html><head><meta charset='utf-8'>"
         "<meta name='viewport' content='width=device-width,initial-scale=1'>"
         "<meta http-equiv='refresh' content='30'><title>Beekeeper</title>"
         "<style>body{font-family:sans-serif;margin:1em}"
         "table{border-collapse:collapse;font-size:14px}"
         "td,th{border:1px solid #ccc;padding:3px 6px;text-align:right}"
         "th{background:#f3f0e6}</style></head><body>"
         "<h2>Beekeeper</h2><p>");
  h += String("Now: ") + fmtTime(time(nullptr));
  h += String(" &middot; WiFi ") + String(WiFi.RSSI()) + " dBm";
  h += String(" &middot; readings stored: ") + String(count);
  h += F(" &middot; <a href='/csv'>download CSV</a></p>");

  if (count == 0) {
    h += F("<p>No readings yet. Waiting for Huneycomb&hellip;</p>");
  } else {
    h += F("<table><tr><th>received</th><th>seq</th><th>node time</th><th>ms</th>"
           "<th>temp C</th><th>RH %</th><th>hPa</th><th>probe</th>"
           "<th>rtc C</th><th>mV</th></tr>");
    for (uint8_t i = 0; i < count; i++) {
      const Row& r = rowAt(i);
      String cells = r.line.substring(4);       // drop "CSV,"
      cells.replace(",", "</td><td>");
      h += String("<tr><td>") + fmtTime(r.when) + "</td><td>" + cells + "</td></tr>";
    }
    h += F("</table>");
  }
  h += F("<p style='color:#888'>Page refreshes every 30 s.</p></body></html>");
  server.send(200, F("text/html"), h);
}

void handleCsv() {
  String out = F("received,seq,node_time,ms,temp_c,rh_pct,press_hpa,probe_raw,rtc_temp_c,vcc_mv\n");
  for (int i = count - 1; i >= 0; i--) {         // oldest first
    const Row& r = rowAt(i);
    out += fmtTime(r.when) + "," + r.line.substring(4) + "\n";
  }
  server.send(200, F("text/csv"), out);
}

// ---------------------------------------------------------------------
// Serial link
// ---------------------------------------------------------------------
void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;
  if (line.startsWith("CSV,")) {
    storeLine(line);
    logln(String("stored #") + String(count) + " at " + fmtTime(time(nullptr)));
  } else if (line == "?") {
    printStatus();
  } else {
    logln(String("ignored: ") + line);
  }
}

void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inBuf.length()) handleLine(inBuf);
      inBuf = "";
    } else if (inBuf.length() < 200) {
      inBuf += c;
    }
  }
}

// ---------------------------------------------------------------------
void setup() {
  Serial.begin(LINK_BAUD);
  delay(300);
  Serial.println();
  logln(F("Beekeeper ESP-01 starting"));

  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print(F("# connecting to " WIFI_SSID));
  for (uint8_t i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();

  configTime(TZ_INFO, "pool.ntp.org", "time.nist.gov");

  if (MDNS.begin(HOSTNAME)) MDNS.addService("http", "tcp", 80);
  server.on("/", handleRoot);
  server.on("/csv", handleCsv);
  server.onNotFound([]() { server.send(404, F("text/plain"), F("not found")); });
  server.begin();

  printStatus();
  if (WiFi.status() != WL_CONNECTED)
    logln(F("Check secrets.h (2.4 GHz networks only) - will keep retrying"));
}

void loop() {
  server.handleClient();
  MDNS.update();
  readSerial();

  static bool wasUp = false;
  bool up = WiFi.status() == WL_CONNECTED;
  if (up != wasUp) {             // report connects/drops
    wasUp = up;
    printStatus();
  }
}