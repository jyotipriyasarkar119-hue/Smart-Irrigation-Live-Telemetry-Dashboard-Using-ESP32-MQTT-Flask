# Smart Irrigation & Live Telemetry Dashboard Using ESP32, MQTT & Flask

[![Hardware](https://img.shields.io/badge/Hardware-ESP32%20%2B%20Sensors-007acc)](#)
[![Status](https://img.shields.io/badge/Status-Active%20Prototype-f97316)](#)
[![Protocol](https://img.shields.io/badge/Protocol-MQTT%20over%20Wi--Fi-8b5cf6)](#)
[![Backend](https://img.shields.io/badge/Backend-Python%20%2B%20Flask%20%2B%20SQLite-10b981)](#)
[![License](https://img.shields.io/badge/License-MIT-eab308)](LICENSE)
[![Validation](https://img.shields.io/badge/System%20Validation-passing-2ea44f?logo=github)](#)

An intelligent, full-stack IoT precision agriculture system built on an **ESP32 microcontroller** and a local **Linux host server**. It continuously monitors soil moisture, ambient temperature, and relative humidity, transmits live JSON telemetry over **MQTT** to a local **Mosquitto** broker, logs historical records into an **SQLite** database via a **Python Flask** backend, and serves a dynamic, dark-themed **Web Dashboard** with real-time updates.

## Features

- 📊 **Real-time telemetry readout** — measures soil moisture %, ambient temperature (°C), and relative humidity (%) every 5 seconds
- 💧 **Automatic pump control** — switches a water pump relay `ON` when soil moisture falls below a configurable dry threshold (< 30%) and `OFF` once adequately moist
- 📡 **MQTT telemetry pipeline** — lightweight publish/subscribe messaging between the ESP32 edge node and host Mosquitto broker over local Wi-Fi
- 💾 **Persistent local logging** — automatically records all sensor payloads into an SQLite database (`irrigation.db`)
- 🖥️ **Live Web Dashboard** — responsive dark-mode UI on port `5000` with metric cards and historical log tables (auto-refreshing via Fetch API)
- ⚙️ **Calibrated ADC range** — accurately maps 12-bit ESP32 raw analog values (`0–4095`) to intuitive 0–100% moisture levels

## How It Works

1. The ESP32 reads raw analog output from a capacitive soil moisture probe on **GPIO 34** and environmental readings from a DHT11 sensor on **GPIO 4**.
2. Telemetry data is packaged into a structured JSON payload containing raw ADC, moisture %, temperature, humidity, and pump status.
3. The JSON payload is published over Wi-Fi to the MQTT topic `farm/sensors/data`.
4. The Python backend (`server.py`) subscribes to Mosquitto, parses incoming payloads, logs data to stdout, and saves entries to `irrigation.db`.
5. The Flask server exposes REST API endpoints (`/api/data`) that feed the web dashboard (`index.html`), updating UI metrics every 2 seconds without page reloads.

## Hardware Required

| Component | Notes |
|---|---|
| ESP32 Development Board | Main microcontroller with built-in Wi-Fi |
| Soil moisture sensor (analog) | Capacitive recommended to prevent probe corrosion |
| DHT11 temperature & humidity sensor | Single-bus digital environmental sensor |
| 1-channel relay module | Active-LOW relay module to switch pump power |
| Water pump (5V/12V mini/submersible) | Powered through external power, **not** directly from ESP32 |
| Jumper wires + breadboard | For prototyping circuit connections |
| External power supply for pump | Dedicated 5V/12V power supply for the water pump |

## Circuit / Pin Connections

| Component Pin | ESP32 Pin | Notes |
|---|---|---|
| Soil Moisture Sensor — Signal (AOUT) | GPIO 34 | Analog input (ADC1_CH6) |
| Soil Moisture Sensor — VCC / GND | 3.3V / GND | Power supply for sensor |
| DHT11 Sensor — Data | GPIO 4 | Digital data signal |
| DHT11 Sensor — VCC / GND | 3.3V / GND | Power supply for DHT11 |
| Relay Module — IN | GPIO 26 | Digital output control pin |
| Relay Module — VCC / GND | 5V / GND | Relay coil power supply |
| Water Pump — via Relay COM/NO | External Power Supply | Intercepts positive power wire |

> ⚠️ **Relay Logic Note:** The relay is wired **active-LOW**: `digitalWrite(RELAY_PIN, LOW)` triggers the pump **ON**, and `HIGH` keeps it **OFF**. If your relay is active-HIGH, invert the logic in your firmware.

## Software Requirements

### Microcontroller (ESP32)
- [Arduino IDE](https://www.arduino.cc/en/software) (1.8.x or 2.x) with ESP32 board support installed
- **PubSubClient** library (by Nick O'Leary)
- **DHT sensor library** (by Adafruit)

### Host Server (Linux / Fedora)
- **Python** (3.10+)
- **Mosquitto MQTT Broker** (`sudo dnf install mosquitto`)
- **SQLite3**
- **Python Dependencies:** `Flask`, `paho-mqtt`

## Getting Started

1. **Clone the repository**
   ```bash
   git clone [[https://github.com/YOUR_USERNAME/smart-irrigation-esp32-mqtt.git](https://github.com/YOUR_USERNAME/smart-irrigation-esp32-mqtt.git)](https://github.com/jyotipriyasarkar119-hue/soil-moisture-monitoring-with-automatic-smart-irrigation-esp32-mqtt.git)
   cd smart-irrigation-esp32-mqtt
2. **Open the sketch** — open `firmware/esp32_irrigation_node/esp32_irrigation_node.ino` in the Arduino IDE. *(Tip: Arduino IDE recognizes `.ino` files by default — rename the file to `arduinocode.ino` if it doesn't open directly as a sketch.)*

3. **Install the DHT11, PubSubClient, PubSubClient library** through the Library Manager if you haven't already.

4. **Wire the hardware** as described in the pin table above.

5. **Select your board and port** under *Tools → Board → ESP32 Dev Module* and *Tools → Port*.

6. **Upload** the sketch to your Arduino.

7. Power the system -the data of the environment will be shown in the localhost.

   
## Configuration

All system tuning and network parameters are configured via constants near the top of `firmware/esp32_irrigation_node/esp32_irrigation_node.ino`:

```cpp
// Wi-Fi & MQTT Network Credentials
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER   = "192.168.X.X";      // Local IP of your Fedora server

// Calibration & Threshold Constants
const int AIR_VALUE       = 4095; // Raw 12-bit ADC reading in dry air (0% moisture)
const int WATER_VALUE     = 1300; // Raw 12-bit ADC reading submerged in water (100% moisture)
const int DRY_THRESHOLD   = 30;   // Moisture % threshold below which the pump turns ON
```
## Repository Structure
```
smart-irrigation-esp32-mqtt/
├── firmware/
│   └── esp32_irrigation_node/
│       └── esp32_irrigation_node.ino   # ESP32 C++ Arduino Sketch
├── server/
│   ├── templates/
│   │   └── index.html                 # Frontend Web Dashboard
│   ├── server.py                      # Flask + MQTT Consumer + SQLite Backend
│   └── requirements.txt               # Python Dependencies
├── .gitignore                         # Prevents uploading venv, DB, secrets
├── LICENSE                            # Open-source license (MIT)
└── README.md                          # Project Documentation
```
## Possible Improvements

- **Interactive Remote Overrides** — Add interactive controls to the web dashboard allowing users to toggle manual pump states (`ON` / `OFF` / `AUTO`) remotely via MQTT command topics
- **Historical Charting & Analytics** — Integrate Chart.js or Grafana into the web UI to visualize time-series trends for soil moisture, temperature, and pump runtime
- **Automated Alerts & Notifications** — Add a notification service (e.g., Telegram Bot or Email via SMTP) to send real-time alerts if moisture drops critically low or if a sensor stops transmitting
- **Predictive Irrigation Model** — Analyze historical moisture decay logs using machine learning to forecast optimal watering schedules and prevent overwatering
- **Power Optimization (Deep Sleep)** — Implement ESP32 Deep Sleep mode between sensor readings for battery or solar-powered field deployment
- **Security & Access Control** — Enable TLS/SSL encryption for MQTT (`mqtts://`) and HTTPS, along with user authentication for the Flask Web Dashboard
## License

Distributed under the [MIT License](LICENSE). See the `LICENSE` file for details.
## Author

[jyotipriyasarkar119-hue](https://github.com/jyotipriyasarkar119-hue)
