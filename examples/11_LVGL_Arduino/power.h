#include "HWCDC.h"
#ifndef POWER_H
#define POWER_H
#include "lvgl.h"

extern int inputPin;   // Define GPIO40 as input
extern int outputPin;  // Define GPIO41 as output

extern bool buttonState;
extern bool clickDetected;
extern int backlight_pwm;
extern int last_touch_time;  //zeit in ms für 60s timeout von Screen
extern HWCDC USBSerial;
//ab hier definitions included von voltage.h
extern int voltageDividerPin;  // GPIO1 pin
extern float vRef;                 // Power supply voltage of ESP32-S3 (unit: volts)
extern float R1;              // Resistance value of the first resistor (unit: ohms)
extern float R2; 
extern lv_obj_t *label_for_percentage;
extern lv_obj_t *label_voltage;
extern lv_obj_t *label_percentage;
extern String voltageStr;
extern String calculated_percentage_string;
extern lv_obj_t *bar;
extern lv_obj_t *bar_volt;
extern unsigned int every_ten_sec_counter;
extern int32_t display_timer_off_time;

void check_if_power_button_is_pressed_once();
void check_lcd_bl_timeout_timer();
void every_ten_seconds();


#endif