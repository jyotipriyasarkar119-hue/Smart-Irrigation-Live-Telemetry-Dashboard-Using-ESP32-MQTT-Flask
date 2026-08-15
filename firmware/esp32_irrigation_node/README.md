# ESP32 Irrigation Sensing Node

## 1. Overview

The ESP32 Irrigation Sensing Node is the field-side embedded component of the Smart Irrigation system.

It collects sensor measurements, performs basic processing, connects to the local Wi-Fi network, and publishes telemetry to the MQTT broker.

```text
┌─────────────────────────────┐
│        ESP32 NODE           │
│                             │
│  ┌───────────────────────┐  │
│  │   Sensor Acquisition  │  │
│  └───────────┬───────────┘  │
│              │              │
│              ▼              │
│  ┌───────────────────────┐  │
│  │   Data Processing      │  │
│  └───────────┬───────────┘  │
│              │              │
│              ▼              │
│  ┌───────────────────────┐  │
│  │   MQTT Client          │  │
│  └───────────┬───────────┘  │
└──────────────┼──────────────┘
               │
              Wi-Fi
               │
               ▼
        MQTT Broker
```

## 2. Responsibilities

The ESP32 firmware is responsible for:

- Initializing connected sensors
- Reading sensor measurements
- Processing raw sensor values
- Validating sensor readings
- Connecting to Wi-Fi
- Connecting to the MQTT broker
- Publishing telemetry
- Publishing device status
- Handling Wi-Fi reconnection
- Handling MQTT reconnection
- Controlling the irrigation actuator when implemented

The ESP32 should not be responsible for permanent telemetry storage.

Persistent storage is handled by the backend/database layer.

## 3. Firmware Structure

The recommended firmware structure is:

```text
firmware/
└── esp32/
    │
    ├── README.md
    │
    ├── src/
    │   ├── main.ino
    │   ├── config.h
    │   ├── sensors.h
    │   ├── sensors.cpp
    │   ├── mqtt_client.h
    │   ├── mqtt_client.cpp
    │   ├── wifi_manager.h
    │   ├── wifi_manager.cpp
    │   └── irrigation_control.h
    │
    └── config/
        └── config.example.h
```

### File Responsibilities

| File | Responsibility |
|---|---|
| `main.ino` | Main firmware execution |
| `config.h` | Runtime configuration |
| `sensors.h` | Sensor interface definitions |
| `sensors.cpp` | Sensor implementation |
| `mqtt_client.h` | MQTT interface |
| `mqtt_client.cpp` | MQTT implementation |
| `wifi_manager.h` | Wi-Fi interface |
| `wifi_manager.cpp` | Wi-Fi implementation |
| `irrigation_control.h` | Pump/relay control |

## 4. Firmware Execution Flow

The firmware follows:

```text
BOOT
 │
 ▼
Initialize GPIO
 │
 ▼
Initialize Sensors
 │
 ▼
Connect to Wi-Fi
 │
 ▼
Connect to MQTT
 │
 ▼
Publish Device Status
 │
 ▼
┌──────────────────────────┐
│        MAIN LOOP         │
│                          │
│ Read Sensors             │
│      ↓                   │
│ Validate Measurements    │
│      ↓                   │
│ Build Telemetry Payload  │
│      ↓                   │
│ Publish MQTT             │
│      ↓                   │
│ Check Connections        │
│      ↓                   │
│ Repeat                   │
└──────────────────────────┘
```

## 5. Sensor Layer

The sensor layer abstracts hardware-specific sensor operations from the main application.

Example interface:

```cpp
float readSoilMoisture();
float readTemperature();
float readHumidity();
float readWaterLevel();
```

The exact functions should correspond to the sensors installed on the physical prototype.

## 6. Wi-Fi Configuration

Wi-Fi configuration should not be hard-coded into multiple source files.

Use a dedicated configuration file.

Example:

```cpp
#define WIFI_SSID       "YOUR_WIFI_NAME"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"
```

For local development, credentials can be stored in a local configuration file that is excluded from Git.

Do not commit real Wi-Fi credentials to GitHub.

## 7. MQTT Configuration

MQTT configuration should include:

```cpp
#define MQTT_HOST       "192.168.1.100"
#define MQTT_PORT       1883
#define MQTT_USERNAME   "YOUR_USERNAME"
#define MQTT_PASSWORD   "YOUR_PASSWORD"
```

The MQTT host must be reachable from the ESP32.

### Important

Do not use:

```text
localhost
```

for the MQTT broker when the broker is running on another computer.

Use the processing node's LAN IP address instead.

Example:

```text
ESP32
  │
  │ Wi-Fi
  ▼
192.168.1.100
  │
  └── MQTT Broker
```

## 8. MQTT Topics

The ESP32 publishes telemetry using a device-specific topic.

Recommended:

```text
irrigation/{device_id}/telemetry
```

Example:

```text
irrigation/ESP32-IRRIGATION-01/telemetry
```

Device status:

```text
irrigation/ESP32-IRRIGATION-01/status
```

Future remote-control topic:

```text
irrigation/ESP32-IRRIGATION-01/control
```

## 9. Telemetry Payload

The ESP32 should publish structured JSON telemetry.

Example:

```json
{
  "device_id": "ESP32-IRRIGATION-01",
  "timestamp": "2026-08-15T13:25:30",
  "soil_moisture": 42.0,
  "temperature": 29.4,
  "humidity": 71.0,
  "water_level": 83.0,
  "pump_status": false
}
```

The exact fields must match the backend's telemetry schema.

## 10. Publishing Cycle

The firmware should periodically execute:

```text
Read Sensors
     │
     ▼
Create Telemetry Object
     │
     ▼
Serialize JSON
     │
     ▼
MQTT Publish
     │
     ▼
Wait for Next Interval
```

Example configuration:

```cpp
#define TELEMETRY_INTERVAL_MS 5000
```

This would publish approximately every five seconds.

The actual interval should be selected according to the application's telemetry requirements.

## 11. Connection Management

### Wi-Fi

If Wi-Fi disconnects:

```text
Wi-Fi Lost
    │
    ▼
Attempt Reconnection
    │
    ├── Success → Continue
    │
    └── Failure → Retry
```

### MQTT

If MQTT disconnects:

```text
MQTT Lost
    │
    ▼
Reconnect MQTT
    │
    ├── Success → Resume Publishing
    │
    └── Failure → Retry
```

The firmware should avoid permanently blocking the main loop while attempting reconnection.

## 12. Device Status

The ESP32 can publish status information.

Example topic:

```text
irrigation/ESP32-IRRIGATION-01/status
```

Example payload:

```json
{
  "device_id": "ESP32-IRRIGATION-01",
  "status": "online"
}
```

A Last Will and Testament message can be configured so the broker can indicate an unexpected device disconnection.

## 13. Irrigation Control

If a relay-controlled pump is connected:

```text
ESP32 GPIO
    │
    ▼
Relay / Driver
    │
    ▼
Water Pump
```

The ESP32 GPIO must never directly drive a high-power pump.

The actual control circuit must use an appropriate relay, MOSFET, motor driver, or other suitable interface.

## 14. Pin Configuration

GPIO assignments are documented separately in:

```text
docs/hardware/pinout.md
```

Do not independently assign GPIO numbers in multiple documentation files.

The firmware configuration should remain synchronized with the documented hardware pinout.

## 15. Required Libraries

The exact libraries depend on the sensors and MQTT implementation.

Typical requirements may include:

```text
WiFi
PubSubClient
ArduinoJson
Sensor-specific libraries
```

Install the required libraries using the selected ESP32 development environment.

## 16. Building the Firmware

Open the firmware directory:

```bash
cd firmware/esp32
```

If using Arduino IDE:

1. Open the main `.ino` file.
2. Select the appropriate ESP32 board.
3. Select the connected serial port.
4. Configure local credentials.
5. Compile the firmware.
6. Upload it to the ESP32.
7. Open Serial Monitor.

## 17. Serial Debugging

The firmware should provide useful startup information.

Example:

```text
================================
 Smart Irrigation ESP32 Node
================================

Device ID:
ESP32-IRRIGATION-01

Initializing sensors...
Sensors initialized.

Connecting to Wi-Fi...
Wi-Fi connected.

IP Address:
192.168.1.120

Connecting to MQTT...
MQTT connected.

Publishing telemetry...
```

During operation:

```text
Soil Moisture: 42.0 %
Temperature:   29.4 C
Humidity:      71.0 %
Water Level:   83.0 %
Pump:          OFF

MQTT: Published successfully
```

## 18. Testing Procedure

Test the firmware in stages.

### Stage 1 — ESP32 Boot

Verify:

```text
ESP32 starts
↓
Serial output appears
```

### Stage 2 — Sensors

Verify each sensor individually:

```text
Sensor
  ↓
ESP32
  ↓
Serial Monitor
```

### Stage 3 — Wi-Fi

Verify:

```text
ESP32
  ↓
Wi-Fi
  ↓
IP Address
```

### Stage 4 — MQTT

Verify:

```text
ESP32
  ↓
MQTT
  ↓
Broker
  ↓
mosquitto_sub
```

### Stage 5 — Backend

Verify:

```text
ESP32
  ↓
MQTT
  ↓
Processing Node
```

### Stage 6 — Database

Verify:

```text
ESP32
  ↓
MQTT
  ↓
Backend
  ↓
Database
```

### Stage 7 — Dashboard

Finally verify:

```text
ESP32
  ↓
MQTT
  ↓
Backend
  ↓
Database
  ↓
Flask API
  ↓
Dashboard
```

## 19. Troubleshooting

### ESP32 does not connect to Wi-Fi

Check:

- SSID
- Password
- Wi-Fi availability
- Signal strength
- ESP32 power supply

### MQTT connection fails

Check:

```text
MQTT Broker IP
MQTT Port
Username
Password
Network connectivity
Firewall
```

From another machine, test:

```bash
ping <MQTT_BROKER_IP>
```

Then:

```bash
mosquitto_sub -h <MQTT_BROKER_IP> -t "irrigation/#" -v
```

### MQTT connects but no telemetry appears

Check:

```text
Sensor reading
      ↓
JSON creation
      ↓
MQTT topic
      ↓
MQTT publish result
```

### Sensor values are incorrect

Check:

- GPIO assignment
- Sensor power
- Ground connection
- ADC configuration
- Sensor calibration
- Sensor-specific library
- Physical wiring

### Backend receives MQTT but database remains empty

The problem is likely beyond the ESP32.

Debug:

```text
ESP32
  ✓
  ↓
MQTT Broker
  ✓
  ↓
MQTT Subscriber
  ✓
  ↓
Telemetry Parser
  ?
  ↓
Database Repository
  ?
  ↓
Database
  ?
```

## 20. Security

Never commit:

```text
Wi-Fi passwords
MQTT passwords
API keys
Private certificates
Production credentials
```

Use:

```text
config.example.h
```

as the public configuration template.

Keep real configuration files outside Git or add them to `.gitignore`.

## 21. Firmware Versioning

Use a firmware version constant:

```cpp
#define FIRMWARE_VERSION "0.1.0"
```

Recommended version format:

```text
MAJOR.MINOR.PATCH
```

Example:

```text
0.1.0
0.2.0
1.0.0
```

## 22. Firmware Development Checklist

Before committing firmware changes:

- [ ] Code compiles
- [ ] ESP32 boots correctly
- [ ] Wi-Fi connects
- [ ] MQTT connects
- [ ] Sensors return valid readings
- [ ] JSON payload is valid
- [ ] MQTT topic is correct
- [ ] Telemetry is received by the backend
- [ ] Database receives telemetry
- [ ] No credentials are committed
- [ ] GPIO assignments match `docs/hardware/pinout.md`
- [ ] README/documentation updated when behavior changes

## 23. Relationship With Backend

The ESP32 is the **sensing and field-control layer**.

The backend is responsible for processing and persistence.

```text
┌──────────────────────┐
│       ESP32          │
│                      │
│ Sensors              │
│ Local Processing     │
│ Wi-Fi                │
│ MQTT Publisher       │
│ Actuator Control     │
└──────────┬───────────┘
           │
           │ MQTT
           ▼
┌──────────────────────┐
│    Backend Server    │
│                      │
│ MQTT Subscriber      │
│ Validation           │
│ Database             │
│ Flask API            │
│ Dashboard             │
└──────────────────────┘
```

This separation keeps the embedded firmware lightweight and allows the backend to evolve independently.
