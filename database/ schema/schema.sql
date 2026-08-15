
# Smart Irrigation Database Schema

-- ============================================================
-- Smart Irrigation & Live Telemetry System
-- Database Schema
-- ============================================================
--
-- Purpose:
--   Persistent storage for ESP32 irrigation telemetry,
--   device information, and irrigation events.
--
-- Data Flow:
--
--   ESP32
--      ↓
--   MQTT
--      ↓
--   Processing Node
--      ↓
--   Telemetry Service
--      ↓
--   PostgreSQL
--
-- ============================================================


-- ============================================================
-- 1. DEVICES
-- ============================================================

CREATE TABLE IF NOT EXISTS devices (
    id              SERIAL PRIMARY KEY,
    device_id       VARCHAR(100) NOT NULL UNIQUE,
    name            VARCHAR(100),
    location        VARCHAR(255),
    status          VARCHAR(30) NOT NULL DEFAULT 'offline',

    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT devices_status_check
        CHECK (status IN ('online', 'offline', 'unknown'))
);


-- ============================================================
-- 2. TELEMETRY
-- ============================================================

CREATE TABLE IF NOT EXISTS telemetry (
    id              BIGSERIAL PRIMARY KEY,

    device_id       VARCHAR(100) NOT NULL,

    timestamp       TIMESTAMP NOT NULL,

    soil_moisture   REAL,
    temperature     REAL,
    humidity        REAL,
    water_level     REAL,

    pump_status     BOOLEAN NOT NULL DEFAULT FALSE,

    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT fk_telemetry_device
        FOREIGN KEY (device_id)
        REFERENCES devices(device_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,

    CONSTRAINT soil_moisture_range
        CHECK (
            soil_moisture IS NULL
            OR soil_moisture >= 0
            AND soil_moisture <= 100
        ),

    CONSTRAINT humidity_range
        CHECK (
            humidity IS NULL
            OR humidity >= 0
            AND humidity <= 100
        ),

    CONSTRAINT water_level_range
        CHECK (
            water_level IS NULL
            OR water_level >= 0
            AND water_level <= 100
        )
);


-- ============================================================
-- 3. IRRIGATION EVENTS
-- ============================================================

CREATE TABLE IF NOT EXISTS irrigation_events (
    id              BIGSERIAL PRIMARY KEY,

    device_id       VARCHAR(100) NOT NULL,

    start_time      TIMESTAMP NOT NULL,

    end_time        TIMESTAMP,

    duration        INTEGER,

    trigger_source  VARCHAR(50) NOT NULL DEFAULT 'manual',

    status          VARCHAR(30) NOT NULL DEFAULT 'completed',

    created_at      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT fk_irrigation_device
        FOREIGN KEY (device_id)
        REFERENCES devices(device_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,

    CONSTRAINT irrigation_duration_check
        CHECK (
            duration IS NULL
            OR duration >= 0
        ),

    CONSTRAINT irrigation_trigger_check
        CHECK (
            trigger_source IN (
                'manual',
                'automatic',
                'scheduled',
                'soil_moisture',
                'remote'
            )
        ),

    CONSTRAINT irrigation_status_check
        CHECK (
            status IN (
                'running',
                'completed',
                'cancelled',
                'failed'
            )
        )
);


-- ============================================================
-- 4. INDEXES
-- ============================================================

-- Quickly find telemetry for a particular device.
CREATE INDEX IF NOT EXISTS idx_telemetry_device
ON telemetry(device_id);


-- Quickly retrieve telemetry chronologically.
CREATE INDEX IF NOT EXISTS idx_telemetry_timestamp
ON telemetry(timestamp);


-- Optimize queries for a device over a time period.
CREATE INDEX IF NOT EXISTS idx_telemetry_device_timestamp
ON telemetry(device_id, timestamp DESC);


-- Quickly retrieve irrigation events for a device.
CREATE INDEX IF NOT EXISTS idx_irrigation_device
ON irrigation_events(device_id);


-- Quickly retrieve recent irrigation events.
CREATE INDEX IF NOT EXISTS idx_irrigation_start_time
ON irrigation_events(start_time DESC);


-- ============================================================
-- 5. SAMPLE DEVICE
-- ============================================================
--
-- Uncomment when initializing a development database.
--
-- INSERT INTO devices (
--     device_id,
--     name,
--     location,
--     status
-- )
-- VALUES (
--     'ESP32-IRRIGATION-01',
--     'Irrigation Node 01',
--     'Field-A',
--     'offline'
-- )
-- ON CONFLICT (device_id) DO NOTHING;


-- ============================================================
-- 6. SAMPLE TELEMETRY
-- ============================================================
--
-- Uncomment only for development/testing.
--
-- INSERT INTO telemetry (
--     device_id,
--     timestamp,
--     soil_moisture,
--     temperature,
--     humidity,
--     water_level,
--     pump_status
-- )
-- VALUES (
--     'ESP32-IRRIGATION-01',
--     CURRENT_TIMESTAMP,
--     42.0,
--     29.4,
--     71.0,
--     83.0,
--     FALSE
-- );


-- ============================================================
-- 7. VERIFICATION QUERIES
-- ============================================================
--
-- List all tables:
--
-- \dt
--
-- Check devices:
--
-- SELECT * FROM devices;
--
-- Check latest telemetry:
--
-- SELECT *
-- FROM telemetry
-- ORDER BY timestamp DESC
-- LIMIT 10;
--
-- Check latest telemetry for one device:
--
-- SELECT *
-- FROM telemetry
-- WHERE device_id = 'ESP32-IRRIGATION-01'
-- ORDER BY timestamp DESC
-- LIMIT 1;
--
-- Check irrigation history:
--
-- SELECT *
-- FROM irrigation_events
-- ORDER BY start_time DESC;


-- ============================================================
-- 8. DATABASE RELATIONSHIP
-- ============================================================

-- devices
--    │
--    ├───────────────┐
--    │               │
--    ▼               ▼
-- telemetry     irrigation_events
--
-- One device can generate many telemetry records
-- and many irrigation events.


-- ============================================================
-- END OF SCHEMA
-- ============================================================
