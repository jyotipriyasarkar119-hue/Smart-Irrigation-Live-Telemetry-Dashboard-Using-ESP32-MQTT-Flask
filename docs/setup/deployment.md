# Deployment Guide

## 1. Overview

This document describes how to move the Smart Irrigation system from development into a continuously running deployment.

The deployment architecture is:

```text
                  ┌────────────────────┐
                  │       ESP32        │
                  │   Sensing Node     │
                  └─────────┬──────────┘
                            │
                           Wi-Fi
                            │
                            ▼
                  ┌────────────────────┐
                  │   MQTT Broker      │
                  └─────────┬──────────┘
                            │
                            ▼
                  ┌────────────────────┐
                  │ Processing Server  │
                  │                    │
                  │ MQTT Subscriber    │
                  │ Flask Backend      │
                  │ Telemetry Service  │
                  └──────┬───────┬─────┘
                         │       │
                         ▼       ▼
                  ┌──────────┐ ┌──────────┐
                  │Database  │ │Dashboard │
                  └──────────┘ └──────────┘
```

## 2. Deployment Components

| Component | Deployment Role |
|---|---|
| ESP32 | Field sensing node |
| MQTT Broker | Message transport |
| Flask | Backend/API |
| Database | Persistent storage |
| Web Server | Dashboard delivery |
| Linux Server | Processing host |

## 3. Server Requirements

A development deployment can run on a Linux PC.

Recommended software:

```text
Linux
Python 3
Mosquitto
PostgreSQL
Flask
Git
```

For larger deployments, a dedicated server or cloud VM can be used.

## 4. Production Configuration

Do not use development credentials in production.

Configure:

```text
MQTT_BROKER_HOST
MQTT_BROKER_PORT
MQTT_USERNAME
MQTT_PASSWORD

DATABASE_HOST
DATABASE_PORT
DATABASE_NAME
DATABASE_USER
DATABASE_PASSWORD

FLASK_SECRET_KEY
```

Store them outside source control.

## 5. Install the Project

Clone:

```bash
git clone https://github.com/jyotipriyasarkar119-hue/Smart-Irrigation-Live-Telemetry-Dashboard-Using-ESP32-MQTT-Flask.git
```

Enter:

```bash
cd Smart-Irrigation-Live-Telemetry-Dashboard-Using-ESP32-MQTT-Flask
```

Create the virtual environment:

```bash
python3 -m venv .venv
```

Activate:

```bash
source .venv/bin/activate
```

Install dependencies:

```bash
pip install -r backend/requirements.txt
```

## 6. Configure MQTT

Ensure Mosquitto is running:

```bash
sudo systemctl enable --now mosquitto
```

Verify:

```bash
sudo systemctl status mosquitto
```

Configure the backend to connect to the broker.

## 7. Configure Database

Create and initialize the database according to:

```text
docs/setup/database-setup.md
```

Verify:

```bash
psql -h localhost -U irrigation_user -d irrigation
```

## 8. Start the Backend

For development:

```bash
python backend/run.py
```

For production, the Flask development server should not be used as the primary public-facing server.

Use a production WSGI server such as Gunicorn behind a reverse proxy.

Example:

```bash
gunicorn \
  --workers 2 \
  --bind 127.0.0.1:8000 \
  backend.run:app
```

Adjust the module path according to the actual Flask application structure.

## 9. Reverse Proxy

A reverse proxy can expose the dashboard while keeping the Flask process bound locally.

Conceptually:

```text
Internet / LAN
      │
      ▼
Reverse Proxy
      │
      ▼
Gunicorn
      │
      ▼
Flask
```

Nginx is one possible reverse-proxy implementation.

## 10. Process Management

The backend should automatically restart if the server reboots or the application crashes.

A service manager such as `systemd` can be used.

Example service concept:

```text
smart-irrigation.service
```

The service should:

- Start the backend
- Run under a dedicated user
- Restart on failure
- Start after networking is available
- Load environment configuration

## 11. MQTT Service

Mosquitto should start automatically:

```bash
sudo systemctl enable mosquitto
```

Check:

```bash
sudo systemctl status mosquitto
```

## 12. Database Service

PostgreSQL should start automatically:

```bash
sudo systemctl enable postgresql
```

Check:

```bash
sudo systemctl status postgresql
```

## 13. ESP32 Deployment

Before deploying the ESP32 to the field:

- Upload the verified firmware.
- Configure Wi-Fi.
- Configure MQTT broker address.
- Configure MQTT credentials.
- Verify GPIO assignments.
- Test every sensor.
- Test pump-control logic.
- Verify reconnect behavior.

The ESP32 should connect:

```text
ESP32
  │
  ├── Connect Wi-Fi
  │
  ├── Connect MQTT
  │
  ├── Publish status
  │
  └── Publish telemetry
```

## 14. Network Architecture

For a local deployment:

```text
                 Local Network
                      │
        ┌─────────────┼─────────────┐
        │             │             │
        ▼             ▼             ▼
      ESP32       Processing      Laptop/
                   Server         Browser
                     │
              ┌──────┴──────┐
              │             │
              ▼             ▼
            MQTT         Database
            Broker
```

The ESP32 must be able to reach the MQTT broker over the network.

## 15. Production Security

For deployment outside a trusted local network:

- Enable MQTT authentication.
- Use TLS for MQTT.
- Use strong passwords.
- Restrict MQTT topics.
- Protect the database from public access.
- Use HTTPS for the dashboard.
- Use firewall rules.
- Do not expose PostgreSQL directly to the Internet.
- Do not expose MQTT anonymously to the Internet.
- Keep `.env` outside Git.

## 16. Monitoring

Monitor the following components:

```text
ESP32
  │
  ├── Wi-Fi status
  ├── MQTT status
  ├── Sensor status
  └── Uptime
       │
       ▼
MQTT Broker
  │
  ├── Connected clients
  └── Message traffic
       │
       ▼
Backend
  │
  ├── MQTT connection
  ├── Database connection
  └── API health
       │
       ▼
Database
  │
  └── Telemetry growth
```

## 17. Health Check

The backend should expose a health endpoint:

```text
GET /api/health
```

Example response:

```json
{
  "status": "ok",
  "mqtt": "connected",
  "database": "connected"
}
```

## 18. Deployment Verification

After deployment, verify the complete pipeline:

```text
ESP32
  ↓
Wi-Fi
  ↓
MQTT Broker
  ↓
MQTT Subscriber
  ↓
Telemetry Service
  ↓
Database
  ↓
Flask API
  ↓
Dashboard
```

Check each layer independently.

## 19. Deployment Checklist

- [ ] Server configured
- [ ] Repository cloned
- [ ] Python environment created
- [ ] Dependencies installed
- [ ] Environment variables configured
- [ ] MQTT broker running
- [ ] Database running
- [ ] Database schema initialized
- [ ] Backend running
- [ ] ESP32 firmware uploaded
- [ ] ESP32 connected to Wi-Fi
- [ ] ESP32 connected to MQTT
- [ ] MQTT telemetry received
- [ ] Database records created
- [ ] Dashboard displays telemetry
- [ ] Health endpoint working
- [ ] Backups configured
- [ ] Security configuration reviewed

## 20. Operational Principle

The deployed system should operate continuously:

```text
             ┌──────────────┐
             │    ESP32     │
             └──────┬───────┘
                    │
                 Telemetry
                    │
                    ▼
             ┌──────────────┐
             │ MQTT Broker  │
             └──────┬───────┘
                    │
                    ▼
             ┌──────────────┐
             │   Backend    │
             └──────┬───────┘
                    │
             ┌──────┴───────┐
             │              │
             ▼              ▼
        ┌──────────┐   ┌───────────┐
        │ Database │   │ Dashboard │
        └──────────┘   └───────────┘
```

The deployment should be considered successful only when telemetry can travel from the physical sensor to persistent database storage and finally be displayed through the dashboard.
