#include "i2c-utils.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/i2c_types.h"
#include "portmacro.h"
#include <stdbool.h>
#include <stdint.h>

static const char *tag = "I2C";

void i2c_init(i2c_port_t port, int scl, int sda, uint32_t clk_speed) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .scl_io_num = scl,
        .sda_io_num = sda,
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        .master.clk_speed = clk_speed,
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0));
    ESP_LOGI(tag, "inicialization done");
}

void i2c_send_data(i2c_port_t port, uint8_t address, uint8_t *data,
                   uint8_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    // set address and mode
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);

    if (i2c_master_cmd_begin(port, cmd, I2C_TIME_WAIT) != ESP_OK)
        ESP_LOGE(tag, "failed to send data");

    i2c_cmd_link_delete(cmd);
    ESP_LOGI(tag, "sent %d byte(s) of data", len);
}

void i2c_read_data(i2c_port_t port, uint8_t address, uint8_t *data,
                   uint8_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    // set address and mode
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len - 1, true);
    i2c_master_read(cmd, data, 1, false); // last read segment doesn't have ACK
    i2c_master_stop(cmd);

    if (i2c_master_cmd_begin(port, cmd, I2C_TIME_WAIT) != ESP_OK)
        ESP_LOGE(tag, "failed to read data");

    i2c_cmd_link_delete(cmd);
    ESP_LOGI(tag, "read %d byte(s) of data", len);
}

void i2c_clean(i2c_port_t port) {
    ESP_ERROR_CHECK(i2c_driver_delete(port));
    ESP_LOGI(tag, "cleaned");
}
