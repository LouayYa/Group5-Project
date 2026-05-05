#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t xButtonSemaphore;

void button_init(void);
bool button_is_pressed(void);

#endif
