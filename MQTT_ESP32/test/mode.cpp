#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Arduino.h>
#include <string.h>
#include <stdio.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define BTN_SETUP 26
#define BTN_UP 25
#define BTN_DOWN 33
#define BTN_FAN 35
#define BTN_LED 32
#define BTN_PUMP 34
#define RELAY_FAN 17
#define RELAY_LED 14
#define RELAY_PUMP 27

struct Flag
{
  uint8_t flag_sys = 0;
  uint8_t sec = 0;
  uint8_t btn_modesetup = 0;
  uint8_t btn_checkfan = 0;
  uint8_t btn_checkled = 0;
  uint8_t btn_checkpump = 0;
  uint8_t btn_check = 0;
};
Flag flag;

hw_timer_t *timer_cfg = NULL;

uint8_t cnt = 0;
uint8_t stt_old_btnmodesetup = 1, stt_new_btnmodesetup = 1;
uint8_t led_setup = 0, fan_setup = 0, pump_setup = 0;
uint8_t led_mqtt, fan_mqtt, pump_mqtt;
uint8_t setup_mqtt, control_mqtt;
uint8_t temp_setup = 0, humi_setup = 0, soil_setup = 0;
uint8_t tmemp_mqtt, humi_mqtt, soil_mqtt;
uint8_t cnt_sec = 0;
uint8_t second = 0;
uint8_t humi = 0, temp = 0, soil = 0;
uint8_t cnt_fan = 0, cnt_led = 0, cnt_pump = 0;

byte character[] ={
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,

};
void IRAM_ATTR BTN()
{
  flag.flag_sys = 1;
}
void Display_Manual();
void Display_Auto();
void Display_Smartgarden();
void Setup_humi();
void Setup_temp();
void Setup_soil();
void Setup_fan();
void Setup_led();
void Setup_pump();

void setup()
{
  Serial.begin(9600);
  timer_cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(timer_cfg, &BTN, true);
  timerAlarmWrite(timer_cfg, 1000, true);
  timerAlarmEnable(timer_cfg);

  pinMode(BTN_SETUP, INPUT);
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  // pinMode(BTN_FAN, INPUT);
  // pinMode(BTN_LED, INPUT);
  // pinMode(BTN_PUMP, INPUT);
  pinMode(RELAY_FAN,OUTPUT);
  digitalWrite(RELAY_FAN,HIGH);
  pinMode(RELAY_LED,OUTPUT);
  digitalWrite(RELAY_LED,HIGH);
  pinMode(RELAY_PUMP,OUTPUT);
  digitalWrite(RELAY_PUMP,HIGH);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.createChar(0,character);
}
void loop()
{
  if (flag.flag_sys)
  {
    flag.sec = 1;
    flag.btn_modesetup = 1;
    flag.btn_checkfan = 1;
    flag.btn_checkled = 1;
    flag.btn_checkfan = 1;
    flag.btn_checkpump = 1;
    flag.btn_check = 1;
    cnt_sec++;
    flag.flag_sys = 0;
  }

  if (flag.btn_modesetup)
  {
    int a = digitalRead(BTN_SETUP);
    if (!a)
    {
      cnt++;
    }
    flag.btn_modesetup = 0;
  }
  if (flag.sec)
  {
    ++cnt_sec;
    flag.sec = 0;
  }
  if (cnt_sec == 10)
  {
    second++;
    cnt_sec = 0;
  }
  if (cnt == 0)
  {
    Serial.println(" Chicken 0 ");
    Setup_fan();
    Setup_led();
    Setup_pump();
    Display_Manual();
  }
  

  else if (cnt == 1) // Chuyen sang mode Auto, check setup mqtt
  {
    Serial.println(" Chicken 2 ");
    if (setup_mqtt == 1)
    {
      soil_mqtt = soil_setup;
      humi_mqtt = humi_setup;
      tmemp_mqtt = temp_setup;
    }
    if (control_mqtt == 1)
    {
      fan_mqtt = fan_setup;
      led_mqtt = led_setup;
      pump_mqtt = pump_setup;
    }
    Display_Auto();
  }
  else if (cnt == 2) // Setup humi bang nut nhan
  {
    Serial.println(" Chicken 3 ");
    lcd.setCursor(8, 1);
    lcd.print("<-");
    Setup_humi();
    Display_Auto();
  }
  else if (cnt == 3) // Setup Temp bang nut nhan
  {
    Serial.println(" Chicken 4 ");
    lcd.setCursor(8, 1);
    lcd.print("  ");
    lcd.setCursor(9, 2);
    lcd.print("<-");
    Setup_temp();
    Display_Auto();
  }
  else if (cnt == 4) // Setup Soil bang nut nhan
  {
    Serial.println(" Chicken 5 ");
    lcd.setCursor(10, 2);
    lcd.print("  ");
    lcd.setCursor(8, 3);
    lcd.print("<-");
    Setup_soil();
    Display_Auto();
  }

  else if (cnt == 5) // Xac nhan mode auto chuyen giao dien bang cnt_sec 5s 1 lan
  {
    Serial.println(" Chicken 6 ");

    if (second <= 5)
    {
      Display_Auto();
    }
    else if (second > 5 && second <= 10)
    {

      Display_Smartgarden();
    }
    else if (second > 10)
    {
      lcd.clear();
      second = 0;
    }
  }
  else if (cnt == 6) // Quay ve manual
  {
    Serial.println(" Chicken 7 ");
    
    cnt_fan = 0;
    cnt_led = 0;
    cnt_pump = 0;
    fan_setup = 0;
    led_setup = 0;
    pump_setup = 0;
    cnt = 0;
    lcd.clear();
   
  }
}

void Display_Manual()
{

  lcd.setCursor(0, 0);
  lcd.print("MANUAL");
  lcd.setCursor(13, 0);
  lcd.print("CONTROL");
  lcd.setCursor(0, 1);
  lcd.print("Humi:");
  lcd.print((humi < 10 && humi >= 0) ? "0" : "");
  lcd.print(humi);
  lcd.print("%");
  lcd.setCursor(14, 1);
  lcd.print("Fan :");
  lcd.print(fan_setup);
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print((temp < 10 && temp >= 0) ? "0" : "");
  lcd.print(temp);
  lcd.write(byte(0));
  lcd.print("C");
  lcd.setCursor(14, 2);
  lcd.print("Led :");
  lcd.print(led_setup);
  lcd.setCursor(14, 3);
  lcd.print("Pump:");
  lcd.print(pump_setup);
  lcd.setCursor(0, 3);
  lcd.print("Soil:");
  lcd.print((soil < 10 && soil >= 0) ? "0" : "");
  lcd.print(soil);
  lcd.print("%");
}
void Display_Auto()
{

  lcd.setCursor(0, 0);
  lcd.print("AUTOMODE");
  lcd.setCursor(13, 0);
  lcd.print("CONTROL");
  lcd.setCursor(0, 1);
  lcd.print("Humi:");
  lcd.print((humi_setup < 10 && humi_setup >= 0) ? "0" : "");
  lcd.print(humi_setup);
  lcd.print("%");
  lcd.setCursor(14, 1);
  lcd.print("Fan :");
  lcd.print(fan_setup);
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print((temp_setup < 10 && temp_setup >= 0) ? "0" : "");
  lcd.print(temp_setup);
  lcd.write(byte(0));
  lcd.print("C");
  lcd.setCursor(14, 2);
  lcd.print("Led :");
  lcd.print(led_setup);
  lcd.setCursor(14, 3);
  lcd.print("Pump:");
  lcd.print(pump_setup);
  lcd.setCursor(0, 3);
  lcd.print("Soil:");
  lcd.print((soil_setup < 10 && soil_setup >= 0) ? "0" : "");
  lcd.print(soil_setup);
  lcd.print("%");
}
void Display_Smartgarden()
{

  lcd.setCursor(0, 0);
  lcd.print("SMARTGARDEN");
  lcd.setCursor(13, 0);
  lcd.print("CONTROL");
  lcd.setCursor(0, 1);
  lcd.print("Humi:");
  lcd.print((humi < 10 && humi >= 0) ? "0" : "");
  lcd.print(humi);
  lcd.print("%");
  lcd.setCursor(14, 1);
  lcd.print("Fan :");
  lcd.print(fan_setup);
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print((temp < 10 && temp >= 0) ? "0" : "");
  lcd.print(temp);
  lcd.write(byte(0));
  lcd.print("C");
  lcd.setCursor(14, 2);
  lcd.print("Led :");
  lcd.print(led_setup);
  lcd.setCursor(14, 3);
  lcd.print("Pump:");
  lcd.print(pump_setup);
  lcd.setCursor(0, 3);
  lcd.print("Soil:");
  lcd.print((soil < 10 && soil >= 0) ? "0" : "");
  lcd.print(soil);
  lcd.print("%");
}
void Setup_humi()
{
  if (flag.btn_check)
  {
    if (!digitalRead(BTN_UP))
    {
      if (humi_setup < 100)
      {
        humi_setup++;
        Serial.println(" Chicken Humi check ");
      }
    }
    if (!digitalRead(BTN_DOWN))
    {
      if (humi_setup > 0)
      {
        humi_setup--;
      }
    }
  }
}
void Setup_temp()
{
  if (flag.btn_check)
  {
    if (!digitalRead(BTN_UP))
    {
      if (temp_setup < 100)
      {
        temp_setup++;
      }
    }
    if (!digitalRead(BTN_DOWN))
    {
      if (temp_setup > 0)
      {
        temp_setup--;
      }
    }
  }
}
void Setup_soil()
{
  if (flag.btn_check)
  {
    if (!digitalRead(BTN_UP))
    {
      if (soil_setup < 100)
      {
        soil_setup++;
      }
    }
    if (!digitalRead(BTN_DOWN))
    {
      if (soil_setup > 0)
      {
        soil_setup--;
      }
    }
  }
}
void Setup_fan()
{
  if (flag.btn_checkfan)
  {
    if (!digitalRead(BTN_FAN))
    {
      cnt_fan++;
    }
    if (cnt_fan == 1)
    {
      fan_setup = 1;
      digitalWrite(RELAY_FAN,LOW);
    }
    else if (cnt_fan == 2)
    {
      fan_setup = 0;
      cnt_fan = 0;
      digitalWrite(RELAY_FAN,HIGH);
    }
    flag.btn_checkfan = 0;
  }
}
void Setup_led()
{
  if (flag.btn_checkled)
  {
    if (!digitalRead(BTN_LED))
    {
      cnt_led++;
    }
    if (cnt_led == 1)
    {
      led_setup = 1;
      digitalWrite(RELAY_LED,LOW);
    }
    else if (cnt_led == 2)
    {
      led_setup = 0;
      cnt_led = 0;
      digitalWrite(RELAY_LED,HIGH);
    }
    flag.btn_checkled = 0;
  }
}
void Setup_pump()
{
  if (flag.btn_checkpump)
  {
    if (!digitalRead(BTN_PUMP))
    {
      cnt_pump++;
    }
    if (cnt_pump == 1)
    {
      pump_setup = 1;
      digitalWrite(RELAY_PUMP,LOW);
    }
    else if (cnt_pump == 2)
    {
      pump_setup = 0;
      cnt_pump = 0;
      digitalWrite(RELAY_PUMP,HIGH);
    }
    flag.btn_checkpump = 0;
  }
}