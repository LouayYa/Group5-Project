#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

// Temperature thresholds (°C)
#define TEMP_WARN_THRESHOLD     35.0f
#define TEMP_CRIT_THRESHOLD     45.0f

// Humidity thresholds (%)
#define HUM_WARN_THRESHOLD      70.0f
#define HUM_CRIT_THRESHOLD      85.0f

// Gas thresholds (ADC raw, 0–4095)
#define GAS_WARN_THRESHOLD      2000
#define GAS_CRIT_THRESHOLD      3000

// Pass a heap-allocated int* as pvParameters with the zone id (1, 2, or 3)
void vZoneSensorTask(void *pvParameters);

#endif
