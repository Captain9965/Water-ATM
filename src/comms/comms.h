#pragma once
#include "A7680_MQTT.h"
#include "mqtt_callback.h"
#include "comms_events.h"

typedef enum {
    COMMS_STATE_HIGH_FREQUENCY = 0,
    COMMS_STATE_LOW_FREQUENCY,
    COMMS_STATE_OFF = -1
}comms_state_t;

class Comms {
public:
    Comms();
    void init(void);
    void update(void);
    static Comms* get_instance(void);
    int16_t get_rss(void);
private:
    comms_ev_error_t dispatch_comms_queue();
};
