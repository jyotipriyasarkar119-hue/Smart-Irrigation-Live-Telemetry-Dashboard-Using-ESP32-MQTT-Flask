# Circuit Diagram

## 1. Overview

The Smart Irrigation system uses an ESP32 as the primary sensing and control microcontroller.

The ESP32 interfaces with environmental sensors and irrigation-control hardware. Sensor measurements are processed by the ESP32 and transmitted to the processing node using MQTT over Wi-Fi.

## 2. Hardware Architecture

```text
                         ┌─────────────────────┐
                         │        ESP32         │
                         │                     │
                         │     Wi-Fi / MQTT    │
                         └──────┬───────┬──────┘
                                │       │
              ┌─────────────────┘       └─────────────────┐
              │                                           │
              ▼                                           ▼
     ┌─────────────────┐                         ┌─────────────────┐
     │ Soil Moisture   │                         │ Temperature /   │
     │ Sensor          │                         │ Humidity Sensor │
     └─────────────────┘                         └─────────────────┘
              │                                           │
              └─────────────────┬─────────────────────────┘
                                │
                                ▼
                         Sensor Readings
                                │
                                ▼
                         ESP32 Processing
                                │
                                ▼
                           MQTT / Wi-Fi
                                │
                                ▼
                         Processing Node
```

## 3. Irrigation Control

If a relay and water pump are used, the control path is:

```text
                     ESP32
                       │
                       │ GPIO
                       ▼
                ┌──────────────┐
                │ Relay Module │
                └──────┬───────┘
                       │
                  Switching
                       │
                       ▼
                ┌──────────────┐
                │ Water Pump   │
                └──────┬───────┘
                       │
                       ▼
                     Field
```

The ESP32 should not directly drive a high-power pump from a GPIO pin. A suitable relay, MOSFET driver, or motor-control stage must be used according to the pump's electrical requirements.

## 4. Power Architecture

The power system should provide appropriate supply voltages for the ESP32, sensors, and actuator circuitry.

```text
                 ┌──────────────────┐
                 │   Power Source   │
                 └────────┬─────────┘
                          │
             ┌────────────┴────────────┐
             │                         │
             ▼                         ▼
      ┌─────────────┐          ┌─────────────┐
      │ ESP32 Supply│          │ Sensor/     │
      │             │          │ Actuator    │
      └─────────────┘          │ Supply      │
                               └─────────────┘
```

The exact voltage and current requirements must be verified against the specific ESP32 board, sensors, relay, and pump used in the physical implementation.

## 5. Grounding

All low-voltage components that communicate directly with the ESP32 should share a common ground unless an isolated interface is intentionally used.

```text
ESP32 GND
    │
    ├──── Sensor GND
    ├──── Sensor GND
    └──── Relay/control GND
```

For externally powered actuators, follow the isolation and grounding requirements of the specific relay or driver circuit.

## 6. Functional Hardware Flow

```text
Sensors
   │
   ▼
ESP32
   │
   ├── Read sensor values
   ├── Validate measurements
   ├── Format telemetry
   └── Publish MQTT
          │
          ▼
      Wi-Fi Network
          │
          ▼
      MQTT Broker
```

## 7. Hardware Safety

The following precautions must be followed:

- Do not connect high-voltage loads directly to ESP32 GPIO pins.
- Use an appropriate relay or driver circuit for pumps.
- Verify sensor operating voltage before connection.
- Verify ESP32 GPIO voltage compatibility.
- Connect grounds correctly.
- Use suitable power supplies.
- Protect outdoor electronics from moisture.
- Use appropriate enclosures for field deployment.

## 8. Hardware Expansion

The architecture can later support:

- Additional soil-moisture sensors
- Rain sensor
- Water-flow sensor
- Water-level sensor
- Light sensor
- pH sensor
- EC sensor
- Multiple irrigation zones
- Multiple ESP32 sensing nodes
