// =====================================================================
//  rtc_test - first hardware test: ZS-042 DS3231 clock module only
//
//  Prints the time and the DS3231's built-in temperature every 2 s.
//  Board: Maduino  -> Tools > Board "Arduino Pro or Pro Mini",
//                     Processor "ATmega328P (3.3V, 8 MHz)"
//         UNO      -> "Arduino Uno"
//  Library: RTClib (by Adafruit)
//  Serial Monitor: 9600 baud
//
//  Breadboard: ZS-042 header in row F, holes 10-15
//     F10 GND  -> board GND
//     F11 VCC  -> board 3V3   (3.3V, NOT 5V - see coin-cell note)
//     F12 SDA  -> A4
//     F13 SCL  -> A5
//     F14 SQW  -> not connected (later: wake-up alarm)
//     F15 32K  -> not connected
//  Jumper wires go in G-J of the same numbered row (e.g. J10 for GND).
//
//  Serial commands: c = set clock to the time this sketch was compiled
//                   s = rescan the I2C bus
// =====================================================================

#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;
bool rtcOk = false;

void scanI2C() {
  Serial.println(F("I2C scan:"));
  uint8_t found = 0;
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.print(F("  0x")); if (a < 16) Serial.print('0'); Serial.print(a, HEX);
      if (a == 0x68) Serial.print(F("  <- DS3231 clock"));
      if (a == 0x57) Serial.print(F("  <- AT24C32 EEPROM on the ZS-042"));
      Serial.println();
      found++;
    }
  }
  if (!found) {
    Serial.println(F("  nothing found - check VCC/GND, and SDA->A4, SCL->A5"));
    Serial.println(F("  (on the breadboard: is each jumper in the same numbered row?)"));
  }
}

void setToCompileTime() {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println(F("Clock set to compile time."));
}

void setup() {
  Serial.begin(9600);
  delay(200);
  Serial.println();
  Serial.println(F("=== rtc_test: ZS-042 / DS3231 ==="));

  Wire.begin();
  scanI2C();

  rtcOk = rtc.begin();
  if (!rtcOk) {
    Serial.println(F("DS3231 not found at 0x68. Fix wiring, then press RESET."));
    return;
  }
  Serial.println(F("DS3231 found."));
  if (rtc.lostPower()) {
    Serial.println(F("Clock lost power (new/dead coin cell?)."));
    setToCompileTime();
  }
  Serial.println(F("Note: the DS3231 updates its temperature every 64 s, "
                   "0.25 C steps, about +/-3 C accuracy."));
  Serial.println(F("Commands: c = set clock, s = I2C scan"));
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 's') scanI2C();
    if (c == 'c' && rtcOk) setToCompileTime();
  }

  static uint32_t last = 0;
  if (rtcOk && millis() - last >= 2000) {
    last = millis();
    DateTime n = rtc.now();
    char buf[20];
    snprintf(buf, sizeof buf, "%04d-%02d-%02d %02d:%02d:%02d",
             n.year(), n.month(), n.day(), n.hour(), n.minute(), n.second());
    Serial.print(buf);
    Serial.print(F("   temp "));
    Serial.print(rtc.getTemperature(), 2);
    Serial.println(F(" C"));
  }
}