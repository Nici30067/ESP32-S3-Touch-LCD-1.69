#include "power.h"
#include "timer.h"
#include "wifi.h"
#include "pin_config.h"
#include <Arduino.h>

HWCDC USBSerial;
int inputPin = 40;   // Define GPIO40 as input
int outputPin = 41;  // Define GPIO41 as output
bool buttonState = false;
bool clickDetected = false;
int backlight_pwm = 0;
int last_touch_time = 0;    //zeit in ms für 60s timeout von Screen
int voltageDividerPin = 1;  // GPIO1 pin
float vRef = 3.3;           // Power supply voltage of ESP32-S3 (unit: volts)
float R1 = 200000.0;        // Resistance value of the first resistor (unit: ohms)
float R2 = 100000.0;
lv_obj_t *bar;
lv_obj_t *bar_volt;
lv_obj_t *label_voltage;
String voltageStr;
unsigned int every_ten_sec_counter = 10;
int32_t display_timer_off_time = 60000;


void check_if_power_button_is_pressed_once() {
  int reading = digitalRead(inputPin);
  if (reading != buttonState) {
    buttonState = reading;

    if (buttonState == LOW) {
      analogWrite(LCD_BL, 0);
      pinMode(LCD_BL, OUTPUT);
      digitalWrite(LCD_BL, HIGH);
      clickDetected = true;
      //esp_sleep_enable_timer_wakeup(10 * 1000000ULL);
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, LOW);
      lv_obj_set_tile(tv, tile1, LV_ANIM_OFF);
      digitalWrite(LCD_BL, LOW);
      delay(500);
      esp_light_sleep_start();
      digitalWrite(LCD_BL, HIGH);  //display aktiv nachdem light sleep beendet
      if (backlight_pwm != 0) {
        analogWrite(LCD_BL, backlight_pwm);
      }
      every_ten_sec_counter = 10;
      last_touch_time = millis();  //60 sec timer wird auch jetzt neu gestartet
    }
    delay(100);  // Used to eliminate button noise
  }
}

//esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
//esp_sleep_pd_config(ESP_PD_DOMAIN_SLOW_MEM, ESP_PD_OPTION_OFF);
//esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
//esp_sleep_enable_timer_wakeup(20000000)
//esp_deep_sleep_start();


void check_lcd_bl_timeout_timer() {
  if (digitalRead(14) == LOW || last_touch_time == 0) {
    last_touch_time = millis();
  }

  if (last_touch_time + display_timer_off_time < millis() && !timer_is_activated && wifi_state == 0 && lv_anim_count_running()<1) {  //60000 ist die dauer des screen timeouts
    analogWrite(LCD_BL, 0);
    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, LOW);
    lv_obj_set_tile(tv, tile1, LV_ANIM_OFF);
    digitalWrite(LCD_BL, LOW);
    delay(500);
    esp_light_sleep_start();
    /*das hier ist neu*/yield(); 
                        delay(50);
    digitalWrite(LCD_BL, HIGH);  //display aktiv nachdem light sleep beendet
    if (backlight_pwm != 0) {
      analogWrite(LCD_BL, backlight_pwm);
    }
    /*lv_timer_handler_run_in_period(10);                   hier bin ich mir nicht so sicher*/
    every_ten_sec_counter = 10;
    last_touch_time = millis();
  }
}
