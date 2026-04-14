#ifndef RGB_LED_H
#define RGB_LED_H

#include <stdint.h>

void rgb_led_init(void);
void rgb1_set(uint8_t r, uint8_t g, uint8_t b);
void rgb2_set(uint8_t r, uint8_t g, uint8_t b);
void rgb3_set(uint8_t r, uint8_t g, uint8_t b);

#endif
