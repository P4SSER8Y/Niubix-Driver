#include <Arduino.h>

#include "config.h"
#include "wifi.h"
#include "matrix.h"
#include "ota.h"
#include "mqtt.h"
#include "sht31.h"

void setup()
{
  config_matrix();
  config_wifi();
  config_ota();
  config_mqtt();
  config_sht31();
}

void loop()
{
  coroutine_ota();
  coroutine_mqtt();
  coroutine_sht31();
}
