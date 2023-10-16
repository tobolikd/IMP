#include "utils.h"
#include "esp_err.h"
#include "esp_log.h"
#include "i2c-utils.h"
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>

static const char *tag = "utils (IMP)";

void ssd1306_turn_on(dev_conf_t device) {
    // software startup sequence from documentation (Application Note pg 5)
    uint8_t commands[] = {
        // clang-format off
        OLED_SET_MUX_RATIO,                 OLED_MUX_64,
        OLED_SET_DISPLAY_OFFSET,            0x00,
        OLED_RESET_DISPLAY_START_LINE,
        OLED_RESET_SEGMENT_REMAP,
        OLED_COM_SCAN_DIRECTION_FLIP,
        OLED_SET_COM_PINS_CONFIG,           OLED_COM_PINS_CONFIG_DEFAULT,
        OLED_SET_CONTRAST,                  OLED_CONTRAST_DEFAULT,
        OLED_ON_RAM,
        OLED_NORMAL_DISPLAY,
        OLED_SET_OSCILATOR_FREQ,            OLED_OSCILATOR_FREQ_DEFAULT,
        OLED_SET_CHARGE_PUMP,               OLED_CHARGE_PUMP_ENABLE,
        OLED_TURN_ON,

        // custom settings
        OLED_SET_ADDRESSING_MODE, OLED_ADDRESSING_MODE_HORIZONTAL,
        // clang-format on
    };

    ssd1306_send_commands(device, commands, sizeof(commands));
}

void show_temperature_info(dev_conf_t display, dev_conf_t senzor) {
    static display_buff buff;

    memset(buff, 0x00, DISPLAY_BUFF_SIZE);
    // get info
    // update num
    // update graph
    // structs to buf
    // display
    //

    buff[7][10] = 60;
    buff[7][11] = 66;
    buff[7][12] = 66;
    buff[7][13] = 60;

    buff[0][97] = 24;
    buff[0][98] = 36;
    buff[0][99] = 66;
    buff[0][100] = 66;
    buff[0][101] = 36;
    buff[0][102] = 24;

    for (int i = GRAPH_AXIS_VERTICAL_START_Y; i < GRAPH_AXIS_VERTICAL_END_Y;
         i++) {
        set_pixel(&buff, GRAPH_AXIS_VERTICAL_X, i);
    }

    for (int i = GRAPH_AXIS_HORIZONTAL_START_X; i < GRAPH_AXIS_HORIZONTAL_END_X;
         i++) {
        set_pixel(&buff, i, GRAPH_AXIS_HORIZONTAL_Y);
    }

    if (ssd1306_write_buffer(display, buff) != ESP_OK)
        ESP_LOGE(tag, "failed to write buffer");
}
void show_humidity_info(dev_conf_t display, dev_conf_t senzor) {}

void set_pixel(display_buff *buff, uint8_t x, uint8_t y) {
    (*buff)[y / DISPLAY_PAGE_HEIGHT][x] |= 1 << (y % DISPLAY_PAGE_HEIGHT);
}
