#ifndef IMP_I2C_UTILS_H
#define IMP_I2C_UTILS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

typedef struct {
	int _address;
	int _width;
	int _height;
	int _pages;
	int _dc;
	bool _scEnable;
	int _scStart;
	int _scEnd;
	int _scDirection;
	bool _flip;
} SSD1306_t;

void i2c_init(SSD1306_t* dev, int scl, int sda);

#endif // IMP_I2C_UTILS_H
