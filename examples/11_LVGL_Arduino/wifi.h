#ifndef WIFI_H
#define WIFI_H

extern int wifi_state;
extern int wifi_timeout;
extern bool second_wifi_didnt_work_aswell;
extern int wifi_server_timeout;




void chech_wifi_and_fetch();

#endif