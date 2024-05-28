#pragma once

#include <stdbool.h>
#include <stdint.h>

#define HTU21D_ADDRESS 0x40
#define HTU21D_TEMP_MEASURE_NOHOLD 0xF3
#define HTU21D_HUMIDITY_MEASURE_NOHOLD 0xF5
#define HTU21D_TEMP_MEASURE_TIME 50000     // 50ms in microseconds
#define HTU21D_HUMIDITY_MEASURE_TIME 16000 // 16ms in microseconds
#define TIMEOUT 100

bool  read_htu21d_data(uint32_t update_interval);
float get_temperature(void);
float get_humidity(void);
