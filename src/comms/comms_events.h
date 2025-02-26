#pragma once
#include "common/common.h"

#define COMMS_EVENTS_QUEUE_SIZE 10

typedef enum{
    COMMS_EV_OK = 0, 
    COMMS_EV_PAYLOAD_ERROR = -1,
    COMMS_EV_QUEUE_FULL = -2,
    COMMS_EV_QUEUE_EMPTY = -3 
}comms_ev_error_t;

typedef struct inMessage{
    char message[128];

}inMessage_t;

typedef struct outMessage{
    char message[128];
}outMessage_t;

class CommsOutQueue{
    public:
        static QueueHandle_t *  get_instance();
};

class CommsInQueue{
    public:
        static QueueHandle_t * get_instance();
};

typedef struct check_event{
    uint8_t rss;
    int machine_state;
    
}check_event_t;

typedef struct sleep_event{
    uint8_t rss;
    long long time;
}sleep_event_t;

typedef struct prec_event{
    std::string uid; 
}prec_event_t;

typedef struct dispense_event{
    int tap;
    float amount;
    int status;
}dispense_event_t;

comms_ev_error_t publish_check_event(check_event_t * event);
comms_ev_error_t publish_prec_event(prec_event_t * event);
comms_ev_error_t publish_dispense_event(dispense_event_t * event);
// int publish_sleep_event(sleep_event_t *event);
static comms_ev_error_t create_check_event_payload(char * buffer, ssize_t len, check_event_t * event);
static comms_ev_error_t create_prec_event_payload(char * buffer, ssize_t len, prec_event_t * event);
static comms_ev_error_t create_dispense_event_payload(char * buffer, ssize_t len, dispense_event_t * event);
