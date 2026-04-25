#ifndef STRUCTS_H
#define STRUCTS_H

typedef enum {
    ZONE_NORMAL = 0,
    ZONE_WARNING = 1,
    ZONE_CRITICAL = 2
} zone_level_t;

typedef struct {
    int zone_id;
    float temperature;
    float humidity;
    int gas;
    zone_level_t level;
} ZoneStatus_t;

typedef struct {
    int zone_id;
    int level;
} AlertMsg_t;

#endif