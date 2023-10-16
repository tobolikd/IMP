#include "utils.h"
#include "esp_err.h"
#include "esp_log.h"
#include "i2c-utils.h"
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>

static const char *tag = "utils (IMP)";

void ssd1306_turn_on(dev_conf_t device) {
    uint8_t commands[] = {
        OLED_TURN_ON,
    };

    ssd1306_send_commands(device, commands, sizeof(commands));
}

void show_temperature_info(dev_conf_t display, dev_conf_t senzor) {
    uint8_t data[] = {0x00, 0};
    data[1] = OLED_TURN_ON;
    i2c_send_data(display, data, 2);
    vTaskDelay(MILI_SECONDS(3000));
    data[1] = OLED_ON_RAM;
    i2c_send_data(display, data, 2);
    display_buff buff;
    memset(buff, 0, DISPLAY_BUFF_SIZE);
    for (int i = 0; i < 16; i++) {
        buff[i][2] = 1;
    }
    if (ssd1306_write_buffer(display, buff) != ESP_OK)
        ESP_LOGE(tag, "failed to write buffer");
}
void show_humidity_info(dev_conf_t display, dev_conf_t senzor) {}
