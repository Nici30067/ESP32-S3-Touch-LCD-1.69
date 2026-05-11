#include <Arduino.h>
#include "ui.h"
#include "qmi.h"

float ax_offset = 0, ay_offset = 0, az_offset = 0;
float gx_offset = 0, gy_offset = 0, gz_offset = 0;
lv_obj_t *acc_data_x;
lv_obj_t *acc_data_y;
lv_obj_t *acc_data_z;
lv_obj_t *gyr_data_x;
lv_obj_t *gyr_data_y;
lv_obj_t *gyr_data_z;
String acc_data_x_string;
String acc_data_y_string;
String acc_data_z_string;
String gyr_data_x_string;
String gyr_data_y_string;
String gyr_data_z_string;
lv_obj_t *start_mpu_6050_button;
lv_obj_t *label_temperature;
lv_obj_t *label_start_mpu_6050_button;
String temp_string;
SensorQMI8658 qmi;  //lb1
IMUdata acc;        //lb2
IMUdata gyr;        //lb2





void refresh_qmi_readings_if_active_mpu() {
  if (qmi_reading_active) {
    //qmi_on_refresh();     PRINT ALL DATA OF QMI
    acc_data_x_string = String((acc.x - ax_offset), 2);  //String, damit label damit beschrieben werden kann. 2 Nachkomma stellen
    acc_data_y_string = String(acc.y - ay_offset, 2);
    acc_data_z_string = String(acc.z - az_offset, 2);
    gyr_data_x_string = String((gyr.x - gx_offset), 2);  //String, damit label damit beschrieben werden kann. 2 Nachkomma stellen
    gyr_data_y_string = String(gyr.y - gy_offset, 2);
    gyr_data_z_string = String(gyr.z - gz_offset, 2);
    lv_label_set_text(acc_data_x, acc_data_x_string.c_str());
    lv_label_set_text(acc_data_y, acc_data_y_string.c_str());
    lv_label_set_text(acc_data_z, acc_data_z_string.c_str());
    lv_label_set_text(gyr_data_x, gyr_data_x_string.c_str());
    lv_label_set_text(gyr_data_y, gyr_data_y_string.c_str());
    lv_label_set_text(gyr_data_z, gyr_data_z_string.c_str());
    //aktivierung wrist based backlight activation
    //  if(((fabs(acc.x-ax_offset))+(fabs(acc.y-ay_offset))+(fabs(acc.z-az_offset)))<=0.45){
    //   if(digitalRead(LCD_BL == LOW)){
    //     digitalWrite(LCD_BL, HIGH);
    //   }
    //       //aktivierungsbedingung: display faced nach oben
    //  }else{
    //   if(digitalRead(LCD_BL == HIGH))
    //       digitalWrite(LCD_BL, LOW);
    //       //aktivierungsbedingung nicht gueltig, was soll gemacht werden
    //  }
  }
}

// void qmi_on_refresh() {

//   if (qmi.getDataReady()) {
//     if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
//       USBSerial.print("{ACCEL: ");
//       USBSerial.print(acc.x);
//       USBSerial.print(",");
//       USBSerial.print(acc.y);
//       USBSerial.print(",");
//       USBSerial.print(acc.z);
//       USBSerial.println("}");
//     }
//     Serial.println("");
//     if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
//       USBSerial.print("{Gyr: ");
//       USBSerial.print(gyr.x);
//       USBSerial.print(",");
//       USBSerial.print(gyr.y);
//       USBSerial.print(",");
//       USBSerial.print(gyr.z);
//       //     sprintf(qmi_data1, "%d", (long)qmi.getTimestamp());
//       //     sprintf(qmi_data2, "%d", (long)qmi.getTemperature_C());
//       //     sprintf(qmi_data3, "%d", (long)acc.x);
//       //     sprintf(qmi_data4, "%d", (long)gyr.x);
//     }
//   }
// }

void qmi_on_click_start() {
#ifdef CALIBRATE_ENABLE
  // Calibrate the MPU6050
  Serial.println("MPU6050 calibrating enabled");
  ///////////////////////////////////////

  float sum_ax = 0, sum_ay = 0, sum_az = 0;
  float sum_gx = 0, sum_gy = 0, sum_gz = 0;

  Serial.println("MPU6050 Calibrating. Please keep the device still...");

  // Take a number of readings to calculate the average
  for (int i = 0; i < CALIBRATE_NUM_READINGS; i++) {

    if (qmi.getDataReady()) {
      if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
        sum_ax += acc.x;
        sum_ay += acc.y;
        sum_az += acc.z;
      }
      if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
        sum_gx += gyr.x;
        sum_gy += gyr.y;
        sum_gz += gyr.z;
      }
    }


    delay(10);
  }

  // calculating gyro offsets
  ax_offset = sum_ax / CALIBRATE_NUM_READINGS;
  ay_offset = sum_ay / CALIBRATE_NUM_READINGS;
  az_offset = sum_az / CALIBRATE_NUM_READINGS;
  gx_offset = sum_gx / CALIBRATE_NUM_READINGS;
  gy_offset = sum_gy / CALIBRATE_NUM_READINGS;
  gz_offset = sum_gz / CALIBRATE_NUM_READINGS;

  Serial.println("MPU6050 Calibration completed.");

  Serial.println("MPU6050 Calibration offsets:");
  Serial.println(
    "MPU6050 dax: " + String(ax_offset) + " day: " + String(ay_offset) + " daz: " + String(az_offset) + " dgx: " + String(gx_offset) + " dgy: " + String(gy_offset) + " dgz: " + String(gz_offset));

  delay(1000);
  //////////////////////////////////////
#else
  Serial.println("MPU6050 calibrating disabled");
#endif
}
void qmi_setup() {

  //Hier gibt es noch mir mehr einstellungen zu:!!!!!!!!!!!!!!!
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
  qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3);

  qmi.enableGyroscope();
  qmi.enableAccelerometer();
  qmi.dumpCtrlRegister();
  USBSerial.print("QMI init successfull; Device ID:");
  USBSerial.println(qmi.getChipID(), HEX);
  USBSerial.println("Reading qmi data now");
}
