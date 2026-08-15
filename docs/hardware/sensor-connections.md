# Sensor Connections

## 1. Overview

This document describes the sensors used by the ESP32 sensing node, their purpose, interface type, electrical requirements, and connection methodology.

The exact GPIO assignment is maintained separately in `pinout.md`.

## 2. Sensor Inventory

| Sensor | Measurement | Interface | ESP32 Connection |
|---|---|---|---|
| Soil Moisture Sensor | Soil moisture | Analog | ADC GPIO |
| Temperature/Humidity Sensor | Temperature + humidity | Digital | GPIO |
| Water-Level Sensor | Water level | Analog/Digital | GPIO/ADC |
| Relay Module | Pump control | Digital | GPIO |

> Replace or extend this table according to the exact hardware installed in your current prototype.

## 3. Soil Moisture Sensor

### Purpose

The soil moisture sensor measures the moisture level of the soil.

The ESP32 reads the sensor output and converts it into a usable telemetry value.

### Interface

Typical interface:

```text
VCC
GND
AO
```

Connection concept:

```text
Soil Moisture Sensor       ESP32
────────────────────────────────
VCC                       → 3.3V*
GND                       → GND
AO                        → ADC GPIO
```

`*` Use the voltage recommended by the exact sensor module being used.

### Data Flow

```text
Soil
  │
  ▼
Moisture Sensor
  │
  │ Analog Signal
  ▼
ESP32 ADC
  │
  ▼
Calibration / Conversion
  │
  ▼
Soil Moisture %
```

## 4. Temperature and Humidity Sensor

### Purpose

The temperature/humidity sensor provides environmental measurements.

Typical measurements:

- Temperature
- Relative humidity

### Connection

```text
Temperature/Humidity Sensor     ESP32
─────────────────────────────────────
VCC                             → Supply
GND                             → GND
DATA                            → Digital GPIO
```

The exact supply voltage and data-line requirements depend on the sensor model.

## 5. Water-Level Sensor

### Purpose

The water-level sensor monitors the amount of water available in the irrigation reservoir.

### Connection

Typical analog configuration:

```text
Water-Level Sensor       ESP32
───────────────────────────────
VCC                     → Supply
GND                     → GND
Signal                  → ADC GPIO
```

### Data Flow

```text
Water Reservoir
      │
      ▼
Water-Level Sensor
      │
      ▼
ESP32 ADC
      │
      ▼
Water-Level Calculation
      │
      ▼
Telemetry
```

## 6. Relay Module

### Purpose

The relay provides electrical switching between the ESP32 control signal and the irrigation pump.

### Connection

```text
ESP32                    Relay Module
─────                    ────────────
GPIO                     → IN
GND                      → GND
Supply                   → VCC
```

The pump's power circuit must be connected according to the relay module and pump specifications.

## 7. Sensor Data Processing

The ESP32 performs the following sequence:

```text
Read Sensor
     │
     ▼
Convert Raw Value
     │
     ▼
Apply Calibration
     │
     ▼
Validate Reading
     │
     ▼
Create Telemetry Payload
     │
     ▼
Publish Through MQTT
```

## 8. Sensor Error Handling

The firmware should account for:

- Sensor disconnection
- Invalid readings
- Out-of-range values
- ADC errors
- Communication failures
- Sensor startup failures

Example:

```text
Sensor Reading
      │
      ▼
Valid?
 ┌────┴────┐
 │         │
YES        NO
 │         │
 ▼         ▼
Process   Log Error
 │         │
 ▼         ▼
MQTT      Retry
```

## 9. Calibration

Sensor-specific calibration should be documented here when implemented.

For example:

```text
Raw ADC Value
      │
      ▼
Calibration Formula
      │
      ▼
Normalized Value
      │
      ▼
Physical Measurement
```

Calibration constants should preferably be stored in the firmware configuration rather than hard-coded throughout the application.

## 10. Adding a New Sensor

To add a new sensor:

1. Define the sensor interface.
2. Select an appropriate ESP32 GPIO.
3. Verify voltage compatibility.
4. Add the pin assignment to `pinout.md`.
5. Implement sensor-reading code.
6. Add validation.
7. Add the value to the telemetry payload.
8. Update the database schema if required.
9. Update the dashboard.
10. Update this documentation.
