#pragma once
#include "common/common.h"
#include "comms_events.h"
#include "sensors/rtc/rtc.h"

void handle_server_side_event(char* event, size_t len);