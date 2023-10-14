#include "freertos/FreeRTOS.h"
#include "i2c-utils.h"
#include "sdkconfig.h"
#include "ssd1306.h"
#include <stdint.h>

void app_main(void) {
    i2c_init(SSD1306_PORT, CONFIG_SSD1306_SCL_GPIO, CONFIG_SSD1306_SDA_GPIO,
             CONFIG_SSD1306_CLK_SPEED);
    uint8_t data[] = {0x00, 0};
    data[1] = OLED_TURN_ON;
    i2c_send_data(SSD1306_PORT, SSD1306_ADDR, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_ON_RAM;
    i2c_send_data(SSD1306_PORT, SSD1306_ADDR, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_ON_FULL;
    i2c_send_data(SSD1306_PORT, SSD1306_ADDR, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_TURN_OFF;
    i2c_send_data(SSD1306_PORT, SSD1306_ADDR, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    i2c_clean(SSD1306_PORT);
}
