#include "matrix.h"
#include "config.h"

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <Fonts/TomThumb.h>

CRGB leds[8 * 32];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 32, 8,
                                                  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

void config_matrix()
{
  FastLED.addLeds<NEOPIXEL, PORT_MATRIX>(leds, 256).setCorrection(TypicalLEDStrip);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(5);
  matrix->setFont(&TomThumb);
  matrix->setCursor(7, 6);
  matrix->print("hello");
  matrix->show();
}