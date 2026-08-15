#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// ================= NETWORK & MQTT CONFIGURATION =================
const char* WIFI_SSID     = "Wi-fi Name";       // Your Wi-Fi Name
const char* WIFI_PASSWORD = "Wi-fi Password";   // Your Wi-Fi Password

const char* MQTT_SERVER   = "10.30.102.192";        // Raspberry Pi Static IP Address
const int   MQTT_PORT     = 1883;                   // Default MQTT Port
const char* MQTT_CLIENT_ID= "ESP32_Irrigation_Node";

// MQTT Topics
const char* TOPIC_PUB_DATA = "farm/sensors/data";   // Topic where ESP32 publishes data
const char* TOPIC_SUB_PUMP = "farm/pump/control";   // Topic ESP32 listens to for manual pump commands

// ================= PIN DEFINITIONS =================
#define DHTPIN        4       // GPIO 4 for DHT11 Data Pin
#define DHTTYPE       DHT11   // Sensor type DHT11

#define SOIL_PIN      34      // GPIO 34 (ADC1 Input)
#define RELAY_PIN     26      // GPIO 26 for Water Pump Relay

#define LED_RED       18      // Red LED (Dry Alert / Pump Active)
#define LED_GREEN     19      // Green LED (Moisture Level Good)
#define LED_WHITE     21      // White LED (Wi-Fi & MQTT Activity Indicator)

// ================= CALIBRATION & THRESHOLDS =================
const int AIR_VALUE     = 3200;  // Raw ADC value when completely dry in air
const int WATER_VALUE   = 1300;  // Raw ADC value when submerged in water

const int DRY_THRESHOLD = 30;    // Auto-pump turns ON below 30%
const int WET_THRESHOLD = 70;    // Auto-pump turns OFF above 70%

const bool RELAY_ACTIVE_LOW = true; // Set to false if relay is Active HIGH

// ================= GLOBAL OBJECTS & VARIABLES =================
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool isPumpActive = false;
bool manualOverride = false;    // Keeps track of manual web overrides
unsigned long lastPublishTime = 0;
const long PUBLISH_INTERVAL = 5000; // Publish every 5 seconds

// Helper function to set relay state safely
void setRelayState(bool turnOn) {
  isPumpActive = turnOn;
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, turnOn ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, turnOn ? HIGH : LOW);
  }
}

// ================= WI-FI SETUP =================
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to Wi-Fi SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WHITE, !digitalRead(LED_WHITE)); // Blink White LED while connecting
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_WHITE, HIGH); // Solid White LED when connected to Wi-Fi
  Serial.println("\nWi-Fi Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT CALLBACK (Incoming Remote Commands) =================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("MQTT Command Received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Command handling from Raspberry Pi dashboard
  if (message.equalsIgnoreCase("ON")) {
    manualOverride = true;
    setRelayState(true);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    Serial.println("Pump manually turned ON via MQTT");
  } 
  else if (message.equalsIgnoreCase("OFF")) {
    manualOverride = true;
    setRelayState(false);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    Serial.println("Pump manually turned OFF via MQTT");
  } 
  else if (message.equalsIgnoreCase("AUTO")) {
    manualOverride = false;
    Serial.println("Reverted to Automatic Moisture Sensor Control");
  }
}

// ================= MQTT RECONNECT LOGIC =================
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection to broker: ");
    Serial.print(MQTT_SERVER);
    Serial.print("... ");

    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("Connected to MQTT Broker!");
      // Subscribe to control topics
      mqttClient.subscribe(TOPIC_SUB_PUMP);
      digitalWrite(LED_WHITE, HIGH);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      digitalWrite(LED_WHITE, LOW);
      delay(5000);
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // Configure Pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);

  setRelayState(false);
  dht.begin();

  // Setup Connections
  setupWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

// ================= MAIN LOOP =================
void loop() {
  // 1. Maintain Wi-Fi and MQTT Connections
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop(); // Process incoming MQTT messages

  // 2. Non-blocking Sensor Sampling & Publishing Interval
  unsigned long now = millis();
  if (now - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = now;

    // Flash White LED to indicate transmission heartbeat
    digitalWrite(LED_WHITE, LOW);

    // Read Sensors
    int rawSoil = analogRead(SOIL_PIN);
    int moisturePercent = map(rawSoil, AIR_VALUE, WATER_VALUE, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    float tempC = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(tempC)) tempC = 0.0;
    if (isnan(humidity)) humidity = 0.0;

    // Automated Logic (Only if Manual Override is inactive)
    if (!manualOverride) {
      if (moisturePercent < DRY_THRESHOLD) {
        setRelayState(true);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
      } 
      else if (moisturePercent > WET_THRESHOLD) {
        setRelayState(false);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
      }
    }

    // Construct JSON Payload
   JsonDocument doc; // Compatible with ArduinoJson v7 & v6
    doc["device_id"] = MQTT_CLIENT_ID;
    doc["soil_adc"] = rawSoil;
    doc["moisture_percent"] = moisturePercent;
    doc["temperature_c"] = tempC;
    doc["humidity_percent"] = humidity;
    doc["pump_status"] = isPumpActive;
    doc["manual_override"] = manualOverride;

    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);

    // Publish to MQTT Broker
    if (mqttClient.publish(TOPIC_PUB_DATA, jsonBuffer)) {
      Serial.print("Published JSON to [");
      Serial.print(TOPIC_PUB_DATA);
      Serial.print("]: ");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("MQTT Publish Failed!");
    }

    digitalWrite(LED_WHITE, HIGH); // Restore White LED
  }
}
