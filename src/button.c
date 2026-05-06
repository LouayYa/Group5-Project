#include "button.h"
#include "pins.h"
#include "driver/gpio.h"
#include "esp_timer.h"

SemaphoreHandle_t xButtonSemaphore = NULL;

#define DEBOUNCE_US 200000  // 200 ms

static void IRAM_ATTR button_isr_handler(void *arg) {
    static int64_t last_press_us = 0;
    int64_t now = esp_timer_get_time();
    if ((now - last_press_us) < DEBOUNCE_US) return;
    last_press_us = now;

    // If button is pressed and wakes up higher priority task, it will sent woken as true.
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xButtonSemaphore, &xHigherPriorityTaskWoken);
    // If woken is true, then immediately serve the higher priority task that was woken.
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void button_init(void) {
    xButtonSemaphore = xSemaphoreCreateBinary();

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,  // falling edge = button pressed
    };
    gpio_config(&cfg);

    // Install ISR service and add handler for button pin
    // When button is pressed, a falling edge is detected, and the interrupt handler is called.
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);
}

// returns true if button is currently pressed (active low)
bool button_is_pressed(void) {
    return gpio_get_level(BUTTON_PIN) == 0;
}
