# 📐 System Architecture & Hardware Specs

This document details the high-level architecture, project workflow flowchart, data pipeline, control state machine, and hardware specs for the **ESP32 Smart Irrigation System**.

---

## 1. Block Diagram

The system operates across three main tiers: **Edge Hardware (ESP32)**, **Messaging & Backend (Mosquitto + Flask)**, and **Presentation (Web UI)**.

```mermaid
graph TD
    subgraph Edge Layer [ESP32 Microcontroller Node]
        A[Soil Moisture Probe] -->|Analog ADC| B[ESP32 Board]
        C[DHT11 Temp/Humidity] -->|Digital GPIO 4| B
        B -->|Relay Control GPIO 26| D[Water Pump Relay]
    end

    subgraph Messaging & Backend Layer [Fedora Linux Server]
        B -->|MQTT / Wi-Fi| E[Mosquitto Broker :1883]
        E -->|Publish / Subscribe| F[Python MQTT Client]
        F -->|SQL Insert| G[(SQLite DB: irrigation.db)]
        F -->|Shared Memory / API| H[Flask Web Framework :5000]
    end

    subgraph Presentation Layer
        I[Browser Dashboard] -->|Fetch API / 2s Poll| H
    end
