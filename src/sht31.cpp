#include "sht31.h"
#include "config.h"

#include "matrix.h"
#include "mqtt.h"

#define PT_USE_TIMER
#define PT_USE_SEM
#include <Adafruit_SHT31.h>
#include <Wire.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31(&Wire);

void config_sht31() {
  Wire.begin(PORT_SDA, PORT_SCL);
  if (sht31.begin(SHT31_ADDR))
    mqtt_log("SHT31 found");
  else
    mqtt_log("SHT31 not found");
}

void coroutine_sht31() {
  static long timestamp = 0;
  auto current_timestamp = millis();
  if (current_timestamp - timestamp <= SHT31_DELAY)
    return;
  timestamp = current_timestamp;

  auto t = sht31.readTemperature();
  auto h = sht31.readHumidity();

  static char buffer[256];
  if (!isnan(t) && client.connected()) {
    sprintf(buffer,
            "{\"type\": \"temperature\", \"value\": %0.1f, \"mac\": \"%s\"}", t,
            NIUBIX_MAC_ADDRESS.c_str());
    client.publish("niubix/temperature", buffer, 1);
  }
  if (!isnan(h) && client.connected()) {
    sprintf(buffer,
            "{\"type\": \"humidity\", \"value\": %0.1f, \"mac\": \"%s\"}", h,
            NIUBIX_MAC_ADDRESS.c_str());
    client.publish("niubix/humidity", buffer, 1);
  }
}
