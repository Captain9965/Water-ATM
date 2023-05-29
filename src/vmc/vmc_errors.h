#pragma once

typedef enum vmc_errors{
    VMC_OK = 0,
    VMC_ERROR_SENSORS_INIT = -1,
    VMC_ERROR_STORAGE_INIT = -2,
    VMC_ERROR_UI_INIT = -3,
    VMC_ERROR_ACTUATORS_INIT = -4,
    VMC_ERROR_SERVICES_INIT = -5,
    VMC_ERROR_SETTINGS_INIT = -6,
    VMC_ERROR_COMMS_INIT = -7,
    VMC_ERROR_MAINTASK_INIT = -8,
    VMC_ERROR_STATUS_INIT = -9,
    VMC_ERROR_INPUT_INIT = - 10,
    VMC_ERROR_UNHANDLED_STATE = -11,
}vmc_error_t;