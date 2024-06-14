#pragma once

#ifndef ENVIRO_SENSOR_H
#    define ENVIRO_SENSOR_H

#    include <stdbool.h>
#    include <stdint.h>

// I2C addresses
#    define HTU21D_ADDRESS 0x40
#    define CCS811_ADDRESS 0x5A

// HTU21D command defines
#    define HTU21D_TEMP_MEASURE_NOHOLD 0xF3
#    define HTU21D_HUMIDITY_MEASURE_NOHOLD 0xF5
#    define HTU21D_TEMP_MEASURE_TIME 50000     // 50ms in microseconds
#    define HTU21D_HUMIDITY_MEASURE_TIME 16000 // 16ms in microseconds

// CCS811 command defines
#    define CCS811_APP_START 0xF4
#    define CCS811_MEAS_MODE 0x01
#    define CCS811_ALG_RESULT_DATA 0x02
#    define CCS811_ENV_DATA 0x05
#    define CCS811_BASELINE 0x11

#    define TIMEOUT 100

// EEPROM address for baseline storage
#    define EEPROM_BASELINE_ADDR 0x00

// State machine states
typedef enum { SENSOR_STATE_IDLE, SENSOR_STATE_WAIT, SENSOR_STATE_HTU21D_READ_TEMP, SENSOR_STATE_HTU21D_WAIT_TEMP, SENSOR_STATE_HTU21D_READ_TEMP_DATA, SENSOR_STATE_HTU21D_READ_HUM, SENSOR_STATE_HTU21D_WAIT_HUM, SENSOR_STATE_HTU21D_READ_HUM_DATA, SENSOR_STATE_CCS811_SET_ENV_DATA, SENSOR_STATE_CCS811_WAIT_ENV_DATA, SENSOR_STATE_CCS811_START_MEASURE, SENSOR_STATE_CCS811_WAIT_MEASURE, SENSOR_STATE_CCS811_READ_DATA } sensor_state_t;

typedef struct {
    float    temperature;
    float    humidity;
    uint16_t co2;
    uint16_t tvoc;
} sensor_data_t;

/**
 * @brief Initialize the environmental sensors.
 *
 * This function initializes the I2C communication and sets up the state machine
 * for both the HTU21D and CCS811 sensors.
 *
 * @return true if the initialization was successful, false otherwise.
 */
bool enviro_sensor_init(void);

/**
 * @brief Update the environmental sensors state machine.
 *
 * This function advances the state machine, performing I2C operations as needed.
 * It should be called regularly in the main loop.
 */
void enviro_sensor_update(void);

/**
 * @brief Get the last measured sensor data.
 *
 * This function returns a struct containing the last measured temperature, humidity,
 * CO2, and TVOC values.
 *
 * @return A struct containing the last measured sensor values.
 */
sensor_data_t enviro_sensor_get_data(void);

/**
 * @brief Format sensor data to strings for display.
 *
 * This function formats the sensor data into string representations.
 *
 * @param data The sensor data to format.
 * @param temp_str Buffer to store the formatted temperature string.
 * @param hum_str Buffer to store the formatted humidity string.
 * @param co2_str Buffer to store the formatted CO2 string.
 * @param tvoc_str Buffer to store the formatted TVOC string.
 */
void format_sensor_data(sensor_data_t data, char *temp_str, char *hum_str, char *co2_str, char *tvoc_str);

/**
 * @brief Get the current baseline from the sensor.
 *
 * This function retrieves the current baseline value from the CCS811 sensor.
 *
 * @return The baseline value.
 */
uint16_t ccs811_get_baseline(void);

/**
 * @brief Set the baseline for the sensor.
 *
 * This function sets the baseline value for the CCS811 sensor.
 *
 * @param baseline The baseline value to set.
 */
void ccs811_set_baseline(uint16_t baseline);

#endif // ENVIRO_SENSOR_H
