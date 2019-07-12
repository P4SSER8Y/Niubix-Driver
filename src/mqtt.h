#pragma once

#include <PubSubClient.h>
extern PubSubClient client;

void config_mqtt();
void mqtt_log(const char *msg);
void coroutine_mqtt();
