#include "i2c-utils.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/i2c_types.h"
#include "portmacro.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const char *tag = "I2C";

void i2c_init(dev_conf_t device) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .scl_io_num = device.scl,
        .sda_io_num = device.sda,
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        .master.clk_speed = device.clk_speed,
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(device.port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(device.port, I2C_MODE_MASTER, 0, 0, 0));
    ESP_LOGI(tag, "inicialization done");
}

esp_err_t i2c_send_data(dev_conf_t device, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    // set address and mode
    i2c_master_write_byte(cmd, (device.address << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write(cmd, data, len, I2C_MASTER_ACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(device.port, cmd, I2C_TIME_WAIT);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "failed to send data");
        return ret;
    }
    i2c_cmd_link_delete(cmd);
    ESP_LOGI(tag, "sent %zu byte(s) of data", len);
    return ESP_OK;
}

esp_err_t i2c_read_data(dev_conf_t device, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    // set address and mode
    i2c_master_write_byte(cmd, (device.address << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    i2c_master_read(cmd, data + len - 1, 1, I2C_MASTER_LAST_NACK); // last read segment doesn't have ACK
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(device.port, cmd, I2C_TIME_WAIT);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "failed to read data");
        return ret;
    }

    i2c_cmd_link_delete(cmd);
    ESP_LOGI(tag, "read %zu byte(s) of data", len);
    return ESP_OK;
}

void i2c_clean(dev_conf_t device) {
    ESP_ERROR_CHECK(i2c_driver_delete(device.port));
}
