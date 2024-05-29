#include "quantum.h"
#include "bme68x/bme68x.h"
#include "i2c_master.h"
#include "print.h"

#define BME680_ADDR BME68X_I2C_ADDR_LOW // 0x76

struct bme68x_dev        gas_sensor;
struct bme68x_conf       conf;
struct bme68x_heatr_conf heatr_conf;

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
    uprintf("I2C initialized\n");

    // Initialize BME680
    gas_sensor.read     = bme680_i2c_read;
    gas_sensor.write    = bme680_i2c_write;
    gas_sensor.delay_us = bme680_delay_us;
    gas_sensor.intf     = BME68X_I2C_INTF;
    gas_sensor.amb_temp = 25; // Ambient temperature in degree Celsius
    rslt                = bme68x_init(&gas_sensor);
    if (rslt != BME68X_OK) {
        uprintf("BME680 initialization failed\n");
        return rslt;
    }
    uprintf("BME680 initialized\n");

    // Set sensor configuration
    conf.os_hum  = BME68X_OS_16X;
    conf.os_temp = BME68X_OS_2X;
    conf.os_pres = BME68X_OS_1X;
    conf.filter  = BME68X_FILTER_SIZE_3;
    conf.odr     = BME68X_ODR_NONE;
    rslt         = bme68x_set_conf(&conf, &gas_sensor);
    if (rslt != BME68X_OK) {
        uprintf("BME680 set config failed\n");
        return rslt;
    }

    // Set heater configuration
    heatr_conf.enable     = BME68X_ENABLE;
    heatr_conf.heatr_temp = 320;
    heatr_conf.heatr_dur  = 150;
    rslt                  = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &gas_sensor);
    if (rslt != BME68X_OK) {
        uprintf("BME680 set heater config failed\n");
        return rslt;
    }

    return BME68X_OK;
}

void bme680_read_data(void) {
    int8_t             rslt;
    struct bme68x_data data;
    uint8_t            n_fields;

    // Set sensor to forced mode
    rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &gas_sensor);
    if (rslt != BME68X_OK) {
        uprintf("BME680 set op mode failed\n");
        return;
    }

    // Wait for the measurement to complete
    uint32_t del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &gas_sensor) + (heatr_conf.heatr_dur * 1000);
    gas_sensor.delay_us(del_period, gas_sensor.intf_ptr);

    // Read sensor data
    rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &gas_sensor);
    if (rslt != BME68X_OK) {
        uprintf("BME680 get data failed\n");
        return;
    }

    if (n_fields) {
        int temp_int = (int)(data.temperature * 100);
        int hum_int  = (int)(data.humidity * 1000);
        int pres_int = (int)(data.pressure * 100);

        char temp_str[8];
        char hum_str[8];
        char pres_str[12];

        snprintf(temp_str, sizeof(temp_str), "T:%d.%01d C", temp_int / 100, temp_int % 10);
        snprintf(hum_str, sizeof(hum_str), "H:%d.%01d %%", hum_int / 1000, hum_int % 10);
        snprintf(pres_str, sizeof(pres_str), "P:%d.%01d hPa", pres_int / 10000, pres_int % 10);

        uprintf("%s\n", temp_str);
        uprintf("%s\n", hum_str);
        uprintf("%s\n", pres_str);

        uprintf("G: %lu ohms\n", (unsigned long)data.gas_resistance);
    } else {
        uprintf("No new data available\n");
    }
}
