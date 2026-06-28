#ifndef UI_H
#define UI_H

#include "lvgl.h"

#define CANVAS_WIDTH 200
#define CANVAS_HEIGHT 150

//evtl bei den ersten alle das extern weg, da ich es spaeter nicht ernerut definiere
extern lv_obj_t *label;
extern lv_obj_t *label_for_time;
extern lv_obj_t *tv;
extern lv_obj_t *timer_application_label;
extern lv_obj_t *label_inside_timer_btn;
extern lv_obj_t *label_turn_off_button;
extern lv_obj_t *label2;
extern lv_obj_t *get_wifi_time_button;
extern lv_obj_t *get_wifi_time_button_label;
extern lv_obj_t *chart;
extern lv_style_t style_btn_transp;
extern lv_obj_t *label_slider_backlight;
extern lv_chart_series_t *series;
extern bool qmi_reading_active;
extern lv_timer_t *timer;
extern int timer_time;
extern lv_obj_t *label_display_off_after_one_min;
extern lv_obj_t *label_display_off_after_five_min;
extern lv_obj_t *label_display_never_off;
extern lv_obj_t *label_display_off_settings;
extern lv_obj_t *btn_timer_increase;
extern lv_obj_t *btn_timer_decrease;
extern lv_timer_t *system_timer_1;
extern lv_obj_t *btn_timer_start;
extern lv_obj_t *tile1;






void load_ui();
void event_handler_timer_button(lv_event_t *e);
void event_handler_start_mpu_6050_button(lv_event_t *e);
void slider_backlight_event_cb(lv_event_t *e);
void event_handler_get_wifi_time_button(lv_event_t *e);
void event_handler_turnoff_button(lv_event_t *e);
void event_handler_timer_increase_button(lv_event_t *e);
void event_handler_timer_decrease_button(lv_event_t *e);
void event_handler_display_timer_off_1(lv_event_t *e);
void event_handler_display_timer_off_2(lv_event_t *e);
void event_handler_display_timer_off_3(lv_event_t *e);
void format_y_axis_cb(lv_event_t *e);
static void set_value_pomodoro(void * indic, int32_t v);
void start_pomodoro_timer(lv_event_t *e);
// void set_value_pomodoro_end_cb(lv_event_t *e);
static void roller_mask_event_cb(lv_event_t * e);
void event_handler_label_qmi_refresh(lv_event_t *e);

#endif