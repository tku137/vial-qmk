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

// State machine states
typedef enum { CCS811_STATE_IDLE, CCS811_STATE_INIT, CCS811_STATE_WAIT, CCS811_STATE_READ } ccs811_state_t;

// Function declarations
bool     ccs811_init(void);
void     ccs811_update(void);
uint16_t ccs811_get_co2(void);
uint16_t ccs811_get_tvoc(void);

#endif // CCS811_H
