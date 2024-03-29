#include "wifi.h"
#include "config.h"

#include <ESP8266WiFi.h>

String NIUBIX_MAC_ADDRESS = "";

void config_wifi() {
  WiFi.mode(WIFI_STA);

  String hostname = WIFI_HOSTNAME_PREFIX;
  hostname += "-";
  NIUBIX_MAC_ADDRESS = WiFi.macAddress();
  auto t = NIUBIX_MAC_ADDRESS;
  int i = 0;
  while (i < t.length())
    if (t[i] == ':')
      t.remove(i, 1);
    else
      i++;
  hostname += t;
  WiFi.hostname(hostname);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);
}
