#include <Arduino.h>

// Juster disse konstanter efter dine behov
const int DEBOUNCE_DELAY = 50; // i millisekunder

// Struktur til at repræsentere en knap
struct Button
{
  int pin;
  unsigned long lastDebounceTime = 0;
  bool previousState = false;
  bool currentState = false;
};

// Array til at holde styr på alle knapper
Button buttons[] = {{D5, 0, false, false}};
const int numButtons = sizeof(buttons) / sizeof(buttons[0]);

// Funktionen til at opdatere tilstanden for alle knapper
void updateButtons()
{
  for (int i = 0; i < numButtons; i++)
  {
    int reading = digitalRead(buttons[i].pin);

    // Hvis der er sket en ændring i knapens tilstand
    if (reading != buttons[i].previousState)
    {
      // Opdater sidste ændringstidspunkt
      buttons[i].lastDebounceTime = millis();
    }

    // Hvis der er gået lang nok tid siden sidste ændring, opdaterer vi den aktuelle tilstand
    if ((millis() - buttons[i].lastDebounceTime) > DEBOUNCE_DELAY)
    {
      buttons[i].previousState = buttons[i].currentState;
      buttons[i].currentState = reading;

      // Hvis knappen nu er trykket
      if (buttons[i].currentState == HIGH)
      {
        // Udfør handling for knap i
        Serial.print("Knap ");
        Serial.print(i + 1);
        Serial.println(" trykket");
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);

  delay(2000);
  Serial.println("Klar...");

  // Sæt alle knap-pins som input
  for (int i = 0; i < numButtons; i++)
  {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
}

void loop()
{
  updateButtons();
  // Resten af din kode her
}