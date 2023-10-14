#ifndef IMP_SSD1306_H
#define IMP_SSD1306_H

#include <stdint.h>
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


#endif // IMP_SSD1306_H

