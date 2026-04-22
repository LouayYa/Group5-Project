#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd_i2c.h"
#include "esp_log.h"

extern ZoneStatus_t zones[3];
extern SemaphoreHandle_t xZoneMutex;
extern volatile system_state_t system_state;

void vDisplayTask(void *pvParameters) {
    ZoneStatus_t localZones[3];
    int index = 0;
    char line1[17];
    char line2[17];

    while (1) {
        if (xSemaphoreTake(xZoneMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            for (int i = 0; i < 3; i++) {
                localZones[i] = zones[i];
            }
            xSemaphoreGive(xZoneMutex);
        }

        lcd_clear();

        snprintf(line1, sizeof(line1), "Zone %d T:%.1f G:%d",
                 localZones[index].zone_id,
                 localZones[index].temperature,
                 localZones[index].gas);

        const char *state_str = "NORMAL";
        if (system_state == SYS_ALERT) state_str = "ALERT";
        else if (system_state == SYS_ACKNOWLEDGED) state_str = "ACK";

        const char *level_str = "NORM";
        if (localZones[index].level == ZONE_WARNING) level_str = "WARN";
        else if (localZones[index].level == ZONE_CRITICAL) level_str = "CRIT";

        snprintf(line2, sizeof(line2), "%s %s",
                 level_str,
                 state_str);

        lcd_set_cursor(0, 0);
        lcd_print(line1);

        lcd_set_cursor(0, 1);
        lcd_print(line2);

        index = (index + 1) % 3;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
