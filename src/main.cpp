#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Bounce2.h>

// Pin definitions
const int DALLAS_PIN = 4;
const int BUTTON_UP_PIN = 6;
const int BUTTON_ONOFF_PIN = 7;
const int BUTTON_DOWN_PIN = 8;
const int RELAY_PIN = 5;  // Assuming we use D1 (GPIO5) for the relay

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temperature sensor setup
OneWire oneWire(DALLAS_PIN);
DallasTemperature sensors(&oneWire);

// Button debounce objects
Bounce buttonUp = Bounce();
Bounce buttonOnOff = Bounce();
Bounce buttonDown = Bounce();

// Global variables
float setTemperature = 22.0;
float currentTemperature = 0.0;
bool heaterOn = false;
bool softOn = true;

void setup() {
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
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();
  
  Serial.println("Setup complete");
}

void loop() {
  // Update button states
  buttonUp.update();
  buttonOnOff.update();
  buttonDown.update();
  
  // Handle button presses
  if (buttonUp.fell()) {
    setTemperature += 0.5;
  }
  if (buttonDown.fell()) {
    setTemperature -= 0.5;
  }
  if (buttonOnOff.fell()) {
    softOn = !softOn;
  }
  
  // Read temperature
  sensors.requestTemperatures();
  currentTemperature = sensors.getTempCByIndex(0);
  
  // Control logic
  if (softOn) {
    if (currentTemperature < setTemperature - 2) {
      heaterOn = true;
    } else if (currentTemperature > setTemperature + 1) {
      heaterOn = false;
    }
  } else {
    heaterOn = false;
  }
  
  // Control relay
  digitalWrite(RELAY_PIN, heaterOn ? HIGH : LOW);
  
  // Update display
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Set: ");
  display.print(setTemperature, 1);
  display.println(" C");
  display.print("Current: ");
  display.print(currentTemperature, 1);
  display.println(" C");
  display.print("Status: ");
  display.println(softOn ? (heaterOn ? "ON" : "Standby") : "OFF");
  display.display();
  
  delay(100);  // Small delay to prevent excessive looping
}
