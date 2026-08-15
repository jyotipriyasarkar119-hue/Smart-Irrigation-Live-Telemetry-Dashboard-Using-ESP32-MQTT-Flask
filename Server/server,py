cat << 'EOF' > server.py
import json
import sqlite3
import threading
from flask import Flask, render_template, jsonify, request
import paho.mqtt.client as mqtt

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_TOPIC_DATA = "farm/sensors/data"
MQTT_TOPIC_PUMP = "farm/pump/control"
DB_NAME = "irrigation.db"

app = Flask(__name__)

# Initialize SQLite Database automatically
def init_db():
    conn = sqlite3.connect(DB_NAME)
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS sensor_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            device_id TEXT,
            soil_adc INTEGER,
            moisture_percent INTEGER,
            temperature_c REAL,
            humidity_percent REAL,
            pump_status BOOLEAN,
            manual_override BOOLEAN
        )
    ''')
    conn.commit()
    conn.close()

def on_connect(client, userdata, flags, rc):
    print("--> Connected to Fedora Local MQTT Broker!")
    client.subscribe(MQTT_TOPIC_DATA)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode('utf-8'))
        print(f"[DATA RECEIVED] {payload}")
        
        conn = sqlite3.connect(DB_NAME)
        cursor = conn.cursor()
        cursor.execute('''
            INSERT INTO sensor_logs 
            (device_id, soil_adc, moisture_percent, temperature_c, humidity_percent, pump_status, manual_override)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (
            payload.get("device_id"),
            payload.get("soil_adc"),
            payload.get("moisture_percent"),
            payload.get("temperature_c"),
            payload.get("humidity_percent"),
            payload.get("pump_status"),
            payload.get("manual_override")
        ))
        conn.commit()
        conn.close()
    except Exception as e:
        print(f"Error parsing MQTT message: {e}")

mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

def start_mqtt():
    mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
    mqtt_client.loop_forever()

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/api/data', methods=['GET'])
def get_data():
    conn = sqlite3.connect(DB_NAME)
    cursor = conn.cursor()
    cursor.execute("SELECT timestamp, moisture_percent, temperature_c, humidity_percent, pump_status FROM sensor_logs ORDER BY id DESC LIMIT 20")
    rows = cursor.fetchall()
    conn.close()

    logs = []
    for r in reversed(rows):
        logs.append({
            "timestamp": r[0],
            "moisture": r[1],
            "temp": r[2],
            "humidity": r[3],
            "pump": r[4]
        })
    return jsonify(logs)

@app.route('/api/control', methods=['POST'])
def control_pump():
    command = request.json.get('command')
    if command in ["ON", "OFF", "AUTO"]:
        mqtt_client.publish(MQTT_TOPIC_PUMP, command)
        print(f"[CONTROL SENT] Issued pump command: {command}")
        return jsonify({"status": "success", "command": command})
    return jsonify({"status": "error"}), 400

if __name__ == '__main__':
    init_db()
    threading.Thread(target=start_mqtt, daemon=True).start()
    print("--> Web Dashboard running at: http://localhost:5000")
    app.run(host='0.0.0.0', port=5000, debug=False)
EOF
