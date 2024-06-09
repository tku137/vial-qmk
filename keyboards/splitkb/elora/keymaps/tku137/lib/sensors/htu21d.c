#include QMK_KEYBOARD_H
#include "htu21d.h"
#include "i2c_master.h"
#include "print.h"
#include "timer.h"
#include <math.h>

// Static variables to hold the sensor data and state
static uint32_t       last_read_time   = 0;
static float          last_temperature = 0.0f;
static float          last_humidity    = 0.0f;
static htu21d_state_t current_state    = HTU21D_STATE_IDLE;
static uint32_t       state_start_time = 0;
static uint8_t        temp_cmd         = HTU21D_TEMP_MEASURE_NOHOLD;
static uint8_t        hum_cmd          = HTU21D_HUMIDITY_MEASURE_NOHOLD;
static uint8_t        data[3];

bool htu21d_init(void) {
    // Initialize I2C
    i2c_init();
    current_state    = HTU21D_STATE_WAIT;
    state_start_time = timer_read32();
    return true;
}

void htu21d_update(void) {
    switch (current_state) {
        case HTU21D_STATE_IDLE:
            // Do nothing
            break;

        case HTU21D_STATE_WAIT:
            // Wait for the next read interval
            if (timer_elapsed32(state_start_time) >= 1000) { // 1000 ms interval
                current_state    = HTU21D_STATE_READ_TEMP;
                state_start_time = timer_read32();
                uprintf("HTU21D: Transitioning to READ_TEMP state\n");
            }
            break;

        case HTU21D_STATE_READ_TEMP:
            // Send temperature measurement command
            if (i2c_transmit(HTU21D_ADDRESS << 1, &temp_cmd, 1, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("I2C temperature write failed\n");
                current_state = HTU21D_STATE_WAIT;
            } else {
                state_start_time = timer_read32();
                current_state    = HTU21D_STATE_WAIT_TEMP_MEASURE;
            }
            break;

        case HTU21D_STATE_WAIT_TEMP_MEASURE:
            // Wait for the sensor to perform the temperature measurement
            if (timer_elapsed32(state_start_time) >= HTU21D_TEMP_MEASURE_TIME / 1000) { // Convert us to ms
                current_state = HTU21D_STATE_READ_TEMP_DATA;
            }
            break;

        case HTU21D_STATE_READ_TEMP_DATA:
            // Read temperature data
            if (i2c_receive(HTU21D_ADDRESS << 1, data, 3, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("I2C temperature read failed\n");
                current_state = HTU21D_STATE_WAIT;
            } else {
                uint16_t raw_temp = (data[0] << 8) | data[1];
                last_temperature  = -46.85 + 175.72 * (raw_temp / 65536.0);
                uprintf("HTU21D: Temperature = %.2f C\n", last_temperature);
                current_state = HTU21D_STATE_READ_HUM;
            }
            break;

        case HTU21D_STATE_READ_HUM:
            // Send humidity measurement command
            if (i2c_transmit(HTU21D_ADDRESS << 1, &hum_cmd, 1, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("I2C humidity write failed\n");
                current_state = HTU21D_STATE_WAIT;
            } else {
                state_start_time = timer_read32();
                current_state    = HTU21D_STATE_WAIT_HUM_MEASURE;
            }
            break;

        case HTU21D_STATE_WAIT_HUM_MEASURE:
            // Wait for the sensor to perform the humidity measurement
            if (timer_elapsed32(state_start_time) >= HTU21D_HUMIDITY_MEASURE_TIME / 1000) { // Convert us to ms
                current_state = HTU21D_STATE_READ_HUM_DATA;
            }
            break;

        case HTU21D_STATE_READ_HUM_DATA:
            // Read humidity data
            if (i2c_receive(HTU21D_ADDRESS << 1, data, 3, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("I2C humidity read failed\n");
                current_state = HTU21D_STATE_WAIT;
            } else {
                uint16_t raw_hum = (data[0] << 8) | data[1];
                last_humidity    = -6.0 + 125.0 * (raw_hum / 65536.0);
                uprintf("HTU21D: Humidity = %.2f %%\n", last_humidity);
                last_read_time = timer_read32();
                current_state  = HTU21D_STATE_WAIT;
            }
            break;

        default:
            current_state = HTU21D_STATE_IDLE;
            break;
    }
}

float htu21d_get_temperature(void) {
    return last_temperature;
}

float htu21d_get_humidity(void) {
    return last_humidity;
}
