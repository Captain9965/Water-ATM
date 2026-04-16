#pragma once
#include "common/common.h"
#include "server_event_handlers.h"
#include "A7680_MQTT.h"

void mqtt_server_event_callback(String topic, String payload);