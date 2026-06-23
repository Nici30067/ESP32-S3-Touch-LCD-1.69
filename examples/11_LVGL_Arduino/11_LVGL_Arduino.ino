//test
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
