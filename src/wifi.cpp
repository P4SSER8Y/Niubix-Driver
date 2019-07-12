#include "wifi.h"
#include "config.h"

#include <ESP8266WiFi.h>

void config_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);
}
