# System Architecture

## 1. Overview

The Smart Irrigation Live Telemetry Dashboard is an IoT-based monitoring and irrigation-control system built around ESP32, MQTT, a Flask processing server, a database, and a web dashboard.

The system continuously collects environmental and irrigation-related telemetry from the field, transmits the data using MQTT, processes the incoming data on the processing node, stores telemetry in a database, and presents the information through a live web dashboard.

## 2. System Components

The system consists of the following major components:

| Component | Responsibility |
|---|---|
| ESP32 Sensing Node | Collects sensor data from the field |
| Sensors | Measure soil and environmental conditions |
| MQTT Broker | Transfers telemetry between devices |
| Processing Node | Receives, validates, processes, and stores telemetry |
| Database | Provides persistent telemetry storage |
| Flask Backend | Provides APIs and dashboard services |
| Web Dashboard | Displays live and historical telemetry |

## 3. High-Level Architecture

```text
                    ┌─────────────────────────┐
                    │      FIELD SENSORS      │
                    │                         │
                    │ Soil Moisture           │
                    │ Temperature             │
                    │ Humidity                │
                    │ Water Level              │
                    │ Other Sensors            │
                    └────────────┬────────────┘
                                 │
                                 ▼
                    ┌─────────────────────────┐
                    │       ESP32 NODE        │
                    │                         │
                    │ Sensor Acquisition      │
                    │ Data Formatting         │
                    │ Wi-Fi Communication     │
                    │ MQTT Publishing         │
                    └────────────┬────────────┘
                                 │
                              MQTT
                                 │
                                 ▼
                    ┌─────────────────────────┐
                    │      MQTT BROKER        │
                    │                         │
                    │ Topic Routing           │
                    │ Message Distribution    │
                    └────────────┬────────────┘
                                 │
                              MQTT
                                 │
                                 ▼
                    ┌─────────────────────────┐
                    │    PROCESSING NODE      │
                    │                         │
                    │ MQTT Subscriber         │
                    │ Data Validation         │
                    │ Telemetry Processing    │
                    └────────────┬────────────┘
                                 │
                    ┌────────────┴────────────┐
                    │                         │
                    ▼                         ▼
          ┌─────────────────┐       ┌─────────────────┐
          │    DATABASE     │       │  FLASK BACKEND  │
          │                 │       │                 │
          │ Telemetry       │       │ REST/API Layer  │
          │ Historical Data │       │ Dashboard       │
          └─────────────────┘       └────────┬────────┘
                                             │
                                             ▼
                                  ┌────────────────────┐
                                  │   WEB DASHBOARD    │
                                  │                    │
                                  │ Live Telemetry     │
                                  │ Sensor Status      │
                                  │ Irrigation Status  │
                                  └────────────────────┘
```

## 4. Sensing Layer

The sensing layer consists of the ESP32 and connected sensors.

The ESP32 periodically reads sensor values and converts them into structured telemetry data.

Typical telemetry parameters include:

- Soil moisture
- Temperature
- Relative humidity
- Water level
- Pump status
- Sensor/device status
- Timestamp
- Device identifier

## 5. Communication Layer

MQTT is used as the primary communication protocol between the ESP32 sensing node and the processing node.

The ESP32 publishes telemetry messages to predefined MQTT topics.

The MQTT broker receives the messages and forwards them to subscribed clients.

## 6. Processing Layer

The processing node receives MQTT messages and performs:

1. MQTT message reception
2. Payload decoding
3. JSON parsing
4. Data validation
5. Telemetry normalization
6. Database insertion
7. Dashboard/API data availability

The processing layer acts as the central software component connecting the IoT network with the application layer.

## 7. Storage Layer

The database provides persistent storage for telemetry.

Each received telemetry record should contain sufficient information to identify:

- The device
- Measurement timestamp
- Sensor values
- Irrigation state
- Data source

Historical records allow the system to analyze changes in field conditions over time.

## 8. Application Layer

The Flask backend provides the application interface.

Its responsibilities include:

- Serving the dashboard
- Providing telemetry APIs
- Reading data from the database
- Providing system-health endpoints
- Handling dashboard requests

## 9. Presentation Layer

The web dashboard provides a human-readable interface for monitoring the irrigation system.

The dashboard can display:

- Current sensor readings
- Historical telemetry
- Soil moisture trends
- Temperature trends
- Humidity trends
- Water-level information
- Pump status
- Device connectivity

## 10. Design Principles

The system follows these principles:

- Modular architecture
- Separation of sensing and processing
- MQTT-based asynchronous communication
- Persistent telemetry storage
- API-based application access
- Expandable sensor architecture
- Clear separation between hardware and software

## 11. Future Expansion

The architecture can be extended with:

- Multiple ESP32 sensing nodes
- Automated irrigation control
- Authentication
- Cloud database integration
- Machine-learning-based irrigation prediction
- Weather-data integration
- Mobile application
- Alert and notification services
- Remote firmware updates
