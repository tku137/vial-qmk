#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include <stdbool.h>

// State machine states for sensors
typedef enum { SENSOR_STATE_IDLE, SENSOR_STATE_WAIT, SENSOR_STATE_READ_HTU21D, SENSOR_STATE_READ_CCS811 } sensor_state_t;

// Function declarations
bool     sensors_init(void);
void     sensors_update(void);
float    get_last_temperature(void);
float    get_last_humidity(void);
uint16_t get_last_co2(void);
uint16_t get_last_tvoc(void);

#endif // SENSORS_H
