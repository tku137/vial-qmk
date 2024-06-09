#include QMK_KEYBOARD_H
#include "ccs811.h"
#include "i2c_master.h"
#include "print.h"
#include "timer.h"

// Static variables to hold the sensor data and state
static uint32_t       last_read_time   = 0;
static uint16_t       last_co2         = 0;
static uint16_t       last_tvoc        = 0;
static ccs811_state_t current_state    = CCS811_STATE_IDLE;
static uint32_t       state_start_time = 0;

bool ccs811_init(void) {
    // Initialize I2C
    i2c_init();

    // Send initialization commands to CCS811
    uint8_t app_start_cmd = CCS811_APP_START;
    if (i2c_transmit(CCS811_ADDRESS << 1, &app_start_cmd, 1, I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
        uprintf("CCS811 initialization failed\n");
        return false;
    }

    // Set measurement mode
    uint8_t meas_mode_cmd[] = {CCS811_MEAS_MODE, 0x10}; // 0x10 sets constant power mode, measurement every second
    if (i2c_transmit(CCS811_ADDRESS << 1, meas_mode_cmd, 2, I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
        uprintf("CCS811 set measurement mode failed\n");
        return false;
    }

    current_state    = CCS811_STATE_WAIT;
    state_start_time = timer_read32();

    return true;
}

void ccs811_update(void) {
    switch (current_state) {
        case CCS811_STATE_IDLE:
            // Do nothing
            break;

        case CCS811_STATE_WAIT:
            // Wait for the next read interval
            if (timer_elapsed32(state_start_time) >= 1000) { // 1000 ms interval
                current_state    = CCS811_STATE_READ;
                state_start_time = timer_read32();
                uprintf("CCS811: Transitioning to READ state\n");
            }
            break;

        case CCS811_STATE_READ: {
            // Read sensor data
            uint8_t data[5];

            uint8_t status_cmd = CCS811_ALG_RESULT_DATA;
            if (i2c_transmit(CCS811_ADDRESS << 1, &status_cmd, 1, I2C_TIMEOUT) == I2C_STATUS_SUCCESS && i2c_receive(CCS811_ADDRESS << 1, data, 5, I2C_TIMEOUT) == I2C_STATUS_SUCCESS) {
                // Parse sensor data
                last_co2  = (data[0] << 8) | data[1];
                last_tvoc = (data[2] << 8) | data[3];

                uprintf("CCS811: CO2 = %d ppm, TVOC = %d ppb\n", last_co2, last_tvoc);

                last_read_time = timer_read32();
            } else {
                uprintf("CCS811: Read failed\n");
            }

            current_state = CCS811_STATE_WAIT;
            break;
        }

        default:
            current_state = CCS811_STATE_IDLE;
            break;
    }
}

uint16_t ccs811_get_co2(void) {
    return last_co2;
}

uint16_t ccs811_get_tvoc(void) {
    return last_tvoc;
}
