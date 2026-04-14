#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "pins.h"
#include "rgb_led.h"
#include "servo.h"
#include "buzzer.h"
#include "potentiometer.h"
#include "button.h"
#include "dht.h"
#include "lcd_i2c.h"

static const char *TAG = "GROUP5";

static lcd_i2c_t lcd;

static void init_lcd(void) {
    lcd_i2c_config_t cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_gpio = LCD_SDA_PIN,
        .scl_gpio = LCD_SCL_PIN,
        .address = LCD_I2C_DEFAULT_ADDR,
        .cols = LCD_I2C_DEFAULT_COLS,
        .rows = LCD_I2C_DEFAULT_ROWS,
        .clk_speed_hz = 100000,
        .backlight = true
    };
    esp_err_t err = lcd_i2c_init(&lcd, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LCD init failed: %s", esp_err_to_name(err));
    } else {
        lcd_i2c_write_str(&lcd, "System Ready");
        ESP_LOGI(TAG, "LCD initialized");
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing peripherals...");

    rgb_led_init();
    servo_init();
    buzzer_init();
    potentiometer_init();
    button_init();
    dht_init();
    init_lcd();

    ESP_LOGI(TAG, "All peripherals initialized");

    while (1) {
        int pot1 = potentiometer_read(1);
        int pot2 = potentiometer_read(2);
        int pot3 = potentiometer_read(3);
        bool btn = button_is_pressed();

        ESP_LOGI(TAG, "POT1=%d POT2=%d POT3=%d BTN=%d", pot1, pot2, pot3, btn);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
