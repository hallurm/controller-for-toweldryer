// Description: Main program for temperature control system

// ===== Includes =====
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Bounce2.h>

// ===== Pin Definitions =====
const int DALLAS_PIN = 4;
const int BUTTON_UP_PIN = 6;
const int BUTTON_ONOFF_PIN = 7;
const int BUTTON_DOWN_PIN = 8;
const int RELAY_PIN = 5;  // Assuming we use D1 (GPIO5) for the relay

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

// ===== Setup Function =====
void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
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
  }
  if (buttonDown.fell()) {
    setTemperature -= 1;
    displayNeedsUpdate = true;
  }
  if (buttonOnOff.fell()) {
    softOn = !softOn;
    displayNeedsUpdate = true;
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
  
  delay(100);  // Small delay to prevent excessive looping
}
