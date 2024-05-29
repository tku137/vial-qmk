/**
 * @file htu21d.h
 * @brief Driver for the HTU21D temperature and humidity sensor.
 *
 * This driver provides functions to read temperature and humidity data from the HTU21D sensor
 * and display it on an OLED display using the QMK firmware.
 *
 * To use this driver, you need to make the following changes in your QMK project:
 *
 * 1. In your `config.h` file, add the following line to define the update interval:
 *    ```c
 *    #define HTU21D_UPDATE_INTERVAL 1000 // Update every 1 second
 *    ```
 *
 * 2. In your `rules.mk` file, add the following line to include the driver source file:
 *    ```makefile
 *    SRC += htu21d.c
 *    ```
 *
 * 3. In your `keymap.c` file, include the `htu21d.h` header file:
 *    ```c
 *    #include "htu21d.h"
 *    ```
 *
 * 4. In your `oled_task_user` function, add the following code to read and display the sensor data:
 *    ```c
 *    // Read temperature and humidity data from the HTU21D sensor
 *    if (read_htu21d_data(HTU21D_UPDATE_INTERVAL)) {
 *        float temperature = get_temperature();
 *        float humidity = get_humidity();
 *
 *        // Convert temperature and humidity to integers with two decimal places
 *        int temp_int = (int)(temperature * 100);
 *        int hum_int = (int)(humidity * 100);
 *
 *        // Convert temperature and humidity to strings
 *        char temp_str;
 *        char hum_str;
 *        snprintf(temp_str, sizeof(temp_str), "T:%d.%01d", temp_int / 100, temp_int % 10);
 *        snprintf(hum_str, sizeof(hum_str), "H:%d.%01d", hum_int / 100, hum_int % 10);
 *
 *        // Display temperature and humidity on the OLED
 *        oled_clear();
 *        oled_write(temp_str, false);
 *        oled_write_P(PSTR(" C\n"), false);
 *        oled_write(hum_str, false);
 *        oled_write_P(PSTR(" %\n"), false);
 *    }
 *    ```
 *
 * @note Make sure to have the necessary I2C configuration and OLED setup in your QMK project.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define HTU21D_ADDRESS 0x40
#define HTU21D_TEMP_MEASURE_NOHOLD 0xF3
#define HTU21D_HUMIDITY_MEASURE_NOHOLD 0xF5
#define HTU21D_TEMP_MEASURE_TIME 50000     // 50ms in microseconds
#define HTU21D_HUMIDITY_MEASURE_TIME 16000 // 16ms in microseconds
#define TIMEOUT 100

/**
 * @brief Reads temperature and humidity data from the HTU21D sensor.
 *
 * This function reads the temperature and humidity data from the HTU21D sensor
 * at the specified update interval. It returns true if new data is available,
 * and false otherwise.
 *
 * @param update_interval The interval in milliseconds at which to update the sensor data.
 * @return true if new data is available, false otherwise.
 */
bool read_htu21d_data(uint32_t update_interval);

/**
 * @brief Gets the last read temperature value.
 *
 * This function returns the last read temperature value in degrees Celsius.
 *
 * @return The temperature value in degrees Celsius.
 */
float get_temperature(void);

/**
 * @brief Gets the last read humidity value.
 *
 * This function returns the last read humidity value in percentage.
 *
 * @return The humidity value in percentage.
 */
float get_humidity(void);
