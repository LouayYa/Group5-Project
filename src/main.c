#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_err.h"

#include "pins.h"
#include "rgb_led.h"
#include "servo.h"
#include "buzzer.h"
#include "potentiometer.h"
#include "button.h"
#include "dht.h"
#include "lcd_i2c.h"
#include "structs.h"
#include "SensorTask.h"
#include "AlertMngrTask.h"
#include "vDisplayTask.h"

static const char *TAG = "GROUP5";

lcd_i2c_t lcd;
volatile system_state_t system_state = SYS_NORMAL;

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

ZoneStatus_t zones[3];
SemaphoreHandle_t xZoneMutex;
QueueHandle_t xAlertQueue;


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

    xZoneMutex = xSemaphoreCreateMutex();
    xAlertQueue = xQueueCreate(10, sizeof(AlertMsg_t));

    for (int i = 0; i < 3; i++) {
        zones[i].zone_id = i + 1;
        zones[i].temperature = 0.0f;
        zones[i].humidity = 0.0f;
        zones[i].gas = 0;
        zones[i].level = ZONE_NORMAL;
    }

    static int zone_ids[3] = {1, 2, 3};
    xTaskCreate(vZoneSensorTask, "SensorTask_Z1", 4096, &zone_ids[0], 2, NULL);
    xTaskCreate(vZoneSensorTask, "SensorTask_Z2", 4096, &zone_ids[1], 2, NULL);
    xTaskCreate(vZoneSensorTask, "SensorTask_Z3", 4096, &zone_ids[2], 2, NULL);
    xTaskCreate(vAlertMngrTask,  "AlertMngrTask", 4096, NULL,  3, NULL);
    xTaskCreate(vDisplayTask,    "DisplayTask",   4096, &lcd, 1, NULL);

    // Keep app_main alive
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
