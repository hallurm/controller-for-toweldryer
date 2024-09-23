#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup()
{

  tft.begin();
  tft.fillScreen(TFT_BROWN);

}

void loop()
{

}