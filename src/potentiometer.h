#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <stdint.h>

void potentiometer_init(void);
int potentiometer_read(int pot_num);

#endif
