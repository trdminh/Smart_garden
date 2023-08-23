#include <my_header.h>
#include <ArduinoJson.h>

String ESP32_Data;
hw_timer_t *timer_cfg = NULL;
DynamicJsonDocument data(1024);
char buffer[1000];
int length_buffer;
void IRAM_ATTR BTN()
{

    fl.flag_sys = !fl.flag_sys;
}

void setup() {
  
  timer_cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(timer_cfg, &BTN, true);
  timerAlarmWrite(timer_cfg, 1000, true);
  timerAlarmEnable(timer_cfg);
  
  
                  
}

void loop() {
    
}

