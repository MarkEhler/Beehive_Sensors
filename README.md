# Beehive Sensors

A hive monitor in two parts:

- **Huneycomb**: a sensor node at the hive (about half a mile out). It reads temperature, humidity and pressure, logs them, and sends them over LoRa.
- **Beekeeper**: a receiver in the house. It gets the LoRa packets and forwards them over WiFi (ESP-01) to a server on the home network.

## Hardware

| Part | Role |
|---|---|
| 2 × Makerfabs Maduino LoRa Radio v2.0 (ATmega328P 3.3V/8MHz + RFM95W, 868 MHz version) | Huneycomb and Beekeeper MCU + radio |
| HiLetgo GY-BME280-3.3 | Temperature / humidity / pressure (I²C) |
| OSEPP TEMP-02 | Analog temperature probe (chip type TBD) |
| ESP-01 (ESP8266) | Beekeeper WiFi |
| HiLetgo data logger shield (SD + DS1307) | Not used on the Maduino: SPI pin conflict on D10, 5V RTC. Replace with a 3.3V microSD breakout. |
| MQ135 | Shelved: ~150 mA heater, not suitable for battery power |

**Radio band:** the boards are the 868 MHz build, but in the US the license-free band is 902–928 MHz, so the firmware runs at **915.0 MHz**. Expect somewhat reduced range with the 868-tuned antenna; a 915 MHz antenna is the fix if needed.

## Sprints

1. **Huneycomb on the bench**: sensor powered from USB and talking to the laptop. ← *current* (`huneycomb/`)
2. **Beekeeper on the bench**: receiver Maduino + ESP-01 troubleshooting while plugged into the laptop.
3. **Rx ↔ Tx**: Huneycomb sends real readings, Beekeeper receives them, both on the desk.
4. **At range**: Huneycomb at the hive, battery/solar, sleep between readings, RSSI/range tuning.
5. **Server**: something on the LAN to store and graph the data.

## Setup (Arduino IDE 2)

1. Install the [CP2104 USB driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) if the board doesn't show up as a port (macOS/Windows; Linux has it built in).
2. **Tools → Board → Arduino AVR Boards → Arduino Pro or Pro Mini**
   **Tools → Processor → ATmega328P (3.3V, 8 MHz)**
3. Library Manager: install **Adafruit BME280 Library** (accept its dependencies, including Adafruit Unified Sensor and Adafruit BusIO).
   For sprint 3 you'll also need **RadioHead** (installed from its zip; see the Makerfabs wiki).
4. Open `huneycomb/huneycomb.ino`, upload, then open the Serial Monitor at **9600 baud**.

## Sprint 1: Huneycomb bench test

### Wiring

| BME280 | Maduino |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

Optional: analog temp probe signal → A0 (`USE_PROBE 1`). microSD breakout CS → D8 (`USE_SD 1`).

### What you should see

```
=== Huneycomb - sprint 1 bench test ===
I2C scan:
  device at 0x76  <- BME280/BMP280 address
BME280 OK at 0x76
--- Huneycomb info ---
Free RAM: (should be well over 500) bytes (of 2048)
Supply:   ~3300 mV
...
#1  T=22.41C  RH=31.2%  P=812.6hPa  probe=...  Vcc=3312mV
CSV,1,1534,22.41,31.2,812.6,...,3312
```

(Pressure around 800–850 hPa is normal at Colorado altitudes.)

The LED blinks **2×** at boot if the BME280 is found and **5×** if it isn't, then once per reading.

### Serial commands

`h` help · `r` read now · `s` I²C scan · `i` info · `f` fast mode (2 s) · `p` pause · `d` dump SD log

### Troubleshooting

| Symptom | Check |
|---|---|
| No serial port | CP2104 driver; try another USB cable (some are charge-only) |
| Upload fails / `not in sync` | Board must be **Pro Mini 3.3V 8 MHz**, not UNO |
| Garbled serial text | Monitor set to 9600; wrong processor setting (16 MHz) also garbles |
| I²C scan finds nothing | VCC on **3V3**, SDA/SCL swapped, loose jumper |
| Found at 0x77 instead of 0x76 | Fine, the sketch tries both |
| Sensor ID 0x58 | It's a BMP280: no humidity (humidity reads `nan`) |
| Temperature reads a degree or two high | Self-heating or board heat; keep the sensor on a short lead away from the Maduino |
| Probe reads ~0 or 1023 | Wiring, or the probe needs more than 3.3 V (an LM35 needs ≥4 V) |
| SD `FAILED` | Card formatted FAT32, CS on D8, module is 3.3V-compatible |


