/*
 * This is a minimal example, see extra-examples.cpp for a version
 * with more explantory documentation, example routines, how to
 * hook up your pixels and all of the pixel types that are supported.
 *
 */

#include "application.h"
#include "neopixel/neopixel.h" // use for Build IDE
// #include "neopixel.h" // use for local build

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D0
#define PIXEL_COUNT 150
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

void setup()
{
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
void loop()
{
  rainbow(20);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      byte pos = (i+j) & 255;
      uint32_t color;
      if(WheelPos < 85) {
        color = strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
      } else if(WheelPos < 170) {
        WheelPos -= 85;
        color = strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
      } else {
        WheelPos -= 170;
        color = strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      strip.setPixelColor(i, color);
    }
    strip.show();
    delay(wait);
  }
}
