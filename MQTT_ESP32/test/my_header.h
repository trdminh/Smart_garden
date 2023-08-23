#ifndef __MY_HEADER__
#define __MY_HEADER__

#include <my_define.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>

extern float humi;
extern float temp;
extern long soil;
extern uint8_t stt_old_led;
extern uint8_t stt_new_led;
extern uint8_t stt_old_pumbs;
extern uint8_t stt_new_pumbs;
extern uint8_t stt_old_fan;
extern uint8_t stt_new_fan;
extern uint8_t btn_led;
extern uint8_t btn_pumb;
extern uint8_t btn_fan;
extern String sTemp, sHumi, sSoil;
extern String val_Temp, val_Humi, val_Soil;

typedef struct Flag{
    uint8_t flag_sys;
    uint8_t flag_button_led;
    uint8_t flag_button_pumb;
    uint8_t flag_button_fan;
    uint8_t flag_soil;
    uint8_t flag_humi;
    uint8_t flag_temp;
    uint8_t flag_up;
    uint8_t flag_down;
    uint8_t flag_menu_set_up; 
} Flag;

extern struct Flag fl;

void Dht11_Init();
void Sensor_Init();
void Flag_Status();
void Soil();
void Led_Manual();
void Pumbs_Manual();
void Fan_Manual();
void Led_Auto();
void Pumb_Auto();
void Fan_Auto();


#endif