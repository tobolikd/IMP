#include "freertos/FreeRTOS.h"
#include "i2c-utils.h"
#include "sdkconfig.h"
#include "ssd1306.h"
#include "utils.h"
#include <stdint.h>

void app_main(void) {
    dev_conf_t ssd1306 = {.address = SSD1306_ADDR,
                          .port = SSD1306_PORT,
                          .scl = CONFIG_SSD1306_SCL_GPIO,
                          .sda = CONFIG_SSD1306_SDA_GPIO,
                          .clk_speed = CONFIG_SSD1306_CLK_SPEED};

    dev_conf_t sht31 = {.address = SHT31_ADDR,
                        .port = SHT31_PORT,
                        .scl = CONFIG_SHT31_SCL_GPIO,
                        .sda = CONFIG_SHT31_SDA_GPIO,
                        .clk_speed = CONFIG_SHT31_CLK_SPEED};
    // init i2c for both devices
    i2c_init(ssd1306);
    i2c_init(sht31);

    // turn on ssd1306
    ssd1306_turn_on(ssd1306);

#if CONFIG_START_ANIMATION_ENABLED
    // TODO play animation
#endif

    while (true) {
        show_temperature_info(ssd1306, sht31);
        vTaskDelay(MILI_SECONDS(200));
#if false
        show_humidity_info(ssd1306, sht31);
        vTaskDelay(SECONDS(CONFIG_SCREEN_SWITCH_INTERVAL_SECONDS));
#endif
    }

    // clean i2c
    i2c_clean(ssd1306);
    i2c_clean(sht31);
}
