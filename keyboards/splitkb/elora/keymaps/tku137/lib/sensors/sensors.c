#include QMK_KEYBOARD_H
#include "sensors.h"
#include "ccs811.h"
#include "htu21d.h"
#include "i2c_master.h"
#include "print.h"
#include "timer.h"

// Static variables to hold the sensor data and state
static sensor_state_t current_state    = SENSOR_STATE_IDLE;
static uint32_t       state_start_time = 0;

bool sensors_init(void) {
    // Initialize HTU21D and CCS811
    if (!htu21d_init() || !ccs811_init()) {
        uprintf("Sensor initialization failed\n");
        return false;
    }
    current_state    = SENSOR_STATE_WAIT;
    state_start_time = timer_read32();
    return true;
}

void sensors_update(void) {
    switch (current_state) {
        case SENSOR_STATE_IDLE:
            // Do nothing
            break;

        case SENSOR_STATE_WAIT:
            // Wait for the next read interval
            if (timer_elapsed32(state_start_time) >= 1000) { // 1000 ms interval
                current_state    = SENSOR_STATE_READ_HTU21D;
                state_start_time = timer_read32();
                uprintf("Sensors: Transitioning to READ_HTU21D state\n");
            }
            break;

        case SENSOR_STATE_READ_HTU21D:
            // Read HTU21D data
            htu21d_update();
            if (timer_elapsed32(state_start_time) >= 100) { // Assume 100 ms for HTU21D to read
                current_state    = SENSOR_STATE_READ_CCS811;
                state_start_time = timer_read32();
                uprintf("Sensors: Transitioning to READ_CCS811 state\n");
            }
            break;

        case SENSOR_STATE_READ_CCS811:
            // Read CCS811 data
            ccs811_update();
            if (timer_elapsed32(state_start_time) >= 100) { // Assume 100 ms for CCS811 to read
                current_state    = SENSOR_STATE_WAIT;
                state_start_time = timer_read32();
                uprintf("Sensors: Transitioning to WAIT state\n");
            }
            break;

        default:
            current_state = SENSOR_STATE_IDLE;
            break;
    }
}

float get_last_temperature(void) {
    return htu21d_get_temperature();
}

float get_last_humidity(void) {
    return htu21d_get_humidity();
}

uint16_t get_last_co2(void) {
    return ccs811_get_co2();
}

uint16_t get_last_tvoc(void) {
    return ccs811_get_tvoc();
}
