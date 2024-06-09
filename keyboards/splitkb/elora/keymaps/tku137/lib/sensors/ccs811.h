#ifndef CCS811_H
#define CCS811_H

#include <stdint.h>
#include <stdbool.h>

// CCS811 I2C address
#define CCS811_ADDRESS 0x5A

// CCS811 command defines
#define CCS811_APP_START 0xF4
#define CCS811_MEAS_MODE 0x01
#define CCS811_ALG_RESULT_DATA 0x02

// State machine states for CCS811
typedef enum { CCS811_STATE_IDLE, CCS811_STATE_WAIT, CCS811_STATE_START_MEASURE, CCS811_STATE_WAIT_MEASURE, CCS811_STATE_READ_DATA } ccs811_state_t;

/**
 * @brief Initialize the CCS811 sensor.
 *
 * This function initializes the I2C communication and sets up the state machine
 * for the CCS811 sensor.
 *
 * @return true if the initialization was successful, false otherwise.
 */
bool ccs811_init(void);

/**
 * @brief Update the CCS811 sensor state machine.
 *
 * This function advances the state machine of the CCS811 sensor, performing
 * I2C operations as needed. It should be called regularly in the main loop.
 */
void ccs811_update(void);

/**
 * @brief Get the last measured CO2 concentration.
 *
 * This function returns the last measured CO2 concentration from the CCS811 sensor.
 *
 * @return The last measured CO2 concentration in ppm.
 */
uint16_t ccs811_get_co2(void);

/**
 * @brief Get the last measured TVOC concentration.
 *
 * This function returns the last measured TVOC concentration from the CCS811 sensor.
 *
 * @return The last measured TVOC concentration in ppb.
 */
uint16_t ccs811_get_tvoc(void);

#endif // CCS811_H
