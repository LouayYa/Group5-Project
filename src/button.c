#include "button.h"
#include "pins.h"
#include "driver/gpio.h"

void button_init(void) {
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&cfg);
}

bool button_is_pressed(void) {
    return gpio_get_level(BUTTON_PIN) == 0;
}
