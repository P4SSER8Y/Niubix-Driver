#include "mqtt.h"
#include "config.h"

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

WiFiClient esp_client;
PubSubClient client(NIUBIX_SERVER, NIUBIX_MQTT_PORT, esp_client);

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
}

void mqtt_log(const char *msg)
{
  client.publish("niubix/log", msg);
}

void mqtt_reconnect()
{
  String client_id = "Niubix-driver-";
  client_id += String(random(0xffff), HEX);
  if (!client.connected())
    if (client.connect(client_id.c_str(), NIUBIX_MQTT_USER, NIUBIX_MQTT_PASSWORD))
      mqtt_log("hello");
}

void config_mqtt()
{
  client.setCallback(mqtt_callback);
  mqtt_reconnect();
}

void coroutine_mqtt()
{
  if (!client.connected())
    mqtt_reconnect();
  else
    client.loop();
}
