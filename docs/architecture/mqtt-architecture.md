
# MQTT Architecture

## 1. Overview

MQTT (Message Queuing Telemetry Transport) is used as the communication protocol between the ESP32 sensing node and the processing node.

The architecture follows the MQTT publish/subscribe model.

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

## 2. MQTT Components

The system contains three primary MQTT components.

| Component | Role |
|---|---|
| ESP32 | MQTT Publisher |
| MQTT Broker | Message Router |
| Processing Node | MQTT Subscriber |

## 3. Publisher

The ESP32 acts as the telemetry publisher.

It publishes sensor measurements at a configured interval.

Example:

```text
ESP32
   │
   ├── soil moisture
   ├── temperature
   ├── humidity
   ├── water level
   └── pump status
            │
            ▼
       MQTT Publish
```

## 4. MQTT Broker

The MQTT broker is responsible for:

- Accepting published messages
- Managing subscriptions
- Routing messages
- Managing MQTT clients
- Handling connection state

The broker does not directly process or permanently store application telemetry.

## 5. Subscriber

The processing node subscribes to the telemetry topics.

```text
             MQTT Broker
                  │
        ┌─────────┴─────────┐
        │                   │
        ▼                   ▼
 Processing Node       Other Clients
```

The processing node receives telemetry and forwards it to the backend processing pipeline.

## 6. Topic Structure

A hierarchical topic structure should be used.

Recommended structure:

```text
irrigation/
├── telemetry
├── status
├── control
└── alerts
```

For multiple devices:

```text
irrigation/{device_id}/telemetry
irrigation/{device_id}/status
irrigation/{device_id}/control
irrigation/{device_id}/alerts
```

Example:

```text
irrigation/ESP32-IRRIGATION-01/telemetry
```

## 7. Telemetry Topic

The telemetry topic carries sensor measurements.

```text
irrigation/ESP32-IRRIGATION-01/telemetry
```

Example payload:

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

## 8. Status Topic

The status topic can report device state.

Example:

```text
irrigation/ESP32-IRRIGATION-01/status
```

Payload:

```json
{
  "device_id": "ESP32-IRRIGATION-01",
  "status": "online",
  "uptime": 5420
}
```

## 9. Control Topic

The control topic can be used for future remote irrigation control.

```text
irrigation/ESP32-IRRIGATION-01/control
```

Example:

```json
{
  "command": "pump_on",
  "duration": 30
}
```

The ESP32 can subscribe to this topic if remote pump control is implemented.

## 10. MQTT QoS

MQTT provides three Quality of Service levels.

| QoS | Meaning | Recommended Use |
|---|---|---|
| 0 | At most once | High-frequency telemetry |
| 1 | At least once | Important telemetry/control |
| 2 | Exactly once | Critical transactions |

For this project, QoS 0 or QoS 1 can be selected depending on whether occasional telemetry loss is acceptable.

## 11. Retained Messages

Retained messages can be useful for device status.

For example:

```text
irrigation/ESP32-IRRIGATION-01/status
```

can retain:

```json
{
  "status": "online"
}
```

A new subscriber can then immediately receive the latest retained status.

## 12. Last Will and Testament

The ESP32 can configure an MQTT Last Will message.

If the ESP32 unexpectedly disconnects, the broker can publish:

```json
{
  "device_id": "ESP32-IRRIGATION-01",
  "status": "offline"
}
```

This allows the dashboard to identify disconnected devices.

## 13. MQTT Security

For deployment, MQTT should use:

- Username/password authentication
- TLS encryption
- Unique device credentials
- Restricted topic permissions
- Non-default broker credentials

Credentials must not be committed to GitHub.

Use environment/configuration files instead.

## 14. MQTT Processing Pipeline

```text
ESP32
  │
  │ Publish
  ▼
MQTT Broker
  │
  │ Topic Match
  ▼
MQTT Subscriber
  │
  ▼
Payload Decoder
  │
  ▼
JSON Parser
  │
  ▼
Validation
  │
  ▼
Telemetry Service
  │
  ▼
Database
```

## 15. MQTT Reconnection

The ESP32 should automatically attempt to reconnect if:

```text
Wi-Fi connection lost
        OR
MQTT connection lost
```

Recommended sequence:

```text
Connection Lost
      │
      ▼
Reconnect Wi-Fi
      │
      ▼
Reconnect MQTT
      │
      ▼
Resubscribe
      │
      ▼
Resume Publishing
```

## 16. Design Principle

MQTT is responsible for **transporting messages**.

It should not be treated as the project's permanent telemetry database.

```text
ESP32
   │
   │ MQTT
   ▼
Broker
   │
   ▼
Processing Node
   │
   ▼
Database
```
