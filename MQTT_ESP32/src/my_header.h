#ifndef __MY_HEADER_H__
#define __MY_HEADER_H__

#include <my_define.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
// #include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <string.h>
#include <stdio.h>

extern String dataMQTT, js_control, js_setup, js_auto, js_manual, js_led, js_fan, js_pump, js_tempMax, js_tempMin, js_soilMax, js_soilMin;
extern int val_Auto, val_Manual;
extern int val_Control, val_Setup, val_Led, val_Fan, val_Pump;
// extern int val_Temp_s_Max, val_Temp_s_Min, val_Soil_s_Min, val_Soil_s_Max;
extern int val_Temp_Serson, val_Humi_Serson, val_Soil_Serson;
extern int cnt_Send, check_Control;
extern int Temp_s_Max, Temp_s_Min, Soil_s_Min, Soil_s_Max;

extern uint8_t stt_old_led;
extern uint8_t stt_new_led;
extern uint8_t stt_old_pumbs;
extern uint8_t stt_new_pumbs;
extern uint8_t stt_old_fan;
extern uint8_t stt_new_fan;
extern uint8_t btn_led;
extern uint8_t btn_pumb;
extern uint8_t btn_fan;
extern uint8_t cnt;
extern uint8_t stt_old_btnmodesetup, stt_new_btnmodesetup;
extern uint8_t led_setup, fan_setup, pump_setup;
extern uint8_t temp_setup, humi_setup, soil_setup;
extern uint8_t tmemp_mqtt, humi_mqtt, soil_mqtt;
extern uint8_t cnt_sec;
extern uint8_t second;
extern uint8_t humi, temp, soil;
extern uint8_t cnt_fan, cnt_led, cnt_pump;
extern uint8_t soil1;
extern uint8_t stt_old_statusfan  , stt_new_statusfan;
extern uint8_t stt_old_statusled  , stt_new_statusled;
extern uint8_t stt_old_statuspump , stt_new_statuspump;

typedef struct Flag{
    uint8_t flag_sys;
    uint8_t sec;
    uint8_t btn_modesetup;
    uint8_t btn_checkfan;
    uint8_t btn_checkled;
    uint8_t btn_checkpump;
    uint8_t btn_controlhumi ;
    uint8_t btn_controltemp ;
    uint8_t btn_controlsoil ;
    uint8_t flag_button_led;
    uint8_t flag_button_pumb;
    uint8_t flag_button_fan;
    uint8_t flag_soil;
    uint8_t flag_humi;
    uint8_t flag_temp;
    uint8_t flag_up;
    uint8_t flag_down;
    uint8_t flag_menu_set_up; 
    uint8_t flag_SendData;
    uint8_t check_sensor ;
} Flag;

extern struct Flag fl;

void Stay_Connect();
void Wifi_Connect();
void MQTT_Connect(void);
void MQTT_Pub_Sub();
void callback(char *topic, byte *payload, unsigned int length);
void Detext();
void readMQTTData(const char* payload, unsigned int length, String& js_autoMQTT, String& js_manualMQTT, String& js_led, String& js_fan, String& js_pump, String& js_temp, String& js_humi, String& js_soil);
void convertToDataMQTT(String& js_autoMQTT, String& js_manualMQTT, String& js_led, String& js_fan, String& js_pump, String& js_temp, String& js_humi, String& js_soil);
void Send_Active_Status(int led, int fan, int pump);
void Send_Serson_Value(int temp, int humi, int soil);

void Sensor_Init();
void Flag_Status();
void Dht_Sensor();
void Soil_Sensor();
void Led_Manual();
void Pumbs_Manual();
void Fan_Manual();
void Led_Auto();
void Pumb_Auto();
void Fan_Auto();
void Button_Led();
void Button_Fan();
void Button_Pump();

void Display_Manual();
void Display_Auto1();
void Display_Auto2();
void Display_Smartgarden();
void Setup_humi();
void Setup_temp();
void Setup_soil();
void Setup_fan();
void Setup_led();
void Setup_pump();

void Character_cnt2();
void Character_cnt3();
void Character_cnt4();
void Character_cnt5();
void Character_cnt6();


#endif