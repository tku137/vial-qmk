#include "quantum.h"
#include "bme68x/bme68x.h"
#include "bme680_integration.h"
#include "i2c_master.h"
#include "print.h"
#include <math.h>

#define BME680_ADDR BME68X_I2C_ADDR_LOW // 0x76
#define SENSOR_UPDATE_INTERVAL 1000     // Read sensor every 1 second

#define SENSOR_STATE_IDLE 0
#define SENSOR_STATE_MEASURING 1
#define SENSOR_STATE_READING 2

// Declare static global variables
static struct bme68x_dev        gas_sensor;
static struct bme68x_conf       conf;
static struct bme68x_heatr_conf heatr_conf;

static uint32_t last_read_time = 0;

static uint8_t     sensor_state      = SENSOR_STATE_IDLE;
static uint32_t    sensor_start_time = 0;
static uint32_t    last_timer_check  = 0;
struct bme680_data current_sensor_data;

void bme680_delay_us(uint32_t period, void *intf_ptr) {
    wait_us(period);
}

int8_t bme680_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    i2c_transmit(BME680_ADDR << 1, &reg_addr, 1, I2C_TIMEOUT);
    i2c_receive(BME680_ADDR << 1, reg_data, len, I2C_TIMEOUT);
    return BME68X_OK;
}

int8_t bme680_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t buffer[len + 1];
    buffer[0] = reg_addr;
    memcpy(buffer + 1, reg_data, len);
    i2c_transmit(BME680_ADDR << 1, buffer, len + 1, I2C_TIMEOUT);
    return BME68X_OK;
}

int8_t bme680_init(void) {
    int8_t rslt;

    // Initialize I2C
    i2c_init();

    // Initialize BME680
    gas_sensor.read     = bme680_i2c_read;
    gas_sensor.write    = bme680_i2c_write;
    gas_sensor.delay_us = bme680_delay_us;
    gas_sensor.intf     = BME68X_I2C_INTF;
    gas_sensor.amb_temp = 22; // Ambient temperature in degree Celsius
    rslt                = bme68x_init(&gas_sensor);
    if (rslt != BME68X_OK) {
        // BME680 initialization failed
        return rslt;
    }

    // Set sensor configuration
    conf.os_hum  = BME68X_OS_16X;
    conf.os_temp = BME68X_OS_2X;
    conf.os_pres = BME68X_OS_1X;
    conf.filter  = BME68X_FILTER_SIZE_3;
    conf.odr     = BME68X_ODR_NONE;
    rslt         = bme68x_set_conf(&conf, &gas_sensor);
    if (rslt != BME68X_OK) {
        // BME680 set config failed
        return rslt;
    }

    // Set heater configuration for parallel mode
    heatr_conf.enable          = BME68X_ENABLE;
    heatr_conf.heatr_temp_prof = temp_prof; // Provide the temperature profile array
    heatr_conf.heatr_dur_prof  = mul_prof;  // Provide the duration multiplier profile array
    heatr_conf.profile_len     = 10;        // Set the length of the temperature and duration profiles

    // Calculate the shared heating duration
    uint32_t meas_dur           = bme68x_get_meas_dur(BME68X_PARALLEL_MODE, &conf, &gas_sensor);
    heatr_conf.shared_heatr_dur = (uint16_t)(140 - (meas_dur / 1000));

    rslt = bme68x_set_heatr_conf(BME68X_PARALLEL_MODE, &heatr_conf, &gas_sensor);
    if (rslt != BME68X_OK) {
        // BME680 set heater config failed
        return rslt;
    }

    // Set the sensor to parallel mode
    rslt = bme68x_set_op_mode(BME68X_PARALLEL_MODE, &gas_sensor);
    if (rslt != BME68X_OK) {
        // BME680 set op mode failed
        return rslt;
    }

    return BME68X_OK;
}

void bme680_update(void) {
    int8_t             rslt;
    struct bme68x_data sensor_data[3]; // Array to store up to 3 sets of sensor data
    uint8_t            n_fields;

    uint32_t current_time = timer_read32();

    // Check if enough time has passed since the last sensor read
    if (timer_elapsed32(last_timer_check) < SENSOR_UPDATE_INTERVAL) {
        return;
    }
    last_timer_check = current_time;

    switch (sensor_state) {
        case SENSOR_STATE_IDLE:
            // Start the measurement
            rslt = bme68x_set_op_mode(BME68X_PARALLEL_MODE, &gas_sensor);
            if (rslt == BME68X_OK) {
                sensor_start_time = timer_read32();
                sensor_state      = SENSOR_STATE_MEASURING;
            }
            break;

        case SENSOR_STATE_MEASURING: {
            // Wait for the measurement to complete
            uint32_t del_period = bme68x_get_meas_dur(BME68X_PARALLEL_MODE, &conf, &gas_sensor) + (heatr_conf.shared_heatr_dur * 1000);
            if (timer_elapsed32(sensor_start_time) >= del_period / 1000) {
                sensor_state = SENSOR_STATE_READING;
            }
            break;
        }

        case SENSOR_STATE_READING:
            // Read the sensor data
            rslt = bme68x_get_data(BME68X_PARALLEL_MODE, sensor_data, &n_fields, &gas_sensor);
            if (rslt == BME68X_OK && n_fields > 0) {
                for (uint8_t i = 0; i < n_fields; i++) {
                    if (sensor_data[i].status & BME68X_VALID_DATA) {
                        // Process the valid sensor data
                        current_sensor_data.temperature    = (int)sensor_data[i].temperature;
                        current_sensor_data.humidity       = (int)sensor_data[i].humidity;
                        current_sensor_data.pressure       = (int)sensor_data[i].pressure;
                        current_sensor_data.gas_resistance = sensor_data[i].gas_resistance;
                    }
                }
            }
            sensor_state = SENSOR_STATE_IDLE;
            break;
    }
}

int calculate_iaq(uint32_t gas_resistance, int humidity) {
    float hum_weighting = 0.25; // Humidity effect is 25% of the total air quality score
    float gas_weighting = 0.75; // Gas effect is 75% of the total air quality score

    float hum_score, gas_score;
    float hum_reference = 40.0;

    // Calculate humidity contribution to IAQ index
    if (humidity >= 38 && humidity <= 42) {
        hum_score = 0.25 * 100.0; // Humidity +/-5% around optimum
    } else {
        if (humidity < 38) {
            hum_score = (hum_weighting / hum_reference) * humidity * 100.0;
        } else {
            hum_score = ((-hum_weighting / (100.0 - hum_reference) * humidity) + 0.416666) * 100.0;
        }
    }

    // Calculate gas contribution to IAQ index
    float gas_lower_limit = 5000.0;  // Bad air quality limit
    float gas_upper_limit = 50000.0; // Good air quality limit
    if (gas_resistance > gas_upper_limit) gas_resistance = gas_upper_limit;
    if (gas_resistance < gas_lower_limit) gas_resistance = gas_lower_limit;
    gas_score = (gas_weighting / (gas_upper_limit - gas_lower_limit) * gas_resistance - (gas_lower_limit * (gas_weighting / (gas_upper_limit - gas_lower_limit)))) * 100.0;

    // Combine results for the final IAQ index value (0-100% where 100% is good quality air)
    float air_quality_score = hum_score + gas_score;
    if (air_quality_score > 100.0) air_quality_score = 100.0;

    return (int)round(air_quality_score);
}

const char *iaq_to_text(int iaq) {
    if (iaq >= 301)
        return "Hazardous";
    else if (iaq >= 201 && iaq <= 300)
        return "Very Unhealthy";
    else if (iaq >= 176 && iaq <= 200)
        return "Unhealthy";
    else if (iaq >= 151 && iaq <= 175)
        return "Unhealthy for Sensitive Groups";
    else if (iaq >= 51 && iaq <= 150)
        return "Moderate";
    else
        return "Good";
}

int8_t bme680_read_data(struct bme680_data *data, uint32_t update_interval) {
    // Check if enough time has passed since the last read
    uint32_t current_time = timer_read32();
    if (timer_elapsed32(last_read_time) < update_interval) {
        return BME68X_OK;
    }

    int8_t             rslt;
    struct bme68x_data sensor_data[3]; // Array to store up to 3 sets of sensor data
    uint8_t            n_fields;

    // Set sensor to parallel mode
    rslt = bme68x_set_op_mode(BME68X_PARALLEL_MODE, &gas_sensor);
    if (rslt != BME68X_OK) {
        // BME680 set op mode failed
        return rslt;
    }

    // Wait for the measurement to complete
    uint32_t del_period = bme68x_get_meas_dur(BME68X_PARALLEL_MODE, &conf, &gas_sensor) + (heatr_conf.shared_heatr_dur * 1000);
    gas_sensor.delay_us(del_period, gas_sensor.intf_ptr);

    // Read sensor data
    rslt = bme68x_get_data(BME68X_PARALLEL_MODE, sensor_data, &n_fields, &gas_sensor);
    if (rslt != BME68X_OK) {
        // BME680 get data failed
        return rslt;
    }

    if (n_fields) {
        // Process the sensor data from the parallel mode
        for (uint8_t i = 0; i < n_fields; i++) {
            if (sensor_data[i].status & BME68X_VALID_DATA) {
                // Process the valid sensor data
                data->temperature    = (int)sensor_data[i].temperature;
                data->humidity       = (int)sensor_data[i].humidity;
                data->pressure       = (int)sensor_data[i].pressure;
                data->gas_resistance = sensor_data[i].gas_resistance;
                last_read_time       = current_time;
            }
        }
        return BME68X_OK;
    } else {
        uprintf("No new data available\n");
        return BME68X_W_NO_NEW_DATA;
    }
}
