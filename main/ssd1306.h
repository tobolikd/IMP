#ifndef IMP_SSD1306_H
#define IMP_SSD1306_H

#include "esp_err.h"
#include "i2c-utils.h"
#include <stdint.h>

#define OLED_CONTROL_BYTE_LENGTH 1

// conrol bytes (specifies next operation)
#define OLED_CONTROL_COMMAND_BYTE 0x80
#define OLED_CONTROL_COMMAND_STREAM 0x00
#define OLED_CONTROL_DATA_BYTE 0xc0
#define OLED_CONTROL_DATA_STREAM 0x40

#define OLED_HORIZONTAL_ADDRESSING 0xa0
#define OLED_VERTICAL_ADDRESSING 0xa1

#define OLED_ON_RAM 0xa4
#define OLED_ON_FULL 0xa5

#define OLED_TURN_OFF 0xae
#define OLED_TURN_ON 0xaf

/**
 * display buffer type (for additional type checking)
 *
 * the size is fixed for the ssd1306
 * width: 128 / 8 = 16
 * height: 64 / 8 = 8
 */
typedef uint8_t display_buff[16][8];

#define DISPLAY_BUFF_SIZE 128 // 16 * 8

esp_err_t ssd1306_send_commands(dev_conf_t device, uint8_t *commands,
                                uint8_t len);
esp_err_t ssd1306_write_buffer(dev_conf_t device, display_buff buf);

#endif // IMP_SSD1306_H
