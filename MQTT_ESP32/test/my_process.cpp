
#include <my_header.h>


struct Flag fl;
uint8_t stt_new_fan   = 1;
uint8_t stt_new_led   = 1;
uint8_t stt_new_pumbs = 1;
uint8_t stt_old_fan   = 1;
uint8_t stt_old_led   = 1;
uint8_t stt_old_pumbs = 1;


void Flag_Status(){
    if(fl.flag_sys){
        fl.flag_button_fan = 1;
        fl.flag_button_led = 1;
        fl.flag_button_pumb = 1;
        fl.flag_down = 1;
        fl.flag_up = 1;
        fl.flag_menu_set_up = 1;
        fl.flag_humi = 1;
        fl.flag_soil = 1;
        fl.flag_temp = 1;
    }
}

void Led_Manual(){
    if(fl.flag_button_led){
        stt_old_led = stt_new_led;
        stt_new_led = btn_led;
        if(stt_old_led == 1 && stt_new_led == 0){
            digitalWrite(RELAY_LED, HIGH);
        }
        else{
            digitalWrite(RELAY_LED, LOW);
        }
        fl.flag_button_led = 0;
    }
}

void Pumbs_Manual(){
    if(fl.flag_button_pumb){
        stt_old_pumbs = stt_new_pumbs;
        stt_new_pumbs = btn_pumb;
        if(stt_old_pumbs == 1 && stt_new_pumbs == 0){
            digitalWrite(RELAY_PUMB, HIGH);
        }
        else{
            digitalWrite(RELAY_PUMB, LOW);
        }
        fl.flag_button_pumb = 0;
    }
    
}

void Fan_Manual(){
    if(fl.flag_button_fan){
        stt_old_fan = stt_new_fan;
        stt_new_fan = btn_fan;
        if(stt_old_fan == 1 && stt_new_fan == 0){
            digitalWrite(RELAY_FAN, HIGH);
        }
        else{
            digitalWrite(RELAY_FAN, LOW);
        }
        fl.flag_button_fan = 0;
    }
}

void Led_Auto(){
    if(temp < CONTROL_TEMP_1){ // tmep cua cb
        digitalWrite(RELAY_LED,HIGH);
    }
    else{
        digitalWrite(RELAY_LED,LOW);
    }
}

void Pumb_Auto(){
    if(temp > CONTROL_TEMP_2 || soil < MIN_SOIL){
        digitalWrite(RELAY_PUMB,HIGH);
    }
    else if(soil > MAX_SOIL || temp < CONTROL_TEMP_2){
        digitalWrite(RELAY_PUMB,LOW);
    }
}

void Fan_Auto(){
    if(temp > CONTROL_TEMP_2){
        digitalWrite(RELAY_FAN, HIGH);
    }
    else{
        digitalWrite(RELAY_FAN, LOW);
    }
}


void Sensor_Init(){
    Serial.begin(9600);
    pinMode(SOIL_TEST,INPUT);
    pinMode(RELAY_FAN, OUTPUT);
    pinMode(RELAY_LED, OUTPUT);
    pinMode(RELAY_PUMB, OUTPUT);
}