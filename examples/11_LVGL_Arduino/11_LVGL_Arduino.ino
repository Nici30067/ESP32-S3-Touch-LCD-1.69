#include "lvgl.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "lv_conf.h"
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
//#include "SensorPCF85063.hpp"
#include "image.h"
#include "colors.h"
#include "power.h"
#include "qmi.h"
#include "timer.h"
#include "wifi.h"
#include "ui.h"

#define USE_WIRE

#ifndef SENSOR_IRQ
#define SENSOR_IRQ -1
#endif
#define IMU_CS 5



Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);  //4 Schnittstellen DATEN
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 0);
std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus = std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);
std::unique_ptr<Arduino_IIC> CST816T(new Arduino_CST816x(IIC_Bus, CST816T_DEVICE_ADDRESS, TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void) {
  CST816T->IIC_Interrupt_Flag = true;
}
#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t lastMillis;

static lv_disp_draw_buf_t draw_buf;
//static lv_color_t buff[LCD_WIDTH * LCD_HEIGHT / 10];  //included seit time example
// static lv_color_t buf[screenWidth * screenHeight / 10];

int elapsed_time = 0;
float y_points[20];

#if LV_USE_LOG != 0
/* USBSerial debugging */
void my_print(const char *buf) {
  USBSerial.printf(buf);
  USBSerial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void example_increase_lvgl_tick(void *arg) {
  /* Tell LVGL how many milliseconds has elapsed */
  lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static uint8_t count = 0;
void example_increase_reboot(void *arg) {  //reboot wenn er sich aufhängt
  count++;
  if (count == 30) {
    esp_restart();
  }
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {  //Void get TOUCHPAD data
  int32_t touchX = CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
  int32_t touchY = CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

  if (CST816T->IIC_Interrupt_Flag == true) {
    CST816T->IIC_Interrupt_Flag = false;
    data->state = LV_INDEV_STATE_PR;

    /* Set the coordinates with some debounce */
    if (touchX >= 0 && touchY >= 0) {
      data->point.x = touchX;
      data->point.y = touchY;

      USBSerial.printf("Data x: %d, Data y: %d\n", touchX, touchY);
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}
/*
              /$$$$$$  /$$$$$$$$ /$$$$$$  /$$$$$$$  /$$$$$$$$
            /$$__  $$|__  $$__//$$__  $$| $$__  $$|__  $$__/
            | $$  \__/   | $$  | $$  \ $$| $$  \ $$   | $$   
            |  $$$$$$    | $$  | $$$$$$$$| $$$$$$$/   | $$   
            \____  $$   | $$  | $$__  $$| $$__  $$   | $$   
            /$$  \ $$   | $$  | $$  | $$| $$  \ $$   | $$   
            |  $$$$$$/   | $$  | $$  | $$| $$  | $$   | $$   
            \______/    |__/  |__/  |__/|__/  |__/   |__/   
*/
/*
                  Ziel: Smartwacht bauen!
                  TILE ANIMATION CALLBACK
                -refresh für qmi rein in callback
      energiestatistik Chart: alle striche entfernen und Zahlen weiter rechts
      -irgendwas mit dem buzzer
      -alles raus aus loop und rein in callbacks(als event bsp tile focused)
      -LIGHT SLEEP FORCED WAKE UP DAMIT ER NICHT UNTER THRESHOLD 3.5V LÄUFT 
-moisture tolerance : https://files.waveshare.com/wiki/common/Esp32-s3_technical_reference_manual_en.pdf; s. 1418
-Wetter anzeigen
-puls messen (mit zusatzsensor)
-display soll langsam aus gehen und nicht abrubt dunkel
-Benachrichtigung vom handy
-deepdive uhr: https://www.reddit.com/r/3Dprinting/comments/mkz25o/my_friend_and_i_3d_printed_the_housing_for_a_diy/
-rgb strip steuern via matter: https://docs.espressif.com/projects/arduino-esp32/en/latest/matter/matter.html
-Idee für wrist induced backlight activation:
      -bei gyrometer innerhalb bereich x, licht an. Jedoch nur, wenn acc so ist
          -viele tests machen
      -wenn gyroskop innerhalb xxx ist, an das lciht(immer in position mit display nach obne)

      -wenn alle beträge der acc data addiert weniger als 0.4 sind, display an

EFFICIENCY
Youtube video: https://www.youtube.com/watch?v=K42VbYoDgjU
-1 HZ wenn kein touch auf displayd
-LVGL WEBSITE-> SLEEP MANAGEMENT
setCpuFrequencyMhz(20);

Wrist movement detection:
IF (Gyroscope_Rotation_Rate < Schwellenwert_Ruhe) UND (Accelerometer_Varianz < Schwellenwert_Ruhe):
    // Arm ist ruhig
    SET Zustand = "STILL"
    RESET Timer

  ELSE IF (Zustand == "STILL") UND (Gyroscope_Rotation_Rate > Schwellenwert_Bewegung):
    // Schnelle Bewegung erkannt
    SET Zustand = "MOVING_UP"
    START Timer_Movement
    SPEICHERE Start_Orientierung

  ELSE IF (Zustand == "MOVING_UP"):
    // Prüfen, ob die Bewegung gestoppt hat und der Arm stabil ist
    IF (Gyroscope_Rotation_Rate < Schwellenwert_Stabilisierung) UND (Timer_Movement > Min_Dauer_Bewegung):
        // Prüfen, ob der Arm nun horizontal ist (Display zeigt zum Nutzer)
        IF (Neigungswinkel_Accelerometer liegt im Bereich 0° bis 60°):
            AKTIVIERE_DISPLAY()
            SET Zustand = "ACTIVE"
        ELSE:
            // Bewegung war falsch (z.B. Arm wurde nur seitlich geschwungen)
            SET Zustand = "STILL"
    ELSE IF (Timer_Movement > Max_Dauer_Bewegung):
        // Bewegung dauerte zu lange (z.B. langsames Heben) -> Ignorieren
        SET Zustand = "STILL"

*/
void setup() {
  USBSerial.begin(115200); /* prepare for possible serial debug */
  pinMode(voltageDividerPin, INPUT);
  if (!rtc.begin(Wire)) {
    USBSerial.println("Failed to find PCF8563 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }
  while (CST816T->begin() == false) {
    USBSerial.println("CST816T initialization fail");
    delay(2000);
  }
  //while (!Serial);

#ifdef USE_WIRE
  //Using WIRE !!
  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS)) {
    USBSerial.println("Failed to find QMI8658 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }
#else
  if (!qmi.begin(IMU_CS)) {
    USBSerial.println("Failed to find QMI8658 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }
#endif

  qmi_setup();  //start qmi sensor

  CST816T->IIC_Write_Device_State(CST816T->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                  CST816T->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);
  //###############################
  USBSerial.println("device state successfull");

  //energie keys:
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, HIGH);  // Initialize output pin to HIGH --->>> pin 41
    //pinMode(BL_PIN, OUTPUT);  // sets the pin as output->>>>pin for pwm backlight
    pinMode(BUZZER, OUTPUT);
  gfx->begin();
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  screenWidth = gfx->width();
  screenHeight = gfx->height();
  lv_init();
  gfx->setTextColor(WHITE);
  gfx->fillScreen(WHITE);

  USBSerial.println("lv init successfull");
  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 4 * sizeof(lv_color_t), MALLOC_CAP_DMA);
  lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 4 * sizeof(lv_color_t), MALLOC_CAP_DMA);

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * screenHeight / 4);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  USBSerial.println("display driver init successfull");
  //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


  if (true) {
    const esp_timer_create_args_t lvgl_tick_timer_args = {
      .callback = &example_increase_lvgl_tick,
      .name = "lvgl_tick"
    };

    const esp_timer_create_args_t reboot_timer_args = {
      .callback = &example_increase_reboot,
      .name = "reboot"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);
  }


  load_ui();

  //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  USBSerial.println("setup finished");
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
    check_lcd_bl_timeout_timer();                 //ueberprueft u.a. beruehrung
  check_if_power_button_is_pressed_once();
  refresh_qmi_readings_if_active_mpu();

}
/*
 if(lv_disp_get_inactive_time(NULL) > 7000) {
      USBSerial.println("jetzt");
sys_delay_ms(1000);
    }*/

void every_ten_seconds(){
  every_ten_sec_counter++;
    if (every_ten_sec_counter >= 10 || every_ten_sec_counter < 0) {  //direkt zum start <0 und alle 10 sec danach
    if(lv_anim_count_running()>0){
      last_touch_time = millis();
    }
      if (qmi.getDataReady()) {
        float qmi_data_current = qmi.getTemperature_C();
        float voltage = 0;
        float voltage_sum = 0;
        for(int z = 0; z<3; z++){
          voltage = analogReadMilliVolts(voltageDividerPin);
          voltage_sum = voltage_sum + voltage;
          delay(5);
        }

        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f V", voltage_sum / 1000.0f);
        lv_label_set_text(label_voltage, buf);
        if (voltage_sum <= 3500) {
          esp_deep_sleep_start();
        }
        set_temp(bar, qmi_data_current);
        set_value(bar_volt, voltage_sum);
        snprintf(buf, sizeof(buf), "%.1f °C", qmi_data_current);
        lv_label_set_text(label_temperature, buf);

        every_ten_sec_counter = 0;

        float temp_for_chart = qmi_data_current;
        for (int i = 0; i <= 18; i++) {
          ser2->y_points[i] = ser2->y_points[i + 1];
        }
        ser2->y_points[19] = (temp_for_chart * 10);     //macht 29.5 wenn temp = 30.5
      }
        int min = 1000;
        int max = 0;
        for (int i = 0; i <= 19; i++) {
          if(ser2->y_points[i] < min){
            min = ser2->y_points[i];
          }
          if(ser2->y_points[i] > max){
            max = ser2->y_points[i];
          }
        }
        if((max - min) < 20){
          max = min + 20;
        }
        USBSerial.println(min);
        USBSerial.println(max);
        double fake_chart_accurate_min = ((min / 10) - 0.3);
        double fake_chart_accurate_max = ((max / 10) + 1.3);
          lv_chart_set_range(fake_chart, LV_CHART_AXIS_PRIMARY_Y, fake_chart_accurate_min, fake_chart_accurate_max);
        lv_chart_refresh(fake_chart);
        USBSerial.println("untere grenze fake chart: " + String(fake_chart_accurate_min) + " obere grenze fake chart: " + String(fake_chart_accurate_max));
          lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, ((min - (min%10)) - 0.3), ((max - (max%10)) + 1.3));
        USBSerial.println("untere grenze real chart: " + String((min - 0.3)) + " obere grenze real chart: " + String((max + 0.3)));
        USBSerial.print("eingetragene zu hohe temp: ");
        USBSerial.println(ser2->y_points[19]);
        USBSerial.println("");
        USBSerial.println("");
        lv_chart_refresh(chart);   
        /*Problem:

-temp 47,1°
fake chart 47.1 - 0.3 = 46.8°
---skala nimmt aber nur ints also wird aus 46.8 als untere grenze 46

-real chart kann aber 468 (also 46.8) annehmen und macht es auch
*/    
    }
}

static void set_temp(void *bar, int32_t temp) {
  lv_bar_set_value((lv_obj_t *)bar, temp, LV_ANIM_OFF);
}
static void set_value(void *bar_volt, int value) {
  lv_bar_set_value((lv_obj_t *)bar_volt, (int32_t)(value), LV_ANIM_OFF);
}
