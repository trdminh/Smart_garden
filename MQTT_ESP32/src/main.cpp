
#include <my_header.h>

hw_timer_t *Timer0_Cfg = NULL;
uint32_t Freq = 0;
uint8_t flag_stt = 0;

// LiquidCrystal_I2C lcd(0x27, 20, 4);
// list_StatusFlag_t

void IRAM_ATTR Break()
{
  // doc gtri cam bien o day:...
  // Dht_Sensor();
  Setup_fan();
  fl.flag_sys = !fl.flag_sys;
}

void setup()
{
  Serial.begin(9600);
  Sensor_Init();
  // thiet lap ngat tgian:
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &Break, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);
  Wifi_Connect();
  MQTT_Connect();
}

void loop()
{
  Stay_Connect();
  MQTT_Pub_Sub();
  Flag_Status();

  // if(fl.flag_SendData == 50){
  //   Send_Active_Status(val_Led, val_Fan, val_Pump);
  //   Send_Serson_Value(val_Temp_Serson, val_Humi_Serson, val_Soil_Serson);
  //   fl.flag_SendData = 0;
  // }
  
  if (fl.btn_modesetup)
  {
    // Dht_Sensor();
    int a = digitalRead(BTN_SETUP);
    if (!a)
    {
      cnt++;
    }
    fl.btn_modesetup = 0;
  }
  if (fl.sec)
  {
    ++cnt_sec;
    fl.sec = 0;
  }
  if (cnt_sec == 10)
  {
    second++;
    cnt_sec = 0;
  }
  if (second % 2 == 0)
  {

    Dht_Sensor();
    delay(100);
    Soil_Sensor();
  }

  if (cnt == 0)
  {
    Serial.println(" Chicken 0 ");
    
    Setup_led();
    Setup_pump();
    Display_Manual();
  }
  else if (cnt == 1) // Chuyen sang mode Auto, check setup mqtt
  {
    digitalWrite(RELAY_FAN,LOW);
    digitalWrite(RELAY_LED,LOW);
    digitalWrite(RELAY_PUMP,LOW);
    Serial.println(" Chicken 2 ");
    if(val_Control){
      digitalWrite(RELAY_LED, val_Led);
      digitalWrite(RELAY_FAN, val_Fan);
      digitalWrite(RELAY_PUMP, val_Pump);
      Serial.println(val_Led);
      Serial.println(val_Fan);
      Serial.println(val_Pump);
    }
    if(!val_Control){
      Led_Auto();
      Fan_Auto();
      Pumb_Auto();
    }
      
      Display_Auto2();
  }
  else if (cnt == 2) // Setup humi bang nut nhan
  {
    digitalWrite(RELAY_FAN,LOW);
    digitalWrite(RELAY_LED,LOW);
    digitalWrite(RELAY_PUMP,LOW);
    Serial.println(" Chicken 3 ");  
    Character_cnt2();
    // lcd.setCursor(8, 1);
    // lcd.print("<-");
    Setup_humi();
    Display_Auto1();
  }
  else if (cnt == 3) // Setup Temp bang nut nhan
  {
    Serial.println(" Chicken 4 ");
    Character_cnt3();
    // lcd.setCursor(8, 1);
    // lcd.print("  ");
    // lcd.setCursor(9, 2);
    // lcd.print("<-");
    Setup_temp();
    Display_Auto1();
  }
  else if (cnt == 4) // Setup Soil bang nut nhan
  {
    Serial.println(" Chicken 5 ");
    Character_cnt4();
    // lcd.setCursor(10, 2);
    // lcd.print("  ");
    // lcd.setCursor(8, 3);
    // lcd.print("<-");
    Setup_soil();
    Display_Auto1();
  }
  else if (cnt == 5) // Xac nhan mode auto chuyen giao dien bang cnt_sec 5s 1 lan
  {
    Temp_s_Max = temp_setup ;
    Soil_s_Min = soil_setup;
    Serial.println(" Chicken 6 ");
    Fan_Auto();
    Pumb_Auto();

    if (second <= 5)
    {
      Display_Auto1();
    }
    else if (second > 5 && second <= 10)
    {

      Display_Smartgarden();
    }
    else if (second > 10)
    {
      Character_cnt5();
      // lcd.clear();
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
    Temp_s_Max = 32;
    Soil_s_Min = 60;
    digitalWrite(RELAY_FAN,LOW);
    digitalWrite(RELAY_LED,LOW);
    digitalWrite(RELAY_PUMP,LOW);
    Character_cnt6();
    // lcd.clear();
  }
}
