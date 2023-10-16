#ifndef IMP_I2C_UTILS_H
#define IMP_I2C_UTILS_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"
#include "hal/i2c_types.h"
#include "portmacro.h"
#include <stdint.h>

// assigned device ports
#define SSD1306_PORT I2C_NUM_0
#define SHT31_PORT I2C_NUM_1

// i2c addresses from documentation
#define SHT31_ADDR 0x44
#define SSD1306_ADDR 0x3C

#define MILI_SECONDS(t) (t / portTICK_PERIOD_MS)
#define SECONDS(t) (1000 * t / portTICK_PERIOD_MS)

#define I2C_TIME_WAIT MILI_SECONDS(10)

typedef struct DeviceConfig {
    i2c_port_t port;
    uint8_t address;
    int scl;
    int sda;
    uint32_t clk_speed;
} dev_conf_t;

void i2c_init(dev_conf_t device);

esp_err_t i2c_send_data(dev_conf_t device, uint8_t *data,
                   uint8_t len);

esp_err_t i2c_read_data(dev_conf_t device, uint8_t *data,
                   uint8_t len);

void i2c_clean(dev_conf_t device);

#endif // IMP_I2C_UTILS_H
