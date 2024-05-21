#include "bme68x_lib/bme68x.h"
#include "bme68x_lib/bme68x_defs.h"
#include "i2c_master.h"
#include "wait.h"
#include <math.h>
#include <string.h>

struct bme68x_dev sensor;

int8_t qmk_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
    uint8_t dev_id = *(uint8_t *)intf_ptr;
    i2c_transmit(dev_id, &reg_addr, 1, I2C_STOP);
    i2c_receive(dev_id, data, len, I2C_STOP);
    return 0;
}

int8_t qmk_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
    uint8_t dev_id = *(uint8_t *)intf_ptr;
    uint8_t buffer[len + 1];
    buffer[0] = reg_addr;
    memcpy(&buffer[1], data, len);
    i2c_transmit(dev_id, buffer, len + 1, I2C_STOP);
    return 0;
}

void qmk_delay_us(uint32_t period, void *intf_ptr) {
    wait_us(period);
}

float global_temperature = 0.0;
float global_humidity    = 0.0;
float global_iaq         = 0.0;

void bme680_setup(void) {
    sensor.intf     = BME68X_I2C_INTF;
    sensor.read     = qmk_i2c_read;
    sensor.write    = qmk_i2c_write;
    sensor.delay_us = qmk_delay_us;
    uint8_t dev_id  = BME68X_I2C_ADDR_HIGH;
    sensor.intf_ptr = &dev_id;

    bme68x_init(&sensor);

    struct bme68x_conf conf;
    bme68x_get_conf(&conf, &sensor);
    conf.os_hum  = BME68X_OS_2X;
    conf.os_temp = BME68X_OS_4X;
    conf.os_pres = BME68X_OS_8X;
    conf.filter  = BME68X_FILTER_SIZE_3;
    bme68x_set_conf(&conf, &sensor);

    struct bme68x_heatr_conf heatr_conf;
    heatr_conf.enable     = BME68X_ENABLE;
    heatr_conf.heatr_temp = 320; // Target temperature in degree Celsius
    heatr_conf.heatr_dur  = 150; // Duration in milliseconds
    bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &sensor);
}

float calculate_iaq(uint32_t gas_resistance, float humidity) {
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

    return air_quality_score;
}

const char *iaq_to_text(float iaq) {
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

void bme680_read_data(void) {
    struct bme68x_data data;
    uint8_t            n_fields;
    int8_t             rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &sensor);

    if (rslt == BME68X_OK) {
        global_temperature = data.temperature / 100.0;
        global_humidity    = data.humidity / 1000.0;
        global_iaq         = calculate_iaq(data.gas_resistance, global_humidity);
    }
}
