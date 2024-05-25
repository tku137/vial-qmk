#include "bme680/bme68x.h"
#include "bme680/bme68x_defs.h"
#include "i2c_master.h"
#include "print.h"
#include "wait.h"
#include <stdint.h>
#include <stdio.h>

// Define the I2C address for the BME680 sensor
#define BME680_I2C_ADDR BME68X_I2C_ADDR_HIGH
// #define I2C_TIMEOUT 100

// BME680 device structure
struct bme68x_dev        bme680;
struct bme68x_conf       conf;
struct bme68x_heatr_conf heatr_conf;
struct bme68x_data       data;
int8_t                   rslt;

// Custom I2C read function
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    return i2c_readReg(BME680_I2C_ADDR, reg_addr, reg_data, len, I2C_TIMEOUT) == I2C_STATUS_SUCCESS ? BME68X_OK : BME68X_E_COM_FAIL;
}

// Custom I2C write function
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    return i2c_writeReg(BME680_I2C_ADDR, reg_addr, reg_data, len, I2C_TIMEOUT) == I2C_STATUS_SUCCESS ? BME68X_OK : BME68X_E_COM_FAIL;
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

    rslt = bme68x_init(&bme680);
    if (rslt != BME68X_OK) {
        printf("BME680 initialization failed!\n");
        return;
    }

    // Configure BME680 sensor settings
    conf.filter  = BME68X_FILTER_OFF;
    conf.odr     = BME68X_ODR_NONE;
    conf.os_hum  = BME68X_OS_2X;
    conf.os_pres = BME68X_OS_4X;
    conf.os_temp = BME68X_OS_8X;
    rslt         = bme68x_set_conf(&conf, &bme680);

    // Set heater profile
    heatr_conf.enable     = BME68X_ENABLE;
    heatr_conf.heatr_temp = 320;
    heatr_conf.heatr_dur  = 100;
    rslt                  = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &bme680);
}

// Function to read data from the BME680 sensor
void read_bme680_data(void) {
    uint32_t del_period;
    uint8_t  n_data = 1;
    del_period      = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme680) + (heatr_conf.heatr_dur * 1000);

    bme68x_set_op_mode(BME68X_FORCED_MODE, &bme680);
    bme680.delay_us(del_period, NULL);

    rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_data, &bme680);
    if (rslt == BME68X_OK) {
        printf("Temperature: %.2f °C, Pressure: %.2f hPa, Humidity: %.2f %%\n", data.temperature / 100.0, data.pressure / 100.0, data.humidity / 1000.0);
    } else {
        printf("BME680 data read failed!\n");
    }
}
