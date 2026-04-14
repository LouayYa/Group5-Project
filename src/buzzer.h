#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void buzzer_init(void);
void buzzer_tone(uint32_t freq_hz);
void buzzer_off(void);

#endif
