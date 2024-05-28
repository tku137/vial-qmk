#include QMK_KEYBOARD_H

#include "htu21d.h"

#include "i2c_master.h"
#include "print.h"
#include "timer.h"
#include <math.h>

static uint32_t last_read_time   = 0;
static float    last_temperature = 0.0f;
static float    last_humidity    = 0.0f;

bool read_htu21d_data(uint32_t update_interval) {
    if (timer_elapsed32(last_read_time) < update_interval) {
        return false; // Not enough time has passed since the last read
    }

    uint8_t  data[3];
    uint16_t raw_temp, raw_hum;

    // Send temperature measurement command
    uint8_t      temp_cmd          = HTU21D_TEMP_MEASURE_NOHOLD;
    i2c_status_t temp_write_status = i2c_transmit(HTU21D_ADDRESS << 1, &temp_cmd, 1, TIMEOUT);
    if (temp_write_status != I2C_STATUS_SUCCESS) {
        uprintf("I2C temperature write failed with status: %d\n", temp_write_status);
        return false;
    }

    // Wait for the sensor to perform the temperature measurement
    wait_us(HTU21D_TEMP_MEASURE_TIME);

    // Read temperature data
    i2c_status_t temp_read_status = i2c_receive(HTU21D_ADDRESS << 1, data, 3, TIMEOUT);
    if (temp_read_status != I2C_STATUS_SUCCESS) {
        uprintf("I2C temperature read failed with status: %d\n", temp_read_status);
        return false;
    }

    raw_temp         = (data[0] << 8) | data[1];
    last_temperature = -46.85 + 175.72 * (raw_temp / 65536.0);

    // Send humidity measurement command
    uint8_t      hum_cmd          = HTU21D_HUMIDITY_MEASURE_NOHOLD;
    i2c_status_t hum_write_status = i2c_transmit(HTU21D_ADDRESS << 1, &hum_cmd, 1, TIMEOUT);
    if (hum_write_status != I2C_STATUS_SUCCESS) {
        uprintf("I2C humidity write failed with status: %d\n", hum_write_status);
        return false;
    }

    // Wait for the sensor to perform the humidity measurement
    wait_us(HTU21D_HUMIDITY_MEASURE_TIME);

    // Read humidity data
    i2c_status_t hum_read_status = i2c_receive(HTU21D_ADDRESS << 1, data, 3, TIMEOUT);
    if (hum_read_status != I2C_STATUS_SUCCESS) {
        uprintf("I2C humidity read failed with status: %d\n", hum_read_status);
        return false;
    }

    raw_hum       = (data[0] << 8) | data[1];
    last_humidity = -6.0 + 125.0 * (raw_hum / 65536.0);

    last_read_time = timer_read32(); // Update the last read time
    return true;
}

float get_temperature(void) {
    return last_temperature;
}

float get_humidity(void) {
    return last_humidity;
}
