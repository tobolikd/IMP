#include "freertos/FreeRTOS.h"
#include "i2c-utils.h"
#include "sdkconfig.h"
#include "ssd1306.h"
#include <stdint.h>

void app_main(void) {
    dev_conf_t ssd1306 = {.port = SSD1306_PORT,
                          .scl = CONFIG_SSD1306_SCL_GPIO,
                          .sda = CONFIG_SSD1306_SDA_GPIO,
                          .clk_speed = CONFIG_SSD1306_CLK_SPEED};

    i2c_init(ssd1306);
    uint8_t data[] = {0x00, 0};
    data[1] = OLED_TURN_ON;
    i2c_send_data(ssd1306, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_ON_RAM;
    i2c_send_data(ssd1306, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_ON_FULL;
    i2c_send_data(ssd1306, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_TURN_OFF;
    i2c_send_data(ssd1306, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    i2c_clean(ssd1306);
}
