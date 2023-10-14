#include "freertos/FreeRTOS.h"
#include "i2c-utils.h"

void app_main(void) {
    SSD1306_t a;
    i2c_init(&a, 0, 0);
}
