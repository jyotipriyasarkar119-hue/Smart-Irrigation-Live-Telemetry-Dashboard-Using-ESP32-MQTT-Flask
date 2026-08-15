
# ESP32 Pinout

## 1. Overview

This document defines the GPIO assignments used by the ESP32 sensing node.

The pin assignments documented here must match the firmware configuration.

> **Important:** Replace the example GPIO assignments below with the exact GPIO numbers used by the current hardware and firmware before treating this document as the final wiring specification.

## 2. GPIO Assignment Table

| ESP32 GPIO | Function | Direction | Interface | Connected Device |
|---:|---|---|---|---|
| GPIO XX | Soil Moisture | INPUT | ADC | Soil Moisture Sensor |
| GPIO XX | Temperature/Humidity | INPUT | Digital | Temperature/Humidity Sensor |
| GPIO XX | Water Level | INPUT | ADC | Water-Level Sensor |
| GPIO XX | Pump Control | OUTPUT | Digital | Relay Module |
| GPIO XX | Status LED | OUTPUT | Digital | LED |
| 3V3 | Sensor Supply | POWER | Power | 3.3 V Sensors |
| GND | Common Ground | POWER | Power | Sensors/Modules |

## 3. Pin Allocation Diagram

```text
                    ┌───────────────────────┐
                    │         ESP32         │
                    │                       │
                    │ GPIO XX ──────────────┼── Soil Moisture
                    │                       │
                    │ GPIO XX ──────────────┼── Temperature/
                    │                       │   Humidity
                    │                       │
                    │ GPIO XX ──────────────┼── Water Level
                    │                       │
                    │ GPIO XX ──────────────┼── Relay
                    │                       │
                    │ GPIO XX ──────────────┼── Status LED
                    │                       │
                    │ 3V3 ──────────────────┼── Sensor VCC
                    │                       │
                    │ GND ──────────────────┼── Common GND
                    └───────────────────────┘
```

## 4. ADC Pins

Analog sensors require ADC-capable GPIOs.

Current ADC assignments:

```text
Soil Moisture → GPIO XX
Water Level   → GPIO XX
```

The actual GPIO values must match the ESP32 board variant and firmware.

## 5. Digital GPIO Pins

Digital sensors and control devices use digital GPIOs.

```text
Temperature/Humidity → GPIO XX
Pump Relay           → GPIO XX
Status LED           → GPIO XX
```

## 6. Power Pins

| Pin | Purpose |
|---|---|
| 3V3 | 3.3 V supply |
| GND | Ground |
| VIN/5V | Board-specific external supply input |

Always verify the exact ESP32 development board's power specifications.

## 7. GPIO Selection Rules

When assigning GPIOs:

- Verify that the selected pin supports the required interface.
- Avoid pins reserved for essential boot functions where appropriate.
- Verify ADC availability for analog sensors.
- Check input/output restrictions.
- Check boot-strapping requirements.
- Avoid conflicts with communication interfaces.
- Document every assigned GPIO.

## 8. Pin Conflict Prevention

Before assigning a new sensor:

```text
New Sensor
    │
    ▼
Required Interface?
    │
    ▼
Compatible GPIO?
    │
    ▼
Already Assigned?
 ┌──┴──┐
 │     │
NO    YES
 │     │
 ▼     ▼
Use   Select
Pin   Another Pin
```

## 9. Firmware Synchronization

The GPIO assignments in this document must remain synchronized with the firmware.

Recommended firmware organization:

```text
firmware/
└── esp32/
    └── src/
        └── config.h
```

Example:

```cpp
#define SOIL_MOISTURE_PIN   GPIO_NUMBER
#define WATER_LEVEL_PIN     GPIO_NUMBER
#define TEMP_HUMIDITY_PIN   GPIO_NUMBER
#define PUMP_RELAY_PIN      GPIO_NUMBER
#define STATUS_LED_PIN      GPIO_NUMBER
```

The documentation and firmware should always use the same assignments.

## 10. Verification Checklist

Before powering the system:

- [ ] Verify every sensor's operating voltage.
- [ ] Verify every GPIO assignment.
- [ ] Verify ADC-capable pins for analog sensors.
- [ ] Verify common ground.
- [ ] Verify relay input wiring.
- [ ] Verify pump power wiring.
- [ ] Verify no high-voltage connection reaches an ESP32 GPIO.
- [ ] Verify firmware pin definitions.
- [ ] Test sensors individually.
- [ ] Test the complete system after individual verification.

## 11. Hardware Revision History

| Version | Date | Change |
|---|---|---|
| 0.1 | 2026-08-15 | Initial hardware documentation |
|  |  |  |
|  |  |  |

## 12. Important Note

GPIO numbers in this document are placeholders until they are verified against the actual ESP32 firmware and physical wiring.

Do not use this document as the final wiring reference until those values have been confirmed.
