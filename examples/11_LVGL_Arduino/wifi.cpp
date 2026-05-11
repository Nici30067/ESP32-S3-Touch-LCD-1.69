#include "wifi.h"
//#include "pin_config.h"
#include <Arduino.h>
#include "WiFi.h"  //wireless lan WLAN capabilities
#include "power.h"
#include "ui.h"
#include "timer.h"


int wifi_state = 0; // 0 -> nichts passiert;;;;; 1-> button geklickt, alles wird in gang gebracht(start), buttton text ist start;;;
                    // 2->connect... und weiter ueberpruefen;;; 3-> connected.
int wifi_timeout = 0;
bool second_wifi_didnt_work_aswell = false;
int wifi_server_timeout = 0;



void chech_wifi_and_fetch(){
  if(wifi_state == 2){      //status connection begin but not successfull rn but below timeout
    if (WiFi.status() == WL_CONNECTED) {
      lv_label_set_text(get_wifi_time_button_label, "connected");
      wifi_state = 3;
      USBSerial.println(WiFi.localIP());
      USBSerial.println("wifi connection successfull");
    }else if(wifi_timeout<15){
      wifi_timeout++;

    }else{
      lv_label_set_text(get_wifi_time_button_label, "no success wifi 1");
      if(second_wifi_didnt_work_aswell == false){
        wifi_state = 10;
      }else{
        wifi_state = 5;
      }
      
      
    }
  }else if(wifi_state == 3){        //wifi besteht, start time fetch
    configTime(1 * 7200, 0,"pool.ntp.org", "1.pool.ntp.org");  // Time zone
    wifi_state = 4;
    USBSerial.println("time server started");
  }else if(wifi_state == 4){          //time fetch erfolgreich?
    time(&t);  // Get current time information, followUp
    USBSerial.print("t=");
    USBSerial.println(t);
    wifi_server_timeout++;
    if(wifi_server_timeout > 60){       
      wifi_state = 5;
      lv_label_set_text(get_wifi_time_button_label, "Prob server");       //server antwortet nicht
      wifi_state = 0;

    }
    if(t > 1577836800){                 //time fetch ist sinnvoll
      struct tm *timeinfo = localtime(&t);
          USBSerial.println(timeinfo->tm_hour);
    USBSerial.println(timeinfo->tm_min);
    USBSerial.println("time fetch successfull");
    lv_label_set_text(get_wifi_time_button_label, "time");
    
    year = timeinfo->tm_year + 1900;
    month = timeinfo->tm_mon + 1;
    day = timeinfo->tm_mday;
    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;
    second = timeinfo->tm_sec;
    USBSerial.println("current time store successfull");
    rtc.setDateTime(year, month, day, hour, minute, second);
    USBSerial.println("rtc set date successfull");
    wifi_state = 5;
    }
  }else if(wifi_state == 5){          //else if, damit es erst bei der nächsten sekunde ausgeführt wird
      WiFi.disconnect(true, true);
      USBSerial.println("wifi disconnected");                     //wlan energiesparmodus
      wifi_state++;
  // 
  }
  if(wifi_state == 6){
    wifi_state++;
  }
  if(wifi_state == 7){
    WiFi.mode(WIFI_OFF);
    //btStop();
    USBSerial.println("Wifi disabled for better battery duration");
    wifi_state = 0;
  }
  if(wifi_state == 10){
    WiFi.disconnect();  
    USBSerial.println("disconnect for startup second wifi");
    wifi_state++;
  }else if(wifi_state == 11){
    wifi_state++;
    USBSerial.println(wifi_state);
  }else if(wifi_state == 12){
    WiFi.mode(WIFI_MODE_NULL);    // Vollständiges Reset
    wifi_state++;
  }else if(wifi_state == 13){
    wifi_state++;
    USBSerial.println(wifi_state);
  }else if(wifi_state == 14){
    WiFi.mode(WIFI_STA);
    WiFi.begin("FRITZ!Box 3000 2.4Ghz", "68459992685037713434");
    USBSerial.println("Connecting to second wifi");
    wifi_timeout = 0;
    wifi_state = 2;
    second_wifi_didnt_work_aswell = true;
  }

}

void event_handler_get_wifi_time_button(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

  wifi_state = 1;           //wifi begin und so alles was start ist
  WiFi.mode(WIFI_STA);
  WiFi.begin("D", "11111112");
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  USBSerial.print("Connecting to WiFi ..");
  lv_label_set_text(get_wifi_time_button_label, "start");
  wifi_state = 2;
}
