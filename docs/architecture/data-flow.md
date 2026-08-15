
# Data Flow

## 1. Overview

The Smart Irrigation system follows a continuous telemetry pipeline:

```text
Sensor
  ↓
ESP32
  ↓
MQTT Publish
  ↓
MQTT Broker
  ↓
MQTT Subscriber
  ↓
Data Validation
  ↓
Telemetry Processing
  ↓
Database
  ↓
Flask API
  ↓
Web Dashboard
```

## 2. Sensor Data Acquisition

The ESP32 periodically reads values from the connected sensors.

For example:

```text
Soil Moisture  →  42%
Temperature    →  29.4 °C
Humidity       →  71%
Water Level    →  83%
Pump Status    →  OFF
```

The ESP32 combines these measurements into a structured telemetry payload.

## 3. Telemetry Payload

A typical MQTT payload can use JSON:

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

## 4. MQTT Transmission

The ESP32 publishes the telemetry payload to the MQTT broker.

```text
ESP32
  │
  │ PUBLISH
  │
  ▼
MQTT Broker
```

The MQTT broker does not need to understand the meaning of the sensor values.

Its primary responsibility is message routing.

## 5. MQTT Subscription

The processing node subscribes to the required telemetry topic.

```text
MQTT Broker
      │
      │ MESSAGE
      ▼
Processing Node
```

The MQTT subscriber receives the payload whenever the ESP32 publishes new telemetry.

## 6. Message Parsing

The processing node decodes the MQTT payload.

The processing pipeline is:

```text
MQTT Message
     │
     ▼
Decode Payload
     │
     ▼
Parse JSON
     │
     ▼
Validate Fields
     │
     ▼
Normalize Data
```

Invalid messages should be rejected or logged rather than inserted into the database.

## 7. Data Validation

The processing node should validate:

- Required fields
- Data types
- Device identifier
- Sensor ranges
- Timestamp format
- MQTT topic
- Payload structure

Example:

```text
soil_moisture:
    valid range → 0–100%

humidity:
    valid range → 0–100%

temperature:
    expected numerical value
```

## 8. Database Storage

After successful validation, the telemetry record is passed to the database layer.

```text
Validated Telemetry
        │
        ▼
Database Repository
        │
        ▼
Telemetry Table
```

Each telemetry message becomes a persistent database record.

## 9. API Access

The Flask backend can retrieve stored telemetry through application services.

```text
Database
    │
    ▼
Flask Service
    │
    ▼
REST API
```

Example endpoints:

```text
GET /api/telemetry
GET /api/telemetry/latest
GET /api/devices
GET /api/health
```

## 10. Dashboard Data Flow

The dashboard requests current or historical telemetry from the Flask backend.

```text
Browser
   │
   │ HTTP Request
   ▼
Flask API
   │
   ▼
Database
   │
   ▼
Flask API
   │
   │ JSON Response
   ▼
Browser
```

The dashboard then updates the displayed values.

## 11. Complete End-to-End Flow

```text
┌─────────────┐
│   Sensors   │
└──────┬──────┘
       │
       │ Sensor Reading
       ▼
┌─────────────┐
│    ESP32    │
└──────┬──────┘
       │
       │ MQTT PUBLISH
       ▼
┌─────────────┐
│ MQTT Broker │
└──────┬──────┘
       │
       │ MQTT MESSAGE
       ▼
┌──────────────────┐
│ MQTT Subscriber  │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│ Data Validation  │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│ Telemetry Service│
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│     Database     │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│   Flask Backend  │
└────────┬─────────┘
         │
         ▼
┌──────────────────┐
│  Web Dashboard   │
└──────────────────┘
```

## 12. Failure Handling

Potential failures include:

| Failure | Expected Handling |
|---|---|
| Sensor unavailable | Mark reading invalid/log error |
| ESP32 disconnected | Device becomes offline |
| MQTT unavailable | ESP32 attempts reconnection |
| Invalid MQTT payload | Reject and log message |
| Database unavailable | Log storage failure and retry |
| API unavailable | Dashboard reports backend error |

## 13. Data Persistence Principle

The database is the persistent source for historical telemetry.

MQTT should be treated as the communication mechanism rather than the permanent storage layer.

Therefore:

```text
MQTT = Transport
Database = Persistence
Flask = Application/API
Dashboard = Presentation
```
