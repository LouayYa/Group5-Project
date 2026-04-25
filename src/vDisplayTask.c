#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "structs.h"
#include "lcd_i2c.h"

extern ZoneStatus_t zones[3];
extern SemaphoreHandle_t xZoneMutex;
extern volatile system_state_t system_state;

void vDisplayTask(void *pvParameters) {
    lcd_i2c_t *lcd = (lcd_i2c_t *)pvParameters;
    ZoneStatus_t localZones[3];
    char line1[17];
    char line2[17];
    int  active_index = 0;

    while (1) {
        if (xSemaphoreTake(xZoneMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            for (int i = 0; i < 3; i++) localZones[i] = zones[i];
            xSemaphoreGive(xZoneMutex);
        }

        // Collect only zones that are warning or critical
        int active_zones[3];
        int active_count = 0;
        for (int i = 0; i < 3; i++) {
            if (localZones[i].level != ZONE_NORMAL)
                active_zones[active_count++] = i;
        }

        lcd_i2c_clear(lcd);

        if (active_count == 0) {
            lcd_i2c_set_cursor(lcd, 0, 0);
            lcd_i2c_write_str(lcd, "All zones normal");
            lcd_i2c_set_cursor(lcd, 0, 1);
            lcd_i2c_write_str(lcd, "System OK");
            active_index = 0;
        } else {
            if (active_index >= active_count) active_index = 0;
            int i = active_zones[active_index];

            const char *level_str = (localZones[i].level == ZONE_CRITICAL) ? "CRIT" : "WARN";

            snprintf(line1, sizeof(line1), "Zone%d %s",
                     localZones[i].zone_id, level_str);
            snprintf(line2, sizeof(line2), "T:%.1f H:%.0f%%",
                     localZones[i].temperature,
                     localZones[i].humidity);

            lcd_i2c_set_cursor(lcd, 0, 0);
            lcd_i2c_write_str(lcd, line1);
            lcd_i2c_set_cursor(lcd, 0, 1);
            lcd_i2c_write_str(lcd, line2);

            active_index++;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
