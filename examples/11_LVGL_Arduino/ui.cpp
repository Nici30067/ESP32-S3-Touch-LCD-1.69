#include <Arduino.h>
#include <lvgl.h>
#include "ui.h"
#include "colors.h"
#include "wifi.h"
#include "pin_config.h"
#include "power.h"
#include "timer.h"
#include "image.h"
#include "qmi.h"


bool qmi_reading_active = false;
int timer_time = 15;
int pomodoro_timer_time;
lv_obj_t *tv;
lv_obj_t *label_inside_timer_btn;
lv_obj_t *label2;
lv_obj_t *get_wifi_time_button;
lv_obj_t *get_wifi_time_button_label;
lv_obj_t *label_turn_off_button;
lv_obj_t *timer_application_label;
lv_obj_t *label_for_time;
lv_obj_t *label_slider_backlight;
lv_obj_t *chart;
lv_obj_t *fake_chart;
lv_chart_series_t *ser2;
lv_timer_t *timer;
lv_timer_t *system_timer_1;
lv_obj_t *label_display_off_after_one_min;
lv_obj_t *label_display_off_after_five_min;
lv_obj_t *label_display_never_off;
lv_obj_t *label_display_off_settings;
lv_style_t style_btn_transp;
lv_obj_t *btn_timer_start;
lv_obj_t *btn_timer_increase;
lv_obj_t *btn_timer_decrease;
static lv_style_t clicked_btn_style;
static lv_style_t default_btn_style;
lv_obj_t *button_display_off_after_one_min;
lv_obj_t *button_display_off_after_five_min;
lv_obj_t *button_display_never_off;
lv_obj_t *meter;  //meter fuer pomodoro timer
lv_anim_t a;      //animation pomodoro timer
lv_obj_t *roller1;
lv_obj_t *tile1;




void load_ui() {

  tv = lv_tileview_create(lv_scr_act());
  lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

  /*Tile1: just a label*/
  tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM | LV_DIR_RIGHT);
  lv_obj_set_style_bg_color(tile1, lv_color_make(0, 0, 0), 0);
  lv_obj_set_style_bg_opa(tile1, LV_OPA_100, 0);
  /*Tile1: Background color mix outside of time*/
  lv_draw_rect_dsc_t rect_dsc;
  lv_draw_rect_dsc_init(&rect_dsc);
  rect_dsc.radius = 10;
  rect_dsc.bg_opa = LV_OPA_COVER;
  rect_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;
  rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
  rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
  static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];
  lv_obj_t *canvas = lv_canvas_create(tile1);
  lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_center(canvas);
  lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);
  /*Tile1: time label and background*/
  int canvas_width = 160;
  lv_canvas_draw_rect(canvas, ((200 - canvas_width) / 2), 0, canvas_width, 70, &rect_dsc);
  lv_obj_t *bkgrnd = lv_obj_create(tile1);
  lv_obj_set_width(bkgrnd, 150);
  lv_obj_set_height(bkgrnd, 60);
  lv_obj_set_pos(bkgrnd, 45, 70);
  lv_obj_set_style_bg_color(bkgrnd, lv_color_hex(0x123456), BLACK);
  lv_obj_set_style_border_width(bkgrnd, 0, 0);
  label_for_time = lv_label_create(tile1);
  lv_label_set_recolor(label_for_time, true);                                        /*Enable re-coloring by commands in the text*/
  lv_obj_set_style_text_font(label_for_time, &lv_font_montserrat_40, LV_PART_MAIN);  //&lv_font_montserrat_40
  lv_obj_center(label_for_time);
  lv_obj_set_y(label_for_time, -40);
  lv_obj_set_style_text_color(label_for_time, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

  /*Tile1: temperature label*/
  label_temperature = lv_label_create(tile1);
  lv_label_set_text(label_temperature, "loading...");
  lv_obj_set_pos(label_temperature, 80, 160);
  lv_obj_set_style_text_color(label_temperature, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  /*Tile1: voltage label */
  label_voltage = lv_label_create(tile1);
  lv_label_set_recolor(label_voltage, true);
  lv_obj_set_pos(label_voltage, 120, 240);
  lv_obj_set_style_text_color(label_voltage, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  //label_percentage = lv_label_create(tile1);
  //lv_label_set_recolor(label_percentage, true);
  //lv_obj_set_pos(label_percentage, 50, 230);
  /*Tile1: TEMPERATUR BAR*/
  static lv_style_t style_indic;
  lv_style_init(&style_indic);
  lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
  lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
  lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);
  bar = lv_bar_create(tile1);
  lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
  lv_obj_set_size(bar, 20, 110);
  lv_obj_center(bar);
  lv_obj_set_y(bar, 70);
  lv_obj_set_x(bar, -65);
  lv_bar_set_range(bar, 20, 50);
  /*Tile1: VOLTAGE BAR*/
  static lv_style_t style_indic_volt;
  lv_style_init(&style_indic_volt);
  lv_style_set_bg_opa(&style_indic_volt, LV_OPA_COVER);
  lv_style_set_bg_color(&style_indic_volt, lv_palette_main(LV_PALETTE_GREEN));
  lv_style_set_bg_grad_color(&style_indic_volt, lv_palette_main(LV_PALETTE_GREY));
  lv_style_set_bg_grad_dir(&style_indic_volt, LV_GRAD_DIR_VER);
  bar_volt = lv_bar_create(tile1);
  lv_obj_add_style(bar_volt, &style_indic_volt, LV_PART_INDICATOR);
  lv_obj_set_size(bar_volt, 20, 110);
  lv_obj_center(bar_volt);
  lv_obj_set_y(bar_volt, 70);
  lv_obj_set_x(bar_volt, 65);
  lv_bar_set_range(bar_volt, 3500, 4200);  //kann nur ints annehmen also so hier

  /*Tile 1.1 Rechts Energiestatistik*/
  lv_obj_t *tile1_1 = lv_tileview_add_tile(tv, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
  //fake chart
  fake_chart = lv_chart_create(tile1_1);
  lv_obj_set_size(fake_chart, 180, 240);  //240,280
  lv_obj_center(fake_chart);
  lv_obj_set_pos(fake_chart, 20, 0);
  lv_chart_set_type(fake_chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
  lv_chart_set_axis_tick(fake_chart, LV_CHART_AXIS_PRIMARY_Y, 8, 3, 6, 2, true, 50);
  lv_chart_set_range(fake_chart, LV_CHART_AXIS_PRIMARY_Y, 34, 38);  //von 38 bis 45 grad -> besser wäre dynamische anpassung
  lv_chart_refresh(fake_chart);


  chart = lv_chart_create(tile1_1);
  lv_obj_set_size(chart, 180, 240);  //240,280
  lv_obj_center(chart);
  lv_obj_set_pos(chart, 20, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 8, 3, 6, 2, false, 50);
  /*Add two data series*/
  ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 340, 380);  //von 38 bis 45 grad -> besser wäre dynamische anpassung
  lv_chart_set_point_count(chart, 20);

  /*Directly set points on 'ser2'*/
  for (int l = 0; l < 20; l++) {
    ser2->y_points[l] = 380;
  }
  lv_chart_refresh(chart); /*Required after direct set*/



  /*TILE 1.2 Display off timer einstellen*/
  lv_obj_t *tile1_2 = lv_tileview_add_tile(tv, 2, 0, LV_DIR_LEFT);
  button_display_off_after_one_min = lv_btn_create(tile1_2);
  lv_obj_center(button_display_off_after_one_min);
  lv_obj_set_pos(button_display_off_after_one_min, -65, 60);
  lv_obj_set_size(button_display_off_after_one_min, 50, 40);
  lv_obj_add_event_cb(button_display_off_after_one_min, event_handler_display_timer_off_1, LV_EVENT_CLICKED, NULL);
  label_display_off_after_one_min = lv_label_create(button_display_off_after_one_min);
  lv_label_set_text(label_display_off_after_one_min, "15s");
  lv_obj_center(label_display_off_after_one_min);

  button_display_off_after_five_min = lv_btn_create(tile1_2);
  lv_obj_center(button_display_off_after_five_min);
  lv_obj_set_pos(button_display_off_after_five_min, -5, 60);
  lv_obj_set_size(button_display_off_after_five_min, 50, 40);
  lv_obj_add_event_cb(button_display_off_after_five_min, event_handler_display_timer_off_2, LV_EVENT_CLICKED, NULL);
  label_display_off_after_five_min = lv_label_create(button_display_off_after_five_min);
  lv_label_set_text(label_display_off_after_five_min, "5m");
  lv_obj_center(label_display_off_after_five_min);

  button_display_never_off = lv_btn_create(tile1_2);
  lv_obj_center(button_display_never_off);
  lv_obj_set_pos(button_display_never_off, 60, 60);
  lv_obj_set_size(button_display_never_off, 50, 40);
  lv_obj_add_event_cb(button_display_never_off, event_handler_display_timer_off_3, LV_EVENT_CLICKED, NULL);
  label_display_never_off = lv_label_create(button_display_never_off);
  lv_label_set_text(label_display_never_off, "never");
  lv_obj_center(label_display_never_off);

  label_display_off_settings = lv_label_create(tile1_2);
  lv_label_set_text(label_display_off_settings, "Display off timer");
  lv_obj_center(label_display_off_settings);
  lv_obj_set_y(label_display_off_settings, -100);
  //

  //button styles pressed
  lv_style_init(&clicked_btn_style);
  lv_style_set_bg_color(&clicked_btn_style, lv_palette_main(LV_PALETTE_RED));

  //button styles default
  lv_style_init(&default_btn_style);
  lv_style_set_bg_color(&default_btn_style, lv_palette_main(LV_PALETTE_RED));


  /*TILE 2: Button*/
  lv_obj_t *tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_TOP | LV_DIR_BOTTOM);
  btn_timer_start = lv_btn_create(tile2);
  label_inside_timer_btn = lv_label_create(btn_timer_start);
  lv_label_set_text(label_inside_timer_btn, "Timer Starten");
  lv_obj_set_size(label_inside_timer_btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_center(label_inside_timer_btn);
  /*Tile2: Style button*/
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_radius(&style, 40);
  lv_style_set_bg_opa(&style, LV_OPA_COVER);
  lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_BLUE, 1));
  lv_style_set_shadow_width(&style, 55);
  lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_BLUE));
  //    lv_style_set_shadow_ofs_x(&style, 10);
  //    lv_style_set_shadow_ofs_y(&style, 20);
  lv_obj_add_style(btn_timer_start, &style, 0);
  lv_obj_center(btn_timer_start);
  lv_obj_set_pos(btn_timer_start, 0, 100);
  lv_obj_set_size(btn_timer_start, 180, 50);
  lv_obj_add_event_cb(btn_timer_start, event_handler_timer_button, LV_EVENT_CLICKED, NULL);
  /*Label 2 Timer information label*/
  timer_application_label = lv_label_create(tile2);
  lv_obj_set_style_text_font(timer_application_label, &lv_font_montserrat_40, LV_PART_MAIN);

  lv_label_set_text(timer_application_label, "00:15");
  lv_obj_center(timer_application_label);
  lv_obj_set_pos(timer_application_label, 0, -40);

  lv_style_init(&style_btn_transp);                       // Initialize the style
  lv_style_set_bg_opa(&style_btn_transp, LV_OPA_TRANSP);  // Set background opacity to transparent
  lv_style_set_border_width(&style_btn_transp, 0);        // No border
  lv_style_set_shadow_width(&style_btn_transp, 0);

  btn_timer_increase = lv_btn_create(tile2);
  //lv_obj_add_style(btn_timer_start, &style, 0);
  lv_obj_center(btn_timer_increase);
  lv_obj_set_pos(btn_timer_increase, 60, -40);
  lv_obj_set_size(btn_timer_increase, 120, 190);
  lv_obj_add_event_cb(btn_timer_increase, event_handler_timer_increase_button, LV_EVENT_ALL, NULL);
  lv_obj_add_style(btn_timer_increase, &style_btn_transp, LV_PART_MAIN);

  btn_timer_decrease = lv_btn_create(tile2);
  //lv_obj_add_style(btn_timer_start, &style, 0);
  lv_obj_center(btn_timer_decrease);
  lv_obj_set_pos(btn_timer_decrease, -60, -40);
  lv_obj_set_size(btn_timer_decrease, 120, 190);
  lv_obj_add_event_cb(btn_timer_decrease, event_handler_timer_decrease_button, LV_EVENT_ALL, NULL);
  lv_obj_add_style(btn_timer_decrease, &style_btn_transp, LV_PART_MAIN);

  /*TILE 3 Accellerometer*/
  lv_obj_t *tile3 = lv_tileview_add_tile(tv, 0, 2, LV_DIR_TOP | LV_DIR_BOTTOM);
  lv_obj_set_style_bg_color(tile3, lv_color_make(0, 0, 0), 0);
  lv_obj_set_style_bg_opa(tile3, LV_OPA_100, 0);
  label2 = lv_label_create(tile3);
  lv_label_set_recolor(label2, true); /*Enable re-coloring by commands in the text*/
  lv_label_set_text(label2, "#ffffff Accellerometer Data");
  lv_obj_center(label2);
  lv_obj_set_y(label2, -100);
  start_mpu_6050_button = lv_btn_create(tile3);
  lv_obj_add_event_cb(start_mpu_6050_button, event_handler_start_mpu_6050_button, LV_EVENT_CLICKED, NULL);
  lv_obj_center(start_mpu_6050_button);
  lv_obj_set_y(start_mpu_6050_button, 90);
  lv_obj_set_size(start_mpu_6050_button, 170, 60);
  label_start_mpu_6050_button = lv_label_create(start_mpu_6050_button);
  lv_label_set_text(label_start_mpu_6050_button, "Start MPU");
  lv_obj_center(label_start_mpu_6050_button);


  //labels for acc data
  acc_data_x = lv_label_create(tile3);
  lv_label_set_text(acc_data_x, "X");
  lv_obj_center(acc_data_x);
  lv_obj_set_x(acc_data_x, -70);
  lv_obj_set_y(acc_data_x, -70);
  lv_obj_set_style_text_color(acc_data_x, lv_color_hex(0xffffff), LV_PART_MAIN);

  acc_data_y = lv_label_create(tile3);
  lv_label_set_text(acc_data_y, "Y");
  lv_obj_center(acc_data_y);
  lv_obj_set_style_text_color(acc_data_y, lv_color_hex(0xffffff), 0);
  lv_obj_set_y(acc_data_y, -70);

  acc_data_z = lv_label_create(tile3);
  lv_label_set_text(acc_data_z, "Z");
  lv_obj_center(acc_data_z);
  lv_obj_set_x(acc_data_z, 70);
  lv_obj_set_style_text_color(acc_data_z, lv_color_hex(0xffffff), 0);
  lv_obj_set_y(acc_data_z, -70);

  gyr_data_x = lv_label_create(tile3);
  lv_label_set_text(gyr_data_x, "X");
  lv_obj_center(gyr_data_x);
  lv_obj_set_x(gyr_data_x, -70);
  lv_obj_set_y(gyr_data_x, -20);
  lv_obj_set_style_text_color(gyr_data_x, lv_color_hex(0xffffff), LV_PART_MAIN);

  gyr_data_y = lv_label_create(tile3);
  lv_label_set_text(gyr_data_y, "Y");
  lv_obj_center(gyr_data_y);
  lv_obj_set_style_text_color(gyr_data_y, lv_color_hex(0xffffff), 0);
  lv_obj_set_y(gyr_data_y, -20);

  gyr_data_z = lv_label_create(tile3);
  lv_label_set_text(gyr_data_z, "Z");
  lv_obj_center(gyr_data_z);
  lv_obj_set_x(gyr_data_z, 70);
  lv_obj_set_style_text_color(gyr_data_z, lv_color_hex(0xffffff), 0);
  lv_obj_set_y(gyr_data_z, -20);
  //test: print only if label is shown
  lv_obj_add_event_cb(gyr_data_z, event_handler_label_qmi_refresh, LV_EVENT_ALL, NULL);




  /*TILE 4 SETTINGS*/
  //idea: slider mit label da drüber der value anzeigt von 2 bis 100 % Bildschirmhelligkeit
  lv_obj_t *tile4 = lv_tileview_add_tile(tv, 0, 3, LV_DIR_TOP | LV_DIR_BOTTOM);
  lv_obj_t *slider_backlight = lv_slider_create(tile4);
  lv_obj_set_width(slider_backlight, 200); /*Set the width*/
  lv_obj_set_height(slider_backlight, 25);
  lv_obj_center(slider_backlight);                                                                /*Align to the center of the parent (screen)*/
  lv_obj_add_event_cb(slider_backlight, slider_backlight_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /*Assign an event function*/
  lv_slider_set_range(slider_backlight, 1, 10);
  lv_obj_set_y(slider_backlight, -50);

  /*Create a label above the slider*/
  label_slider_backlight = lv_label_create(tile4);
  lv_label_set_text(label_slider_backlight, "10");
  lv_obj_align_to(label_slider_backlight, slider_backlight, LV_ALIGN_OUT_TOP_MID, 0, 0);

  get_wifi_time_button = lv_btn_create(tile4);
  lv_obj_add_event_cb(get_wifi_time_button, event_handler_get_wifi_time_button, LV_EVENT_CLICKED, NULL);
  lv_obj_center(get_wifi_time_button);
  lv_obj_set_y(get_wifi_time_button, 70);
  lv_obj_set_size(get_wifi_time_button, 150, 60);

  get_wifi_time_button_label = lv_label_create(get_wifi_time_button);
  lv_label_set_text(get_wifi_time_button_label, "Ready for pull");
  lv_obj_center(get_wifi_time_button_label);


  /*TILE 5 Turn off*/
  lv_obj_t *tile5 = lv_tileview_add_tile(tv, 0, 4, LV_DIR_TOP | LV_DIR_BOTTOM);
  lv_obj_t *turn_off_button = lv_btn_create(tile5);
  lv_obj_add_event_cb(turn_off_button, event_handler_turnoff_button, LV_EVENT_CLICKED, NULL);
  label_turn_off_button = lv_label_create(turn_off_button);
  lv_label_set_text(label_turn_off_button, "Turn off");
  lv_obj_center(label_turn_off_button);
  lv_obj_center(turn_off_button);

  /*TILE 6   IMAGE
  lv_obj_t *tile6 = lv_tileview_add_tile(tv, 0, 5, LV_DIR_TOP);
  lv_obj_t *img_obj = lv_img_create(tile6);
  lv_img_set_src(img_obj, &crop_img);  // Set the image source to img_test3
  lv_obj_align(img_obj, LV_ALIGN_CENTER, 0, 0);*/

  //MINIMALISTIC POMODORO TIMER
  lv_obj_t *tile6 = lv_tileview_add_tile(tv, 0, 5, LV_DIR_TOP | LV_DIR_RIGHT);
  lv_obj_set_style_bg_color(tile6, lv_color_make(0, 0, 0), 0);
  lv_obj_set_style_bg_opa(tile6, LV_OPA_100, 0);
  meter = lv_meter_create(tile6);
  /*Remove the background and the circle from the middle*/
  lv_obj_remove_style(meter, NULL, LV_PART_MAIN);
  lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);
  lv_obj_set_size(meter, 190, 190);
  lv_obj_center(meter);
  lv_obj_add_event_cb(meter, start_pomodoro_timer, LV_EVENT_CLICKED, NULL);
  /*Add a scale first with no ticks.*/
  lv_meter_scale_t *scale = lv_meter_add_scale(meter);
  lv_meter_set_scale_ticks(meter, scale, 0, 0, 0, lv_color_black());
  lv_meter_set_scale_range(meter, scale, 0, 360, 360, 0);
  /*Add a three arc indicator*/
  lv_coord_t indic_w = 300;
  lv_meter_indicator_t *indic1 = lv_meter_add_arc(meter, scale, indic_w, lv_color_white(), 0);
  lv_meter_set_indicator_start_value(meter, indic1, 0);
  lv_meter_set_indicator_end_value(meter, indic1, 360);
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, set_value_pomodoro);
  //lv_anim_set_deleted_cb(&a, set_value_pomodoro_end_cb);
  lv_anim_set_values(&a, 360, 0);
  lv_anim_set_repeat_delay(&a, 100);
  lv_anim_set_playback_delay(&a, 1000);
  lv_anim_set_repeat_count(&a, 1);  //LV_ANIM_REPEAT_INFINITE
  lv_anim_set_time(&a, pomodoro_timer_time);
  lv_anim_set_playback_time(&a, 500);
  lv_anim_set_var(&a, indic1);

  /*TILE 7 SELECT TIME*/
  lv_obj_t *tile6_2 = lv_tileview_add_tile(tv, 1, 5, LV_DIR_LEFT);

  lv_obj_set_style_bg_color(tile6_2, lv_color_make(0, 0, 0), 0);
  lv_obj_set_style_bg_opa(tile6_2, LV_OPA_100, 0);
  static lv_style_t style_roller;
  lv_style_init(&style_roller);
  lv_style_set_bg_color(&style_roller, lv_color_make(0, 0, 0));   //lv_color_black()
  lv_style_set_text_color(&style_roller, lv_color_white());
  lv_style_set_border_width(&style_roller, 0);
  lv_style_set_pad_all(&style_roller, 0);
  lv_obj_add_style(tile6_2, &style_roller, 0);
  roller1 = lv_roller_create(tile6_2);
  lv_obj_add_style(roller1, &style_roller, 0);
  lv_obj_set_style_bg_opa(roller1, LV_OPA_TRANSP, LV_PART_SELECTED);
#if LV_FONT_MONTSERRAT_22
  lv_obj_set_style_text_font(roller1, &lv_font_montserrat_30, LV_PART_SELECTED);
#endif
  lv_roller_set_options(roller1, "1\n"
                                 "5\n"
                                 "10\n"
                                 "15\n"
                                 "20\n"
                                 "30\n"
                                 "45\n"
                                 "60\n",
                        LV_ROLLER_MODE_NORMAL);
  lv_obj_center(roller1);
  lv_obj_set_size(roller1, 300, 360);
  lv_roller_set_selected(roller1, 3, LV_ANIM_OFF);
  lv_roller_set_visible_row_count(roller1, 3);
  lv_obj_add_event_cb(roller1, roller_mask_event_cb, LV_EVENT_ALL, NULL);


  //Start system timer
  static uint32_t user_data = 0;
  system_timer_1 = lv_timer_create(system_timer_1_ended, 1000, &user_data);
  lv_timer_set_repeat_count(system_timer_1, -1);
}

static void roller_mask_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  static int16_t mask_top_id = -1;
  static int16_t mask_bottom_id = -1;

  if (code == LV_EVENT_COVER_CHECK) {
    lv_event_set_cover_res(e, LV_COVER_RES_MASKED);

  } else if (code == LV_EVENT_DRAW_MAIN_BEGIN) {
    /* add mask */
    const lv_font_t *font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t font_h = lv_font_get_line_height(font);

    lv_area_t roller_coords;
    lv_obj_get_coords(obj, &roller_coords);

    lv_area_t rect_area;
    rect_area.x1 = roller_coords.x1;
    rect_area.x2 = roller_coords.x2;
    rect_area.y1 = roller_coords.y1;
    rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - font_h - line_space) / 2;

    lv_draw_mask_fade_param_t *fade_mask_top = (lv_draw_mask_fade_param_t *)lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
    lv_draw_mask_fade_init(fade_mask_top, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
    mask_top_id = lv_draw_mask_add(fade_mask_top, NULL);

    rect_area.y1 = rect_area.y2 + font_h + line_space - 1;
    rect_area.y2 = roller_coords.y2;

    lv_draw_mask_fade_param_t *fade_mask_bottom = (lv_draw_mask_fade_param_t *)lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
    lv_draw_mask_fade_init(fade_mask_bottom, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
    mask_bottom_id = lv_draw_mask_add(fade_mask_bottom, NULL);

  } else if (code == LV_EVENT_DRAW_POST_END) {
    lv_draw_mask_fade_param_t *fade_mask_top = (lv_draw_mask_fade_param_t *)lv_draw_mask_remove_id(mask_top_id);
    lv_draw_mask_fade_param_t *fade_mask_bottom = (lv_draw_mask_fade_param_t *)lv_draw_mask_remove_id(mask_bottom_id);
    lv_draw_mask_free_param(fade_mask_top);
    lv_draw_mask_free_param(fade_mask_bottom);
    lv_mem_buf_release(fade_mask_top);
    lv_mem_buf_release(fade_mask_bottom);
    mask_top_id = -1;
    mask_bottom_id = -1;
  }
}

static void set_value_pomodoro(void *indic, int32_t v) {
  lv_meter_set_indicator_end_value(meter, (lv_meter_indicator_t *)indic, v);
}
void start_pomodoro_timer(lv_event_t *e) {
  switch (lv_roller_get_selected(roller1)) {
    case 0: pomodoro_timer_time = 1; break;
    case 1: pomodoro_timer_time = 5; break;
    case 2: pomodoro_timer_time = 10; break;
    case 3: pomodoro_timer_time = 15; break;
    case 4: pomodoro_timer_time = 20; break;
    case 5: pomodoro_timer_time = 30; break;
    case 6: pomodoro_timer_time = 45; break;
    case 7: pomodoro_timer_time = 60; break;
  }
  lv_anim_set_time(&a, (pomodoro_timer_time * 60000));  //pomodoro_timer_time
  lv_anim_start(&a);                                    //startet die animation des pomodoro timer
  //display off erst viel spaeter machen
  last_touch_time = millis();
}

void event_handler_label_qmi_refresh(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_DRAW_MAIN) {
    USBSerial.println("jetzt ist es sichtbar");
  }
}

void event_handler_timer_button(lv_event_t *e) {
  if (!timer_is_activated) {
    timer_is_activated = true;
    lv_obj_add_state(btn_timer_decrease, LV_STATE_DISABLED);
    lv_obj_clear_state(btn_timer_decrease, LV_STATE_DEFAULT);
    lv_obj_add_state(btn_timer_increase, LV_STATE_DISABLED);
    lv_obj_clear_state(btn_timer_increase, LV_STATE_DEFAULT);
    lv_obj_add_flag(btn_timer_start, LV_OBJ_FLAG_HIDDEN);
    static uint32_t user_data = 0;  // Not sure if I need to use this...
    timer = lv_timer_create(timer_ended, 1000, &user_data);
  }
}

void event_handler_timer_increase_button(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_SCROLL) {
    //pressing, pressed, pressed repeat
  } else {
    if (code == LV_EVENT_LONG_PRESSED_REPEAT || code == LV_EVENT_CLICKED) {
      if (code == LV_EVENT_LONG_PRESSED_REPEAT) {
        timer_time = timer_time + 10;
      } else if (code == LV_EVENT_CLICKED) {
        timer_time = timer_time + 5;
      }
      int timer_time_minutes = timer_time / 60;
      int timer_time_seconds = timer_time % 60;
      char buffer[32];
      sprintf(buffer, "%02d:%02d", timer_time_minutes, timer_time_seconds);
      lv_label_set_text(timer_application_label, buffer);
    }
  }
}

void event_handler_timer_decrease_button(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_SCROLL) {

  } else {
    if (code == LV_EVENT_LONG_PRESSED_REPEAT || code == LV_EVENT_CLICKED) {
      if (code == LV_EVENT_LONG_PRESSED_REPEAT) {
        if (timer_time > 10) {
          timer_time = timer_time - 10;
        }
      } else if (code == LV_EVENT_CLICKED) {
        if (timer_time > 5) {
          timer_time = timer_time - 5;
        }
      }
      int timer_time_minutes = timer_time / 60;
      int timer_time_seconds = timer_time % 60;
      char buffer[32];
      sprintf(buffer, "%02d:%02d", timer_time_minutes, timer_time_seconds);
      lv_label_set_text(timer_application_label, buffer);
    }
  }
}

void event_handler_display_timer_off_1(lv_event_t *e) {
  display_timer_off_time = 15000;  //15 sekunden
  lv_obj_add_style(button_display_off_after_one_min, &clicked_btn_style, 0);
  lv_obj_remove_style(button_display_off_after_five_min, &clicked_btn_style, 0);
  lv_obj_remove_style(button_display_never_off, &clicked_btn_style, 0);
}
void event_handler_display_timer_off_2(lv_event_t *e) {
  display_timer_off_time = 60000;  // 1 min
  lv_obj_add_style(button_display_off_after_five_min, &clicked_btn_style, 0);
  lv_obj_remove_style(button_display_off_after_one_min, &clicked_btn_style, 0);
  lv_obj_remove_style(button_display_never_off, &clicked_btn_style, 0);
}
void event_handler_display_timer_off_3(lv_event_t *e) {
  display_timer_off_time = 36000000;  //10 stunden
  lv_obj_add_style(button_display_never_off, &clicked_btn_style, 0);
  lv_obj_remove_style(button_display_off_after_one_min, &clicked_btn_style, 0);
  lv_obj_remove_style(button_display_off_after_five_min, &clicked_btn_style, 0);
}

void event_handler_start_mpu_6050_button(lv_event_t *e) {
  if (strcmp(lv_label_get_text(label_start_mpu_6050_button), "Start MPU") == 0) {
    lv_label_set_text(label_start_mpu_6050_button, "Button pressed");
    qmi_on_click_start();
    qmi_reading_active = true;
    lv_label_set_text(label_start_mpu_6050_button, "Stop MPU");

  } else if (strcmp(lv_label_get_text(label_start_mpu_6050_button), "Stop MPU") == 0) {
    qmi_reading_active = false;
    lv_label_set_text(label_start_mpu_6050_button, "Start MPU");

  } else {
    lv_label_set_text(label_start_mpu_6050_button, "Not equally");
  }
}
void slider_backlight_event_cb(lv_event_t *e) {
  lv_obj_t *slider_backlight = lv_event_get_target(e);
  /*Refresh the text*/
  lv_label_set_text_fmt(label_slider_backlight, "%" LV_PRId32, lv_slider_get_value(slider_backlight));
  backlight_pwm = lv_slider_get_value(slider_backlight);
  backlight_pwm = backlight_pwm * 256 / 10;
  USBSerial.println(backlight_pwm);
  analogWrite(LCD_BL, backlight_pwm);  //set led pin pwm value  ( max 256)
}
void event_handler_turnoff_button(lv_event_t *e) {
  //lv_event_code_t code = lv_event_get_code(e);

  // if (code == LV_EVENT_CLICKED) {
  //  esp_deep_sleep_start();
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  //  //esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
  // // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);
  // esp_sleep_enable_timer_wakeup(20000000);
  esp_deep_sleep_start();

}

//}
