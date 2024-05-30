/**
 * @file bme680_integration.h
 * @brief Header file for BME680 sensor integration with QMK firmware.
 * @author Your Name (your@email.com)
 * @date 2023-05-30
 *
 * This file contains the declarations and documentation for the BME680 sensor
 * integration with QMK firmware. It includes the struct definitions for sensor
 * data and the function prototypes for initializing, reading, and processing
 * the sensor data.
 */

#ifndef BME680_INTEGRATION_H
#define BME680_INTEGRATION_H

#include <stdint.h>

/**
 * @brief Struct to hold the BME680 sensor data.
 */
struct bme680_data {
    int      temperature;    /**< Temperature in degrees Celsius (scaled by 100) */
    int      humidity;       /**< Humidity in percent (scaled by 1000) */
    int      pressure;       /**< Pressure in hectoPascals (scaled by 100) */
    uint32_t gas_resistance; /**< Gas resistance in ohms */
};

/**
 * @brief Initialize the BME680 sensor.
 *
 * This function initializes the BME680 sensor, sets up the I2C communication,
 * and configures the sensor settings.
 *
 * @return BME68X_OK on success, or an error code on failure.
 */
int8_t bme680_init(void);

/**
 * @brief Read data from the BME680 sensor.
 *
 * This function reads the temperature, humidity, pressure, and gas resistance
 * data from the BME680 sensor.
 *
 * @param[out] data Pointer to a bme680_data struct to store the sensor data.
 * @return BME68X_OK on success, or an error code on failure.
 */
int8_t bme680_read_data(struct bme680_data *data);

/**
 * @brief Calculate the Indoor Air Quality (IAQ) index.
 *
 * This function calculates the IAQ index based on the gas resistance and
 * humidity values from the BME680 sensor.
 *
 * @param gas_resistance The gas resistance value from the BME680 sensor.
 * @param humidity The humidity value from the BME680 sensor.
 * @return The calculated IAQ index (0-100, where 100 is good quality air).
 */
int calculate_iaq(uint32_t gas_resistance, int humidity);

/**
 * @brief Get the IAQ text description based on the IAQ index.
 *
 * This function returns a string describing the air quality based on the
 * provided IAQ index value.
 *
 * @param iaq The IAQ index value.
 * @return A string describing the air quality.
 */
const char *iaq_to_text(int iaq);

#endif /* BME680_INTEGRATION_H */
