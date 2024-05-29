#include "i2c_master.h"
#include "quantum.h"
#include "bme68x/bme68x.h"
#include <string.h>
#include <stdio.h>

#define BME680_ADDR 0x76

struct bme68x_dev        gas_sensor;
struct bme68x_conf       conf;
struct bme68x_heatr_conf heatr_conf;
struct bme68x_data       data;

void bme680_delay_us(uint32_t period, void *intf_ptr) {
    wait_us(period);
}

BME68X_INTF_RET_TYPE bme680_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t result = i2c_transmit(BME680_ADDR << 1, &reg_addr, 1, I2C_TIMEOUT);
    if (result != 0) {
        printf("I2C transmit error: %d\n", result);
        return BME68X_E_COM_FAIL;
    }
    result = i2c_receive(BME680_ADDR << 1, reg_data, len, I2C_TIMEOUT);
    if (result != 0) {
        printf("I2C receive error: %d\n", result);
        return BME68X_E_COM_FAIL;
    }
    return BME68X_INTF_RET_SUCCESS;
}

BME68X_INTF_RET_TYPE bme680_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t buf[len + 1];
    buf[0] = reg_addr;
    memcpy(buf + 1, reg_data, len);
    uint8_t result = i2c_transmit(BME680_ADDR << 1, buf, len + 1, I2C_TIMEOUT);
    if (result != 0) {
        printf("I2C write error: %d\n", result);
        return BME68X_E_COM_FAIL;
    }
    return BME68X_INTF_RET_SUCCESS;
}

void bme680_init(void) {
    gas_sensor.intf     = BME68X_I2C_INTF;
    gas_sensor.read     = bme680_i2c_read;
    gas_sensor.write    = bme680_i2c_write;
    gas_sensor.delay_us = bme680_delay_us;
    gas_sensor.intf_ptr = NULL;

    int8_t result = bme68x_init(&gas_sensor);
    if (result != BME68X_OK) {
        printf("BME68x initialization error: %d\n", result);
        return;
    }

    conf.os_hum  = BME68X_OS_16X;
    conf.os_temp = BME68X_OS_2X;
    conf.os_pres = BME68X_OS_1X;
    conf.filter  = BME68X_FILTER_SIZE_3;
    conf.odr     = BME68X_ODR_NONE;

    result = bme68x_set_conf(&conf, &gas_sensor);
    if (result != BME68X_OK) {
        printf("BME68x set config error: %d\n", result);
        return;
    }

    heatr_conf.enable     = BME68X_ENABLE;
    heatr_conf.heatr_temp = 320;
    heatr_conf.heatr_dur  = 150;

    result = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &gas_sensor);
    if (result != BME68X_OK) {
        printf("BME68x set heater config error: %d\n", result);
        return;
    }
}

void bme680_read_data(void) {
    int8_t result = bme68x_set_op_mode(BME68X_FORCED_MODE, &gas_sensor);
    if (result != BME68X_OK) {
        printf("BME68x set op mode error: %d\n", result);
        return;
    }

    gas_sensor.delay_us(150000, gas_sensor.intf_ptr);

    uint32_t del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &gas_sensor) + (heatr_conf.heatr_dur * 1000);
    gas_sensor.delay_us(del_period, gas_sensor.intf_ptr);

    uint8_t n_fields;
    result = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &gas_sensor);
    if (result != BME68X_OK) {
        printf("BME68x get data error: %d\n", result);
        return;
    }

    printf("n_fields: %d\n", n_fields);
    if (n_fields)
        if (n_fields) {
            // Print raw data
            printf("Raw Temperature: %ld, Raw Humidity: %ld, Raw Pressure: %ld, Raw Gas Resistance: %ld\n", (long)data.temperature, (long)data.humidity, (long)data.pressure, (long)data.gas_resistance);

            // Process the data
            float    temperature    = data.temperature / 100.0f;
            float    humidity       = data.humidity / 1000.0f;
            float    pressure       = data.pressure / 100.0f;
            uint32_t gas_resistance = data.gas_resistance;

            // Display the processed data
            printf("Temperature: %.2f C, Humidity: %.2f %%RH, Pressure: %.2f hPa, Gas Resistance: %lu ohms\n", temperature, humidity, pressure, (unsigned long)gas_resistance);
        }
}
