#include "power.h"
#include "SensorQMI8658.hpp"  //lb1
#ifndef QMI_H
#define QMI_H

#define CALIBRATE_ENABLE
#define CALIBRATE_NUM_READINGS 200

extern SensorQMI8658 qmi;  //lb1
extern IMUdata acc;        //lb2
extern IMUdata gyr;        //lb2
extern lv_obj_t *label_start_mpu_6050_button;
extern lv_obj_t *start_mpu_6050_button;

extern float ax_offset, ay_offset, az_offset;
extern float gx_offset, gy_offset, gz_offset;
extern lv_obj_t *acc_data_x;
extern lv_obj_t *acc_data_y;
extern lv_obj_t *acc_data_z;
extern lv_obj_t *gyr_data_x;
extern lv_obj_t *gyr_data_y;
extern lv_obj_t *gyr_data_z;
extern String acc_data_x_string;
extern String acc_data_y_string;
extern String acc_data_z_string;
extern String gyr_data_x_string;
extern String gyr_data_y_string;
extern String gyr_data_z_string;
extern String temp_string;
extern lv_obj_t *label_temperature;

void refresh_qmi_readings_if_active_mpu();
void qmi_on_refresh();
void qmi_on_click_start();
void qmi_setup();
#endif