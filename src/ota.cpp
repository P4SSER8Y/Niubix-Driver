#include "ota.h"
#include "matrix.h"

#include <Arduino.h>
#include <ArduinoOTA.h>

bool is_updating = false;

void ota_flashing(unsigned int progress, unsigned int total)
{
  matrix->clear();
  matrix->setBrightness(5);
  matrix->setCursor(0, 7);
  matrix->setTextColor(matrix->Color(255, 0, 0));
  matrix->printf(" OTA %2d%%", 100 * progress / total);
  matrix->show();
}

void config_ota()
{
  ArduinoOTA.onStart([&]() { is_updating = true; });
  ArduinoOTA.onProgress(ota_flashing);
  ArduinoOTA.begin();
}

void coroutine_ota()
{
  do
  {
    ArduinoOTA.handle();
  } while (is_updating);
}
