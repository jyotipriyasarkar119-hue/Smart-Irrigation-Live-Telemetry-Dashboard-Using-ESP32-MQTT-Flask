# Database Setup Guide

## 1. Overview

The database provides persistent storage for telemetry received from the ESP32 through MQTT.

```text
ESP32
  ↓
MQTT
  ↓
Processing Node
  ↓
Telemetry Service
  ↓
Database
```

## 2. Database Requirements

The database should support:

- Persistent telemetry storage
- Device identification
- Timestamped measurements
- Historical queries
- Dashboard queries
- Multiple ESP32 devices

## 3. Database Configuration

Database configuration should be stored in environment variables.

Example:

```text
DATABASE_HOST=localhost
DATABASE_PORT=5432
DATABASE_NAME=irrigation
DATABASE_USER=irrigation_user
DATABASE_PASSWORD=change_me
```

## 4. PostgreSQL Installation

### Fedora

```bash
sudo dnf install postgresql postgresql-server
```

Initialize the database if required:

```bash
sudo postgresql-setup --initdb
```

Start PostgreSQL:

```bash
sudo systemctl enable --now postgresql
```

Check status:

```bash
sudo systemctl status postgresql
```

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install postgresql postgresql-contrib
```

Start PostgreSQL:

```bash
sudo systemctl enable --now postgresql
```

## 5. Create Database

Enter PostgreSQL:

```bash
sudo -u postgres psql
```

Create the database:

```sql
CREATE DATABASE irrigation;
```

Create a dedicated user:

```sql
CREATE USER irrigation_user WITH PASSWORD 'CHANGE_THIS_PASSWORD';
```

Grant access:

```sql
GRANT ALL PRIVILEGES ON DATABASE irrigation TO irrigation_user;
```

Exit:

```sql
\q
```

## 6. Connect to the Database

Test:

```bash
psql -h localhost -U irrigation_user -d irrigation
```

If successful, PostgreSQL is ready.

## 7. Database Schema

The schema should be maintained in:

```text
database/
└── schema/
    └── schema.sql
```

The logical structure is:

```text
Device
  │
  ├──────────► Telemetry
  │
  └──────────► Irrigation Event
```

## 8. Example Telemetry Table

A basic telemetry table can contain:

```sql
CREATE TABLE telemetry (
    id SERIAL PRIMARY KEY,
    device_id VARCHAR(100) NOT NULL,
    timestamp TIMESTAMP NOT NULL,
    soil_moisture REAL,
    temperature REAL,
    humidity REAL,
    water_level REAL,
    pump_status BOOLEAN,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

## 9. Device Table

```sql
CREATE TABLE devices (
    id SERIAL PRIMARY KEY,
    device_id VARCHAR(100) UNIQUE NOT NULL,
    name VARCHAR(100),
    location VARCHAR(255),
    status VARCHAR(30),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

## 10. Apply the Schema

From the project root:

```bash
psql \
  -h localhost \
  -U irrigation_user \
  -d irrigation \
  -f database/schema/schema.sql
```

## 11. Verify Tables

Connect:

```bash
psql -h localhost -U irrigation_user -d irrigation
```

List tables:

```sql
\dt
```

Expected tables include:

```text
devices
telemetry
```

## 12. Test Database Insertion

Insert a test telemetry record:

```sql
INSERT INTO telemetry (
    device_id,
    timestamp,
    soil_moisture,
    temperature,
    humidity,
    water_level,
    pump_status
)
VALUES (
    'ESP32-IRRIGATION-01',
    CURRENT_TIMESTAMP,
    42.0,
    29.4,
    71.0,
    83.0,
    FALSE
);
```

Verify:

```sql
SELECT * FROM telemetry;
```

## 13. Backend Database Pipeline

The backend should follow:

```text
MQTT Message
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
Repository
     │
     ▼
INSERT
     │
     ▼
PostgreSQL
```

Database SQL should not be scattered throughout MQTT callbacks or Flask routes.

## 14. Verify Live Storage

Start the backend:

```bash
python backend/run.py
```

Start the MQTT subscriber/backend.

Then publish test telemetry:

```bash
mosquitto_pub \
  -h localhost \
  -t "irrigation/ESP32-IRRIGATION-01/telemetry" \
  -m '{"device_id":"ESP32-IRRIGATION-01","soil_moisture":42.0,"temperature":29.4,"humidity":71.0,"water_level":83.0,"pump_status":false}'
```

Check the database:

```sql
SELECT *
FROM telemetry
ORDER BY timestamp DESC
LIMIT 10;
```

A newly received telemetry record should appear.

## 15. Database Troubleshooting

### Database connection failure

Check:

```bash
sudo systemctl status postgresql
```

### Authentication failure

Verify:

```text
DATABASE_USER
DATABASE_PASSWORD
DATABASE_NAME
DATABASE_HOST
DATABASE_PORT
```

### Table does not exist

Apply:

```bash
psql -h localhost -U irrigation_user -d irrigation -f database/schema/schema.sql
```

### MQTT data arrives but database remains empty

Debug in this order:

```text
MQTT Broker
     ↓
MQTT Subscriber
     ↓
JSON Parsing
     ↓
Validation
     ↓
Telemetry Service
     ↓
Repository
     ↓
Database Connection
     ↓
INSERT
     ↓
Telemetry Table
```

Do not assume that receiving an MQTT message means the database insertion succeeded.

## 16. Database Backup

Create a backup:

```bash
pg_dump \
  -U irrigation_user \
  -d irrigation \
  > irrigation_backup.sql
```

Restore:

```bash
psql \
  -U irrigation_user \
  -d irrigation \
  < irrigation_backup.sql
```

## 17. Security

Never commit database credentials.

`.gitignore` should contain:

```text
.env
*.db
*.sqlite
*.sqlite3
```

Production deployments should also use:

- Strong database passwords
- Restricted database access
- Network/firewall rules
- Regular backups
- Separate application database users
