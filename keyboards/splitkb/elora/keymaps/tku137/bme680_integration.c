#include "bme68x_lib/bme68x.h"
#include "bme68x_lib/bme68x_defs.h"
#include "i2c_master.h"
#include "wait.h"
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
}

void bme680_read_data(void) {
    struct bme68x_data data;
    uint8_t            n_fields;
    int8_t             rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &sensor);

    if (rslt == BME68X_OK) {
        global_temperature = data.temperature / 100.0;
        global_humidity    = data.humidity / 1000.0;
    }
}
