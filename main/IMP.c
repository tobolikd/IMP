#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "freertos/FreeRTOS.h"
#include "i2c-utils.h"
#include "sdkconfig.h"
#include "soc/clk_tree_defs.h"
#include "ssd1306.h"
#include <stdint.h>

void app_main(void) {
    vTaskDelay(MILI_SECONDS(3000));

    i2c_master_bus_config_t conf = {
        .i2c_port = -1,
        .scl_io_num = CONFIG_SSD1306_SCL_GPIO,
        .sda_io_num = CONFIG_SSD1306_SDA_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags = {0},
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&conf, &bus_handle));

    i2c_device_config_t ssd1306_config = {
        .scl_speed_hz = CONFIG_SSD1306_CLK_SPEED,
        .device_address = SSD1306_ADDR,
    };

    i2c_master_dev_handle_t ssd1306_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &ssd1306_config, &ssd1306_handle));

    uint8_t data[] = {0x00, 0};
    data[1] = OLED_TURN_ON;
    ESP_ERROR_CHECK(i2c_master_transmit(ssd1306_handle, data, 2, I2C_TIME_WAIT));
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_ON_RAM;
    i2c_master_transmit(ssd1306_handle, data, 2, I2C_TIME_WAIT);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_ON_FULL;
    i2c_master_transmit(ssd1306_handle, data, 2, I2C_TIME_WAIT);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_TURN_OFF;
    i2c_master_transmit(ssd1306_handle, data, 2, I2C_TIME_WAIT);
    vTaskDelay(MILI_SECONDS(3000));

    i2c_master_bus_rm_device(ssd1306_handle);
    i2c_del_master_bus(bus_handle);
}
