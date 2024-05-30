#ifndef BME680_INTEGRATION_H
#define BME680_INTEGRATION_H

#include "quantum.h"
#include "bme68x/bme68x.h"

// Define the address of the BME680 sensor
#define BME680_ADDR BME68X_I2C_ADDR_LOW // 0x76

// Define the interval for sensor updates in milliseconds
#define SENSOR_UPDATE_INTERVAL 1000 // 1000 ms (1 second)

// State definitions for the state machine
#define SENSOR_STATE_IDLE 0
#define SENSOR_STATE_MEASURING 1
#define SENSOR_STATE_READING 2

// Structure to hold sensor data
struct bme680_data {
    int      temperature;
    int      humidity;
    int      pressure;
    uint32_t gas_resistance;
};

/*
 * Macro definition for valid new data (0x80) AND
 * heater stability (0x10) AND gas resistance (0x20) values
 */
#define BME68X_VALID_DATA UINT8_C(0xB0)

// External declaration of the current sensor data
extern struct bme680_data current_sensor_data;

/**
 * @brief Initializes the BME680 sensor.
 *
 * This function initializes the I2C communication and configures the BME680 sensor
 * for parallel mode operation with specified settings for temperature, humidity,
 * and pressure oversampling, as well as the gas sensor heater configuration.
 *
 * @return int8_t BME68X_OK if initialization is successful, otherwise an error code.
 */
int8_t bme680_init(void);

/**
 * @brief Updates the BME680 sensor data.
 *
 * This function implements a state machine to manage the sensor reading process
 * in a non-blocking manner. It periodically checks the sensor state and reads
 * the sensor data when the measurement is complete. The function should be called
 * regularly from the main loop (e.g., from matrix_scan_user).
 */
void bme680_update(void);

/**
 * @brief Calculates the Indoor Air Quality (IAQ) index.
 *
 * This function calculates the IAQ index based on the gas resistance and humidity
 * readings from the BME680 sensor. The IAQ index is a measure of air quality, with
 * higher values indicating better air quality.
 *
 * @param gas_resistance The gas resistance reading from the BME680 sensor.
 * @param humidity The humidity reading from the BME680 sensor.
 *
 * @return int The calculated IAQ index.
 */
int calculate_iaq(uint32_t gas_resistance, int humidity);

/**
 * @brief Converts the IAQ index to a text description.
 *
 * This function converts the numerical IAQ index to a human-readable text description
 * indicating the air quality level.
 *
 * @param iaq The IAQ index.
 *
 * @return const char* The text description of the air quality level.
 */
const char *iaq_to_text(int iaq);

/**
 * @brief Reads the BME680 sensor data.
 *
 * This function reads the temperature, humidity, pressure, and gas resistance data
 * from the BME680 sensor. It should be used to get the latest sensor data after
 * calling bme680_update.
 *
 * @param data Pointer to a bme680_data structure to store the read data.
 * @param update_interval The minimum interval between sensor reads in milliseconds.
 *
 * @return int8_t BME68X_OK if data is read successfully, otherwise an error code.
 */
int8_t bme680_read_data(struct bme680_data *data, uint32_t update_interval);

#endif // BME680_INTEGRATION_H
