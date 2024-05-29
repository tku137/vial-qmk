#include "bme680/bme68x.h"
#include "bme680/bme68x_defs.h"
#include "i2c_master.h"
#include "print.h"
#include "wait.h"
#include <stdint.h>
#include <stdio.h>
#include "debug.h"

// Define the I2C address for the BME680 sensor
#define BME680_I2C_ADDR BME68X_I2C_ADDR_LOW
// #define BME680_I2C_ADDR BME68X_I2C_ADDR_HIGH

// BME680 device structure
struct bme68x_dev bme680;

// Custom I2C read function
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    int8_t result = i2c_transmit(BME680_I2C_ADDR << 1, &reg_addr, 1, I2C_TIMEOUT);
    if (result != I2C_STATUS_SUCCESS) {
        printf("I2C Write Failed: %d\n", result);
        return BME68X_E_COM_FAIL;
    }

    result = i2c_receive(BME680_I2C_ADDR << 1, reg_data, len, I2C_TIMEOUT);
    if (result != I2C_STATUS_SUCCESS) {
        printf("I2C Read Failed: %d\n", result);
        return BME68X_E_COM_FAIL;
    }

    return BME68X_OK;
}

// Custom I2C write function
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t data[len];
    data[0] = reg_addr;
    memcpy(&data[1], reg_data, len);

    int8_t result = i2c_transmit(BME680_I2C_ADDR << 1, data, len, I2C_TIMEOUT);
    if (result != I2C_STATUS_SUCCESS) {
        printf("I2C Write Failed: %d\n", result);
        return BME68X_E_COM_FAIL;
    }

    return BME68X_OK;
}

// Custom delay function
void user_delay_us(uint32_t period, void *intf_ptr) {
    wait_us(period);
}

// Function to initialize the BME680 sensor
void init_bme680(void) {
    // Configure I2C settings
    i2c_init();
    bme680.intf     = BME68X_I2C_INTF;
    bme680.read     = user_i2c_read;
    bme680.write    = user_i2c_write;
    bme680.delay_us = user_delay_us;
    bme680.intf_ptr = NULL;

    dprintf("Initializing BME680\n");
    int8_t rslt = bme68x_init(&bme680);
    if (rslt != BME68X_OK) {
        printf("BME680 init failed: %d\n", rslt);
        return;
    }

    // Configure BME680 sensor settings
    struct bme68x_conf conf;
    conf.filter  = BME68X_FILTER_OFF;
    conf.odr     = BME68X_ODR_NONE;
    conf.os_hum  = BME68X_OS_2X;
    conf.os_pres = BME68X_OS_4X;
    conf.os_temp = BME68X_OS_8X;
    rslt         = bme68x_set_conf(&conf, &bme680);
    if (rslt != BME68X_OK) {
        printf("BME680 conf failed: %d\n", rslt);
        return;
    }

    // Set heater profile
    struct bme68x_heatr_conf heatr_conf;
    heatr_conf.enable     = BME68X_ENABLE;
    heatr_conf.heatr_temp = 320;
    heatr_conf.heatr_dur  = 100;
    rslt                  = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &bme680);
    if (rslt != BME68X_OK) {
        printf("BME680 heater conf failed: %d\n", rslt);
        return;
    }

    printf("BME680 init successful\n");
}

// Function to read data from the BME680 sensor
void read_bme680_data(void) {
    struct bme68x_data data;
    uint8_t            n_fields;

    int8_t rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme680);
    if (rslt != BME68X_OK) {
        printf("BME680 set op mode failed: %d\n", rslt);
        return;
    }

    struct bme68x_conf conf;
    conf.filter  = BME68X_FILTER_OFF;
    conf.odr     = BME68X_ODR_NONE;
    conf.os_hum  = BME68X_OS_2X;
    conf.os_pres = BME68X_OS_4X;
    conf.os_temp = BME68X_OS_8X;

    uint16_t meas_dur = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme680);
    meas_dur += 1000; // Add an additional delay of 1000 microseconds
    bme680.delay_us(meas_dur, bme680.intf_ptr);

    rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme680);
    if (rslt == BME68X_OK) {
        printf("Temperature: %.2f °C, Pressure: %.2f hPa, Humidity: %.2f %%, Gas Resistance: %.2f ohms\n", data.temperature, data.pressure, data.humidity, data.gas_resistance);
    } else {
        printf("BME680 data read failed: %d\n", rslt);
    }
}
