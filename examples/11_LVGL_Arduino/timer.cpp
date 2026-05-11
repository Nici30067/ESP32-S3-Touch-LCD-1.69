#include "timer.h"
#include "pin_config.h"
#include <Arduino.h>
#include "ui.h"

SensorPCF85063 rtc;
RTC_DateTime datetime;

bool timer_is_activated = false;
unsigned long start_time = 0;
time_t t;
uint16_t year;
uint8_t month, day, hour, minute, second;
int timer_counter = 0;
char timer_buffer[16];

void timer_ended(lv_timer_t *timer) {
  timer_counter++;
  if (timer_counter >= timer_time) {
    //timer ende
    timer_is_activated = false;
    lv_timer_del(timer);
    int timer_time_minutes = timer_time / 60;
    int timer_time_seconds = timer_time % 60;
    char buffer[32];
    sprintf(buffer, "%02d:%02d", timer_time_minutes, timer_time_seconds);
    lv_label_set_text(timer_application_label, buffer);
    timer_counter = 0;
    lv_obj_add_state(btn_timer_decrease, LV_STATE_DEFAULT);
    lv_obj_clear_state(btn_timer_decrease, LV_STATE_DISABLED);
    lv_obj_add_state(btn_timer_increase, LV_STATE_DEFAULT);
    lv_obj_clear_state(btn_timer_increase, LV_STATE_DISABLED);
    lv_obj_clear_flag(btn_timer_start, LV_OBJ_FLAG_HIDDEN);
    tone(BUZZER, 100, 500);
    delay(10);
  } else {
    //timer noch nicht fertig
    int displayed_time = timer_time - timer_counter;
    int timer_time_minutes = displayed_time / 60;
    int timer_time_seconds = displayed_time % 60;
    char buffer[32];
    sprintf(buffer, "%02d:%02d", timer_time_minutes, timer_time_seconds);
    lv_label_set_text(timer_application_label, buffer);
    lv_timer_reset(timer);
  }
}

//Ab hier system Timer

void system_timer_1_ended(lv_timer_t *timer) {
  chech_wifi_and_fetch();

  if (Serial) {
    last_touch_time = millis();
  }

  every_ten_seconds();

  datetime = rtc.getDateTime();
  char buf[32];
  snprintf(buf, sizeof(buf), "%02d:%02d", datetime.hour, datetime.minute); /*snprintf(buf, sizeof(buf), "%02d:%02d:%02d\n%02d-%02d-%04d",datetime.hour, datetime.minute, datetime.second,datetime.day, datetime.month, datetime.year);*/
  lv_label_set_text(label_for_time, buf);
}
