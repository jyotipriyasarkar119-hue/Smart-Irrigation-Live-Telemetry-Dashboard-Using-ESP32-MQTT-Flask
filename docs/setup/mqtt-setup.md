# MQTT Setup Guide

## 1. Overview

MQTT provides communication between the ESP32 sensing node and the processing node.

```text
ESP32
  │
  │ Publish
  ▼
MQTT Broker
  │
  │ Subscribe
  ▼
Processing Node
```

## 2. Install Mosquitto

On Fedora:

```bash
sudo dnf install mosquitto mosquitto-clients
```

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install mosquitto mosquitto-clients
```

## 3. Start the MQTT Broker

Start Mosquitto:

```bash
sudo systemctl start mosquitto
```

Enable it at system startup:

```bash
sudo systemctl enable mosquitto
```

Check its status:

```bash
sudo systemctl status mosquitto
```

## 4. Test the Broker

Open Terminal 1 and subscribe:

```bash
mosquitto_sub -h localhost -t "irrigation/test" -v
```

Open Terminal 2 and publish:

```bash
mosquitto_pub -h localhost -t "irrigation/test" -m "MQTT_TEST"
```

Terminal 1 should display:

```text
irrigation/test MQTT_TEST
```

## 5. Configure MQTT Topics

Recommended topic structure:

```text
irrigation/
├── {device_id}/
│   ├── telemetry
│   ├── status
│   ├── control
│   └── alerts
```

Example:

```text
irrigation/ESP32-IRRIGATION-01/telemetry
```

## 6. Telemetry Topic

The ESP32 publishes sensor readings to:

```text
irrigation/ESP32-IRRIGATION-01/telemetry
```

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

## 7. Subscribe to Telemetry

Run:

```bash
mosquitto_sub \
  -h localhost \
  -t "irrigation/+/telemetry" \
  -v
```

This subscribes to telemetry from all irrigation devices following the topic pattern.

## 8. Test Telemetry Manually

Publish test data:

```bash
mosquitto_pub \
  -h localhost \
  -t "irrigation/ESP32-IRRIGATION-01/telemetry" \
  -m '{"device_id":"ESP32-IRRIGATION-01","soil_moisture":42.0,"temperature":29.4,"humidity":71.0,"water_level":83.0,"pump_status":false}'
```

The subscriber should receive the message.

## 9. Configure Backend

The backend should obtain MQTT configuration from environment variables.

Example:

```text
MQTT_BROKER_HOST=localhost
MQTT_BROKER_PORT=1883
MQTT_USERNAME=
MQTT_PASSWORD=
MQTT_TELEMETRY_TOPIC=irrigation/+/telemetry
```

## 10. ESP32 MQTT Configuration

The ESP32 requires:

```text
Wi-Fi SSID
Wi-Fi Password
MQTT Broker IP/Hostname
MQTT Port
MQTT Username
MQTT Password
MQTT Topic
```

Example:

```text
MQTT Broker:
192.168.1.100

MQTT Port:
1883

Topic:
irrigation/ESP32-IRRIGATION-01/telemetry
```

The broker address must be the IP address reachable by the ESP32, not necessarily `localhost`.

## 11. Localhost vs LAN

Important:

```text
localhost
```

means the current machine.

Therefore, if Mosquitto is running on the processing node:

```text
ESP32 ──────────────► Processing Node
                         │
                         └── MQTT Broker
```

the ESP32 should use the processing node's LAN IP address.

Example:

```text
192.168.1.100
```

rather than:

```text
localhost
```

## 12. MQTT Authentication

For a development-only local network, anonymous access may be used temporarily.

For deployment, configure:

* Username
* Password
* Access control
* TLS
* Restricted topics

Never commit credentials to GitHub.

## 13. MQTT Troubleshooting

Check broker:

```bash
sudo systemctl status mosquitto
```

Check listening port:

```bash
ss -lntp | grep 1883
```

Check all irrigation messages:

```bash
mosquitto_sub -h localhost -t "irrigation/#" -v
```

Test publishing:

```bash
mosquitto_pub -h localhost -t "irrigation/test" -m "hello"
```

## 14. MQTT Verification Checklist

* [ ] Mosquitto installed
* [ ] Broker running
* [ ] Port accessible
* [ ] Publisher connected
* [ ] Subscriber connected
* [ ] Telemetry topic correct
* [ ] JSON payload valid
* [ ] Backend receiving messages
* [ ] Database receiving processed telemetry

