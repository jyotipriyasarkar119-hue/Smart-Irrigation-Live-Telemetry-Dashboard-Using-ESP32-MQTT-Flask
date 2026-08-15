# Installation Guide

## 1. Overview

This document explains how to install and prepare the Smart Irrigation Live Telemetry Dashboard on a Linux-based development machine.

The complete software stack consists of:

```text
ESP32
   │
   ▼
MQTT Broker
   │
   ▼
Python / Flask Backend
   │
   ▼
Database
   │
   ▼
Web Dashboard
```

## 2. Prerequisites

Install the following software:

- Git
- Python 3
- Python virtual environment
- MQTT broker
- MQTT client utilities
- Database server
- ESP32 development environment

Verify Git:

```bash
git --version
```

Verify Python:

```bash
python3 --version
```

Verify pip:

```bash
python3 -m pip --version
```

## 3. Clone the Repository

Clone the repository:

```bash
git clone https://github.com/jyotipriyasarkar119-hue/Smart-Irrigation-Live-Telemetry-Dashboard-Using-ESP32-MQTT-Flask.git
```

Enter the project:

```bash
cd Smart-Irrigation-Live-Telemetry-Dashboard-Using-ESP32-MQTT-Flask
```

## 4. Create Python Virtual Environment

Create a virtual environment:

```bash
python3 -m venv .venv
```

Activate it:

```bash
source .venv/bin/activate
```

After activation, the terminal should indicate that the virtual environment is active.

## 5. Install Backend Dependencies

Navigate to the backend directory:

```bash
cd backend
```

Install dependencies:

```bash
pip install -r requirements.txt
```

Return to the project root:

```bash
cd ..
```

## 6. Environment Configuration

Create a local environment file:

```bash
cp .env.example .env
```

Configure the required values.

Example:

```text
MQTT_BROKER_HOST=localhost
MQTT_BROKER_PORT=1883
MQTT_USERNAME=
MQTT_PASSWORD=

DATABASE_HOST=localhost
DATABASE_PORT=5432
DATABASE_NAME=irrigation
DATABASE_USER=irrigation_user
DATABASE_PASSWORD=
```

Do not commit `.env` to Git.

## 7. ESP32 Environment

The ESP32 firmware requires:

- ESP32 board package
- Required sensor libraries
- MQTT library
- Wi-Fi support

Open:

```text
firmware/esp32/
```

in the selected ESP32 development environment.

Before uploading the firmware, configure:

```text
Wi-Fi SSID
Wi-Fi password
MQTT broker address
MQTT port
MQTT credentials
GPIO assignments
```

## 8. Verify MQTT Connectivity

Before starting the Flask backend, verify that the MQTT broker is reachable.

Example:

```bash
mosquitto_sub -h localhost -t "irrigation/#" -v
```

If the ESP32 is publishing correctly, MQTT messages should appear.

## 9. Initialize the Database

Follow:

```text
docs/setup/database-setup.md
```

to create and initialize the database.

## 10. Start the Backend

From the project root:

```bash
source .venv/bin/activate
```

Then:

```bash
python backend/run.py
```

The Flask application should start successfully.

## 11. Verify the Dashboard

Open the dashboard in a browser using the address printed by Flask.

Typical development configuration:

```text
http://127.0.0.1:5000
```

## 12. Verify End-to-End Communication

The complete system should now operate as:

```text
ESP32
  │
  │ Wi-Fi
  ▼
MQTT Broker
  │
  ▼
Flask Processing Node
  │
  ├──► Database
  │
  └──► Dashboard
```

## 13. Installation Verification Checklist

- [ ] Repository cloned
- [ ] Python installed
- [ ] Virtual environment created
- [ ] Backend dependencies installed
- [ ] `.env` configured
- [ ] MQTT broker running
- [ ] Database created
- [ ] ESP32 firmware configured
- [ ] ESP32 connected to Wi-Fi
- [ ] ESP32 publishing MQTT data
- [ ] Backend receiving MQTT data
- [ ] Telemetry stored in database
- [ ] Dashboard displaying telemetry

## 14. Troubleshooting

### Backend does not start

Check:

```bash
python3 --version
```

Then reinstall dependencies:

```bash
pip install -r backend/requirements.txt
```

### MQTT messages are not visible

Check the broker:

```bash
sudo systemctl status mosquitto
```

Then subscribe:

```bash
mosquitto_sub -h localhost -t "irrigation/#" -v
```

### Database contains no telemetry

Check the complete pipeline:

```text
ESP32
 ↓
MQTT
 ↓
Subscriber
 ↓
Telemetry Service
 ↓
Database Repository
 ↓
Database
```

Check backend logs for database or parsing errors.

## 15. Development Principle

The installation process should always be reproducible on a fresh machine.

Dependencies and configuration should therefore be documented rather than relying on manually installed software.
