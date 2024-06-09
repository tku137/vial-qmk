#pragma once

#ifndef HTU21D_H
#    define HTU21D_H

#    include <stdbool.h>
#    include <stdint.h>

#    define HTU21D_ADDRESS 0x40
#    define HTU21D_TEMP_MEASURE_NOHOLD 0xF3
#    define HTU21D_HUMIDITY_MEASURE_NOHOLD 0xF5
#    define HTU21D_TEMP_MEASURE_TIME 50000     // 50ms in microseconds
#    define HTU21D_HUMIDITY_MEASURE_TIME 16000 // 16ms in microseconds
#    define TIMEOUT 100

// State machine states for HTU21D
typedef enum { HTU21D_STATE_IDLE, HTU21D_STATE_WAIT, HTU21D_STATE_READ_TEMP, HTU21D_STATE_WAIT_TEMP_MEASURE, HTU21D_STATE_READ_TEMP_DATA, HTU21D_STATE_READ_HUM, HTU21D_STATE_WAIT_HUM_MEASURE, HTU21D_STATE_READ_HUM_DATA } htu21d_state_t;

bool  htu21d_init(void);
void  htu21d_update(void);
float htu21d_get_temperature(void);
float htu21d_get_humidity(void);

#endif // HTU21D_H
