// Description: Main program for temperature control system

// ===== Includes =====
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// EEPROM addresses
#define EEPROM_SET_TEMP 0
#define EEPROM_SOFT_ON 1
#define EEPROM_SIZE 2

// ===== Pin Definitions =====
#define DALLAS_PIN 4
#define BUTTON_UP_PIN 6
#define BUTTON_ONOFF_PIN 7
#define BUTTON_DOWN_PIN 8
#define RELAY_PIN 5  // Assuming we use D1 (GPIO5) for the relay

// ===== OLED Display Settings =====
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// ===== Temperature Sensor Setup =====
OneWire oneWire(DALLAS_PIN);
DallasTemperature sensors(&oneWire);

// ===== Button Debounce Objects =====
Bounce buttonUp = Bounce();
Bounce buttonOnOff = Bounce();
Bounce buttonDown = Bounce();

// ===== Global Variables =====
int setTemperature = 22;
int currentTemperature = 0;
bool heaterOn = false;
bool softOn = true;
bool displayNeedsUpdate = true;

// ===== WiFi and MQTT Settings =====
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;
const char* mqtt_user = "YOUR_MQTT_USERNAME";
const char* mqtt_password = "YOUR_MQTT_PASSWORD";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

// ===== EEPROM Functions =====
void saveSettings() {
  EEPROM.write(EEPROM_SET_TEMP, setTemperature);
  EEPROM.write(EEPROM_SOFT_ON, softOn ? 1 : 0);
  EEPROM.commit();
}

void loadSettings() {
  setTemperature = EEPROM.read(EEPROM_SET_TEMP);
  softOn = EEPROM.read(EEPROM_SOFT_ON) == 1;
  
  // Validate loaded values
  if (setTemperature < 0 || setTemperature > 40) {
    setTemperature = 22;  // Default value if invalid
  }
}

// ===== MQTT Functions =====
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishMQTTData() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    // Publish on/off status
    snprintf(msg, MSG_BUFFER_SIZE, "%s", softOn ? "ON" : "OFF");
    client.publish("tempcontrol/status", msg);
    
    // Publish set temperature
    snprintf(msg, MSG_BUFFER_SIZE, "%d", setTemperature);
    client.publish("tempcontrol/settemp", msg);
    
    // Publish current temperature
    snprintf(msg, MSG_BUFFER_SIZE, "%d", currentTemperature);
    client.publish("tempcontrol/currenttemp", msg);
  }
}

// ===== Setup Function =====
void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  // Initialize buttons with debounce
  buttonUp.attach(BUTTON_UP_PIN, INPUT_PULLUP);
  buttonOnOff.attach(BUTTON_ONOFF_PIN, INPUT_PULLUP);
  buttonDown.attach(BUTTON_DOWN_PIN, INPUT_PULLUP);
  buttonUp.interval(50);
  buttonOnOff.interval(50);
  buttonDown.interval(50);
  
  // Initialize temperature sensor
  sensors.begin();
  
  // Initialize OLED display
  display.begin();
  display.setFont(u8g2_font_6x10_tf);
  display.setDrawColor(1);
  display.clearBuffer();
  display.drawStr(0, 10, "Initializing...");
  display.sendBuffer();
  
  // Setup WiFi
  setup_wifi();
  
  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  
  Serial.println("Setup complete");
}

// ===== Main Loop Function =====
void loop() {
  // Update button states
  buttonUp.update();
  buttonOnOff.update();
  buttonDown.update();
  
  // Handle button presses
  if (buttonUp.fell()) {
    setTemperature += 1;
    displayNeedsUpdate = true;
    saveSettings();
  }
  if (buttonDown.fell()) {
    setTemperature -= 1;
    displayNeedsUpdate = true;
    saveSettings();
  }
  if (buttonOnOff.fell()) {
    softOn = !softOn;
    displayNeedsUpdate = true;
    saveSettings();
  }
  
  // Read temperature
  sensors.requestTemperatures();
  int newTemperature = round(sensors.getTempCByIndex(0));
  if (newTemperature != currentTemperature) {
    currentTemperature = newTemperature;
    displayNeedsUpdate = true;
  }
  
  // Control logic
  bool newHeaterState = false;
  if (softOn) {
    if (currentTemperature < setTemperature - 2) {
      newHeaterState = true;
    } else if (currentTemperature > setTemperature + 1) {
      newHeaterState = false;
    } else {
      newHeaterState = heaterOn;  // Keep previous state
    }
  }
  
  if (newHeaterState != heaterOn) {
    heaterOn = newHeaterState;
    digitalWrite(RELAY_PIN, heaterOn ? HIGH : LOW);
    displayNeedsUpdate = true;
    saveSettings();
  }
  
  // Update display if needed
  if (displayNeedsUpdate) {
    display.clearBuffer();
    display.drawStr(0, 10, ("Set: " + String(setTemperature) + " C").c_str());
    display.drawStr(0, 25, ("Current: " + String(currentTemperature) + " C").c_str());
    display.drawStr(0, 40, ("Status: " + String(softOn ? (heaterOn ? "ON" : "Standby") : "OFF")).c_str());
    display.sendBuffer();
    displayNeedsUpdate = false;
  }
  
  // Publish MQTT data
  publishMQTTData();
  
  delay(100);  // Small delay to prevent excessive looping
}
