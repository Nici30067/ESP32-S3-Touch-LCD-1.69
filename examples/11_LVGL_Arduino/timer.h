#ifndef TIMER_H
#define TIMER_H

#include "SensorPCF85063.hpp"
#include "power.h"
#include "ui.h"

// #include "lvgl.h"
// #include "Arduino_GFX_Library.h"
// #include "Arduino_DriveBus_Library.h"
// #include "pin_config.h"

// #include "lv_conf.h"
// #include <Wire.h>
// #include <SPI.h>

//#include <Arduino.h>
//#include "qmi_8658.h"
//#include "image.h"


#include "lvgl.h"
extern SensorPCF85063 rtc;
extern RTC_DateTime datetime;
extern time_t t;
extern uint16_t year;
extern uint8_t month, day, hour, minute, second;
extern bool timer_is_activated;
extern unsigned long start_time;
extern int timer_counter;
extern char timer_buffer[16];




//lv_obj_t *label;

void timer_ended(lv_timer_t *timer);
void system_timer_1_ended(lv_timer_t *timer);
void chech_wifi_and_fetch();
//void load_ui();

#endif