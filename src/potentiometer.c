#include "potentiometer.h"
#include "pins.h"
#include "driver/adc.h"

void potentiometer_init(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POT1_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(POT2_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(POT3_CHANNEL, ADC_ATTEN_DB_11);
}

int potentiometer_read(int pot_num) {
    switch (pot_num) {
        case 1: return adc1_get_raw(POT1_CHANNEL);
        case 2: return adc1_get_raw(POT2_CHANNEL);
        case 3: return adc1_get_raw(POT3_CHANNEL);
        default: return -1;
    }
}
