#include QMK_KEYBOARD_H
#include "enviro_sensor.h"
#include "i2c_master.h"
#include "print.h"
#include "timer.h"
#include "debug.h"
#include <math.h>

// Static variables to hold the sensor data and state
static sensor_data_t  last_sensor_data = {0.0f, 0.0f, 0, 0};
static sensor_state_t current_state    = SENSOR_STATE_IDLE;
static uint32_t       state_start_time = 0;
static uint8_t        temp_cmd         = HTU21D_TEMP_MEASURE_NOHOLD;
static uint8_t        hum_cmd          = HTU21D_HUMIDITY_MEASURE_NOHOLD;
static uint8_t        meas_cmd         = CCS811_ALG_RESULT_DATA;
static uint8_t        data[8];

bool enviro_sensor_init(void) {
    // Initialize I2C
    uprintf("Enviro Sensor: Initializing I2C\n");
    i2c_init();

    // Send initialization commands to CCS811
    uint8_t app_start_cmd = CCS811_APP_START;
    uprintf("Enviro Sensor: Sending CCS811 APP_START command\n");
    if (i2c_transmit(CCS811_ADDRESS << 1, &app_start_cmd, 1, I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
        uprintf("CCS811 initialization failed\n");
        return false;
    }

    // Set CCS811 measurement mode
    uint8_t meas_mode_cmd[] = {CCS811_MEAS_MODE, 0x10}; // 0x10 sets constant power mode, measurement every second
    uprintf("Enviro Sensor: Setting CCS811 measurement mode\n");
    if (i2c_transmit(CCS811_ADDRESS << 1, meas_mode_cmd, 2, I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
        uprintf("CCS811 set measurement mode failed\n");
        return false;
    }

    uprintf("CCS811 initialization successful\n");
    current_state    = SENSOR_STATE_WAIT;
    state_start_time = timer_read32();

    return true;
}

bool ccs811_set_environmental_data(float temperature, float humidity) {
    uprintf("CCS811: Setting environmental data\n");
    uint8_t  env_data[5];
    uint16_t temp = (uint16_t)((temperature + 25) * 512); // Temperature in 1/512 degrees
    uint16_t hum  = (uint16_t)(humidity * 512);           // Humidity in 1/512 %

    env_data[0] = CCS811_ENV_DATA; // Add the register address
    env_data[1] = (temp >> 8) & 0xFF;
    env_data[2] = temp & 0xFF;
    env_data[3] = (hum >> 8) & 0xFF;
    env_data[4] = hum & 0xFF;

    uprintf("CCS811: Sending data: %02X %02X %02X %02X %02X\n", env_data[0], env_data[1], env_data[2], env_data[3], env_data[4]);
    if (i2c_transmit(CCS811_ADDRESS << 1, env_data, 5, I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
        uprintf("CCS811: Failed to set environmental data\n");
        return false;
    }
    uprintf("CCS811: Environmental data set successfully\n");
    return true;
}

void enviro_sensor_update(void) {
    switch (current_state) {
        case SENSOR_STATE_IDLE:
            // Do nothing
            uprintf("Enviro Sensor: IDLE\n");
            break;

        case SENSOR_STATE_WAIT:
            // Wait for the next read interval
            if (timer_elapsed32(state_start_time) >= 1000) { // 1000 ms interval
                uprintf("-----------------------------------------------\n");
                uprintf("                                               \n");
                uprintf("                                               \n");
                uprintf("                                               \n");
                uprintf("-----------------------------------------------\n");
                current_state    = SENSOR_STATE_HTU21D_READ_TEMP;
                state_start_time = timer_read32();
                uprintf("Enviro Sensor: Transitioning to READ_TEMP state\n");
            }
            break;

        case SENSOR_STATE_HTU21D_READ_TEMP:
            // Send temperature measurement command
            // uprintf("HTU21D: Sending temperature measurement command\n");
            if (i2c_transmit(HTU21D_ADDRESS << 1, &temp_cmd, 1, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("HTU21D: I2C temperature write failed\n");
                current_state = SENSOR_STATE_WAIT;
            } else {
                uprintf("HTU21D: I2C temperature write success\n");
                state_start_time = timer_read32();
                current_state    = SENSOR_STATE_HTU21D_WAIT_TEMP;
                uprintf("HTU21D: Transitioning to WAIT_TEMP state\n");
            }
            break;

        case SENSOR_STATE_HTU21D_WAIT_TEMP:
            // uprintf("HTU21D: Waiting for temperature measurement to complete\n");
            // Wait for the sensor to perform the temperature measurement
            if (timer_elapsed32(state_start_time) >= HTU21D_TEMP_MEASURE_TIME / 1000) { // Convert us to ms
                uprintf("HTU21D: Wait for temperature measurement is complete\n");
                current_state = SENSOR_STATE_HTU21D_READ_TEMP_DATA;
                uprintf("HTU21D: Transitioning to READ_TEMP_DATA state\n");
            }
            break;

        case SENSOR_STATE_HTU21D_READ_TEMP_DATA:
            // Read temperature data
            // uprintf("HTU21D: Reading temperature data\n");
            if (i2c_receive(HTU21D_ADDRESS << 1, data, 3, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("HTU21D: I2C temperature read failed\n");
                current_state = SENSOR_STATE_WAIT;
            } else {
                uprintf("HTU21D: Temperature read success\n");
                uint16_t raw_temp            = (data[0] << 8) | data[1];
                last_sensor_data.temperature = -46.85 + 175.72 * (raw_temp / 65536.0);
                uprintf("HTU21D: Temperature = %.2f C\n", (float)last_sensor_data.temperature);
                current_state = SENSOR_STATE_HTU21D_READ_HUM;
                uprintf("HTU21D: Transitioning to READ_HUM state\n");
            }
            break;

        case SENSOR_STATE_HTU21D_READ_HUM:
            // Send humidity measurement command
            // uprintf("HTU21D: Sending humidity measurement command\n");
            if (i2c_transmit(HTU21D_ADDRESS << 1, &hum_cmd, 1, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("HTU21D: I2C humidity write failed\n");
                current_state = SENSOR_STATE_WAIT;
            } else {
                uprintf("HTU21D: I2C humidity write success\n");
                state_start_time = timer_read32();
                current_state    = SENSOR_STATE_HTU21D_WAIT_HUM;
                uprintf("HTU21D: Transitioning to WAIT_HUM state\n");
            }
            break;

        case SENSOR_STATE_HTU21D_WAIT_HUM:
            // uprintf("HTU21D: Waiting for humidity measurement to complete\n");
            // Wait for the sensor to perform the humidity measurement
            if (timer_elapsed32(state_start_time) >= HTU21D_HUMIDITY_MEASURE_TIME / 1000) { // Convert us to ms
                uprintf("HTU21D: Wait for humidity measurement is complete\n");
                current_state = SENSOR_STATE_HTU21D_READ_HUM_DATA;
                uprintf("HTU21D: Transitioning to READ_HUM_DATA state\n");
            }
            break;

        case SENSOR_STATE_HTU21D_READ_HUM_DATA:
            // Read humidity data
            // uprintf("HTU21D: Reading humidity data\n");
            if (i2c_receive(HTU21D_ADDRESS << 1, data, 3, TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("HTU21D: I2C humidity read failed\n");
                current_state = SENSOR_STATE_WAIT;
            } else {
                uprintf("HTU21D: I2C humidity read success\n");
                uint16_t raw_hum          = (data[0] << 8) | data[1];
                last_sensor_data.humidity = -6.0 + 125.0 * (raw_hum / 65536.0);
                uprintf("HTU21D: Humidity = %.2f %%\n", (float)last_sensor_data.humidity);
                current_state = SENSOR_STATE_CCS811_START_MEASURE;
                uprintf("HTU21D: Transitioning to CCS811_START_MEASURE state\n");
            }
            break;

        case SENSOR_STATE_CCS811_START_MEASURE:
            // Send CCS811 measurement command
            // uprintf("CCS811: Sending measurement command\n");
            if (i2c_transmit(CCS811_ADDRESS << 1, &meas_cmd, 1, I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("CCS811: Measurement command failed\n");
                current_state = SENSOR_STATE_WAIT;
            } else {
                uprintf("CCS811: Measurement command success\n");
                // Send environmental data to CCS811
                // ccs811_set_environmental_data(last_sensor_data.temperature, last_sensor_data.humidity);
                state_start_time = timer_read32();
                current_state    = SENSOR_STATE_CCS811_WAIT_MEASURE;
                uprintf("CCS811: Transitioning to WAIT_MEASURE state\n");
            }
            break;

        case SENSOR_STATE_CCS811_WAIT_MEASURE:
            // uprintf("CCS811: Waiting for measurement to complete\n");
            // Wait for the measurement to complete
            if (timer_elapsed32(state_start_time) >= 1000) { // 1000 ms for measurement
                uprintf("CCS811: Waiting for measurement is complete\n");
                current_state = SENSOR_STATE_CCS811_READ_DATA;
                uprintf("CCS811: Transitioning to READ_DATA state\n");
            }
            break;

        case SENSOR_STATE_CCS811_READ_DATA:
            // Read CCS811 sensor data
            // uprintf("CCS811: Reading sensor data\n");
            if (i2c_receive(CCS811_ADDRESS << 1, data, 5, I2C_TIMEOUT) != I2C_STATUS_SUCCESS) {
                uprintf("CCS811: Data read failed\n");
                current_state = SENSOR_STATE_WAIT;
            } else {
                uprintf("CCS811: Data read success\n");
                uint16_t raw_co2  = (data[0] << 8) | data[1];
                uint16_t raw_tvoc = (data[2] << 8) | data[3];

                // Debug the raw data received
                uprintf("CCS811: Raw data - CO2: %d, TVOC: %d\n", raw_co2, raw_tvoc);

                last_sensor_data.co2  = raw_co2;
                last_sensor_data.tvoc = raw_tvoc;
                uprintf("CCS811: CO2 = %d ppm, TVOC = %d ppb\n", last_sensor_data.co2, last_sensor_data.tvoc);

                current_state = SENSOR_STATE_WAIT;
                uprintf("Enviro sensor: Transitioning to WAIT_STATE state\n");
            }
            break;

        default:
            current_state = SENSOR_STATE_IDLE;
            break;
    }
}

sensor_data_t enviro_sensor_get_data(void) {
    return last_sensor_data;
}

void format_sensor_data(sensor_data_t data, char *temp_str, char *hum_str, char *co2_str, char *tvoc_str) {
    int temp_int = (int)(data.temperature * 100);
    int hum_int  = (int)(data.humidity * 100);
    snprintf(temp_str, 16, "T:%d.%01d", temp_int / 100, temp_int % 10);
    snprintf(hum_str, 16, "H:%d.%01d", hum_int / 100, hum_int % 10);
    snprintf(co2_str, 16, "CO2:%dppm", data.co2);
    snprintf(tvoc_str, 16, "TVOC:%dppb", data.tvoc);
}
