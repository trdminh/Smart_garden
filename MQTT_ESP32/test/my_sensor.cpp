
#include <my_header.h>

DHT dht(DHT_SENSOR, DHT11);
long soil;
uint8_t btn_fan;
uint8_t btn_led;
uint8_t btn_pumb;
void Dht_Init(){
    dht.begin();
}

void Dht_Sensor(){
    humi = dht.readHumidity();
    temp = dht.readTemperature();
}



void Soil(){
    long soil1;
    for(int i = 0;i < 50;i++){
        soil1 = analogRead(SOIL_TEST);
    }
    soil1 = soil1/50;
    soil = map(soil1,0,4095,0,100);
}

void Button_Led(){
    btn_led = digitalRead(BUTTON_LED);
}

void Button_Fan(){
    btn_fan = digitalRead(BUTTON_FAN);
}

void Button_Pumb(){
    btn_pumb = digitalRead(BUTTON_PUMB);
}