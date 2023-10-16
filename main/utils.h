#ifndef IMP_UTILS_H
#define IMP_UTILS_H

#include "i2c-utils.h"

/**
 * initialize ssd1306 display settings
 */
void ssd1306_turn_on(dev_conf_t device);

void show_temperature_info(dev_conf_t display, dev_conf_t senzor);
void show_humidity_info(dev_conf_t display, dev_conf_t senzor);

#endif // IMP_UTILS_H
