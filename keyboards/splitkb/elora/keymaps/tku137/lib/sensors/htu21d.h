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

/**
 * @enum htu21d_state_t
 * @brief Enumeration for HTU21D sensor state machine states.
 */
typedef enum { HTU21D_STATE_IDLE, HTU21D_STATE_WAIT, HTU21D_STATE_READ_TEMP, HTU21D_STATE_WAIT_TEMP_MEASURE, HTU21D_STATE_READ_TEMP_DATA, HTU21D_STATE_READ_HUM, HTU21D_STATE_WAIT_HUM_MEASURE, HTU21D_STATE_READ_HUM_DATA } htu21d_state_t;

/**
 * @brief Initialize the HTU21D sensor.
 *
 * This function initializes the I2C communication and sets up the state machine
 * for the HTU21D sensor.
 *
 * @return true if the initialization was successful, false otherwise.
 */
bool htu21d_init(void);

/**
 * @brief Update the HTU21D sensor state machine.
 *
 * This function advances the state machine of the HTU21D sensor, performing
 * I2C operations as needed. It should be called regularly in the main loop.
 */
void htu21d_update(void);

/**
 * @brief Get the last measured temperature.
 *
 * This function returns the last measured temperature from the HTU21D sensor.
 *
 * @return The last measured temperature in degrees Celsius.
 */
float htu21d_get_temperature(void);

/**
 * @brief Get the last measured humidity.
 *
 * This function returns the last measured humidity from the HTU21D sensor.
 *
 * @return The last measured humidity in percentage.
 */
float htu21d_get_humidity(void);

/**
 * @example
 * Example usage in oled_task_user:
 *
 * @code
 * float temperature = htu21d_get_temperature();
 * float humidity = htu21d_get_humidity();
 *
 * int temp_int = (int)(temperature * 100);
 * int hum_int = (int)(humidity * 100);
 *
 * char temp_str[16];
 * char hum_str[16];
 *
 * snprintf(temp_str, sizeof(temp_str), "T:%d.%01d", temp_int / 100, temp_int % 10);
 * snprintf(hum_str, sizeof(hum_str), "H:%d.%01d", hum_int / 100, hum_int % 10);
 *
 * // Display sensor data on the OLED
 * oled_set_cursor(0, 4);
 * oled_write(temp_str, false);
 * oled_set_cursor(0, 6);
 * oled_write(hum_str, false);
 *
 * oled_render();
 * @endcode
 */

#endif // HTU21D_H
