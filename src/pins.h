#ifndef PINS_H
#define PINS_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/adc.h"

// ── DHT22 Sensors ───────────────────────────────────────────────────
#define DHT1_PIN        GPIO_NUM_23
#define DHT2_PIN        GPIO_NUM_17
#define DHT3_PIN        GPIO_NUM_16

// ── RGB LED 1 (common cathode) ──────────────────────────────────────
#define RGB1_R_PIN      GPIO_NUM_25
#define RGB1_G_PIN      GPIO_NUM_26
#define RGB1_B_PIN      GPIO_NUM_27

// ── RGB LED 2 (common cathode) ──────────────────────────────────────
#define RGB2_R_PIN      GPIO_NUM_14
#define RGB2_G_PIN      GPIO_NUM_2
#define RGB2_B_PIN      GPIO_NUM_13

// ── RGB LED 3 (common cathode) ──────────────────────────────────────
#define RGB3_R_PIN      GPIO_NUM_19
#define RGB3_G_PIN      GPIO_NUM_18
#define RGB3_B_PIN      GPIO_NUM_12

// ── Potentiometers (ADC1) ───────────────────────────────────────────
#define POT1_PIN        GPIO_NUM_34
#define POT1_CHANNEL    ADC1_CHANNEL_6
#define POT2_PIN        GPIO_NUM_35
#define POT2_CHANNEL    ADC1_CHANNEL_7
#define POT3_PIN        GPIO_NUM_32
#define POT3_CHANNEL    ADC1_CHANNEL_4

// ── Servo ───────────────────────────────────────────────────────────
#define SERVO_PIN       GPIO_NUM_4

// ── Buzzer ──────────────────────────────────────────────────────────
#define BUZZER_PIN      GPIO_NUM_15

// ── LCD I2C ─────────────────────────────────────────────────────────
#define LCD_SDA_PIN     GPIO_NUM_21
#define LCD_SCL_PIN     GPIO_NUM_22

// ── Push Button ─────────────────────────────────────────────────────
#define BUTTON_PIN      GPIO_NUM_33

// ── LEDC Channel Assignments ────────────────────────────────────────
// Low-speed mode (Timer 0, 5 kHz, 8-bit): RGB LEDs
#define RGB1_R_CHANNEL  LEDC_CHANNEL_0
#define RGB1_G_CHANNEL  LEDC_CHANNEL_1
#define RGB1_B_CHANNEL  LEDC_CHANNEL_2
#define RGB2_R_CHANNEL  LEDC_CHANNEL_3
#define RGB2_G_CHANNEL  LEDC_CHANNEL_4
#define RGB2_B_CHANNEL  LEDC_CHANNEL_5
#define RGB3_R_CHANNEL  LEDC_CHANNEL_6
#define RGB3_G_CHANNEL  LEDC_CHANNEL_7

// High-speed mode channels
#define SERVO_CHANNEL   LEDC_CHANNEL_0  // Timer 1, 50 Hz, 16-bit
#define BUZZER_CHANNEL  LEDC_CHANNEL_1  // Timer 2, variable freq, 8-bit
#define RGB3_B_CHANNEL  LEDC_CHANNEL_2  // Timer 3, 5 kHz, 8-bit

#endif
