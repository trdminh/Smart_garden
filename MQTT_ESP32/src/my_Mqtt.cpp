
#include <my_header.h>

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_SENSOR, DHT11);
LiquidCrystal_I2C lcd(0x27, 20, 4);

byte character[] = {
    B00111,
    B00101,
    B00111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,

};

struct Flag fl;
uint8_t btn_fan;
uint8_t btn_led;
uint8_t btn_pumb;
uint8_t stt_new_fan = 1;
uint8_t stt_new_led = 1;
uint8_t stt_new_pumbs = 1;
uint8_t stt_old_fan = 1;
uint8_t stt_old_led = 1;
uint8_t stt_old_pumbs = 1;
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
uint8_t stt_old_statusfan = 1 , stt_new_statusfan = 1 ;
uint8_t stt_old_statusled = 1 , stt_new_statusled = 1 ;
uint8_t stt_old_statuspump = 1 , stt_new_statuspump = 1 ;

String dataMQTT, js_control, js_setup, js_auto, js_manual, js_led, js_fan, js_pump, js_tempMax, js_tempMin, js_soilMax, js_soilMin;
int val_Auto, val_Manual;
int val_Control, val_Setup, val_Led, val_Fan, val_Pump;
// int val_Temp_s_Max, val_Temp_s_Min, val_Soil_s_Min, val_Soil_s_Max;
int val_Temp_Serson, val_Humi_Serson, val_Soil_Serson;
int cnt_Send = 0;
int Temp_s_Max = 32, Temp_s_Min = 22, Soil_s_Min = 60, Soil_s_Max = 75;
uint8_t soil1 = 0;

void Sensor_Init()
{
  Wire.begin();
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, character);

  pinMode(SOIL_TEST, INPUT);

  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_LED, OUTPUT);
  pinMode(RELAY_PUMP, OUTPUT);

  pinMode(BTN_LED, INPUT);
  pinMode(BTN_FAN, INPUT);
  pinMode(BTN_PUMP, INPUT);

  pinMode(BTN_SETUP, INPUT);
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);

  digitalWrite(RELAY_FAN, LOW);
  digitalWrite(RELAY_LED, LOW);
  digitalWrite(RELAY_PUMP, LOW);
}

void Stay_Connect()
{
  client.loop();
}

void Wifi_Connect()
{
  WiFi.begin(SSID1, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void MQTT_Connect()
{
  // Serial.println("TOPIC: " + String(TOPIC_SUB));
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
    }
  }
}

void MQTT_Pub_Sub()
{
  client.subscribe(TOPIC_SUB);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // Serial.println("TOPIC: " + String(topic));
  // for (int i = 0; i< length; i++){
  //   dataMQTT += (char) payload[i];
  // }
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload, length);
  if (doc.containsKey("control"))
  {
    js_control = doc["control"].as<String>();
    js_led = doc["led"].as<String>();
    js_fan = doc["fan"].as<String>();
    js_pump = doc["pump"].as<String>();

    val_Control = js_control.toInt();
    if (val_Control)
    {
      val_Led = js_led.toInt();
      val_Fan = js_fan.toInt();
      val_Pump = js_pump.toInt();
    }
  }
  if (doc.containsKey("setup"))
  {
    js_setup = doc["setup"].as<String>();
    js_tempMax = doc["tempmax"].as<String>();
    js_tempMin = doc["tempmin"].as<String>();
    js_soilMax = doc["soilmax"].as<String>();
    js_soilMin = doc["soilmin"].as<String>();

    val_Setup = js_setup.toInt();
    if (val_Setup)
    {
      Temp_s_Max = js_tempMax.toInt();
      Temp_s_Min = js_tempMin.toInt();
      Soil_s_Max = js_soilMax.toInt();
      Soil_s_Min = js_soilMin.toInt();
    }
  }
}

// void Detext(){
  
//   // dataMQTT.remove(0, strlen(dataMQTT));
//   Serial.println(dataMQTT);
// }

void Send_Active_Status(int led, int fan, int pump)
{
  DynamicJsonDocument doc(256);
  doc["led"] = led;
  doc["fan"] = fan;
  doc["pump"] = pump;

  Serial.println("Gia tri gui len cua trang thai: ");
  Serial.print("LED: " + String(led));
  Serial.print(" ");
  Serial.print("FAN: " + String(fan));
  Serial.print(" ");
  Serial.println("PUMP: " + String(pump));

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(TOPIC_SUB, buffer);
}

void Send_Serson_Value(int temp, int humi, int soil)
{
  DynamicJsonDocument doc(256);
  doc["temp"] = temp;
  doc["humi"] = humi;
  doc["soil"] = soil;

  Serial.println("Gia tri gui len cua cam bien: ");
  Serial.print("TEMP: " + String(temp));
  Serial.print(" ");
  Serial.print("HUMI: " + String(humi));
  Serial.print(" ");
  Serial.println("SOIL: " + String(soil));

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(TOPIC_SUB, buffer);
}

void Flag_Status()
{
  if (fl.flag_sys)
  {
    fl.sec = 1;
    fl.btn_modesetup = 1;
    fl.btn_checkfan = 1;
    fl.btn_checkled = 1;
    fl.btn_checkfan = 1;
    fl.btn_checkpump = 1;
    fl.btn_controlhumi = 1;
    fl.btn_controltemp = 1;
    fl.btn_controlsoil = 1;
    cnt_sec++;
    fl.flag_SendData++;
    fl.flag_button_fan = 1;
    fl.flag_button_led = 1;
    fl.flag_button_pumb = 1;
    fl.flag_down = 1;
    fl.flag_up = 1;
    fl.flag_menu_set_up = 1;
    fl.flag_humi = 1;
    fl.flag_soil = 1;
    fl.flag_temp = 1;
    fl.flag_sys = 0;
    fl.check_sensor = 1;
  }
}

void Led_Manual()
{
  if (fl.flag_button_led)
  {
    stt_old_led = stt_new_led;
    stt_new_led = btn_led;
    if (stt_old_led == 1 && stt_new_led == 0)
    {
      digitalWrite(RELAY_LED, val_Led);
    }
    else
    {
      digitalWrite(RELAY_LED, !val_Led);
    }
    fl.flag_button_led = 0;
  }
}

void Pumbs_Manual()
{
  if (fl.flag_button_pumb)
  {
    stt_old_pumbs = stt_new_pumbs;
    stt_new_pumbs = btn_pumb;
    if (stt_old_pumbs == 1 && stt_new_pumbs == 0)
    {
      digitalWrite(RELAY_PUMP, val_Pump);
    }
    else
    {
      digitalWrite(RELAY_PUMP, !val_Pump);
    }
    fl.flag_button_pumb = 0;
  }
}

void Fan_Manual()
{
  if (fl.flag_button_fan)
  {
    stt_old_fan = stt_new_fan;
    stt_new_fan = btn_fan;
    if (stt_old_fan == 1 && stt_new_fan == 0)
    {
      digitalWrite(RELAY_FAN, val_Fan);
    }
    else
    {
      digitalWrite(RELAY_FAN, !val_Fan);
    }
    fl.flag_button_fan = 0;
  }
}

void Soil_Sensor()
{

  for (int i = 0; i < 50; i++)
  {
    soil1 += analogRead(SOIL_TEST);
  }
  soil1 = analogRead(SOIL_TEST);
  val_Soil_Serson = map(soil1, 0, 4095, 0, 100);
}

void Dht_Sensor()
{
  // val_Temp_Serson = dht.readTemperature();

  // val_Humi_Serson = dht.readHumidity();
  val_Temp_Serson = random(27,30);
  
  val_Humi_Serson = random(60,63);
  
}
void Led_Auto()
{
  if (val_Temp_Serson < Temp_s_Min)
  { // tmep cua cb
    digitalWrite(RELAY_LED, HIGH);
  }
  else
  {
    digitalWrite(RELAY_LED, LOW);
  }
}

void Pumb_Auto()
{
  if (val_Temp_Serson > Temp_s_Max || val_Soil_Serson < Soil_s_Min)
  {
    digitalWrite(RELAY_PUMP, HIGH);
  }
  else if (val_Soil_Serson > Soil_s_Max || val_Temp_Serson <= Temp_s_Min)
  {
    digitalWrite(RELAY_PUMP, LOW);
  }
}

void Fan_Auto()
{
  if (val_Temp_Serson >= Temp_s_Max)
  {
    digitalWrite(RELAY_FAN, HIGH);
  }
  else
  {
    digitalWrite(RELAY_FAN, LOW);
  }
}

void Button_Led()
{
  btn_led = digitalRead(BTN_LED);
}

void Button_Fan()
{
  btn_fan = digitalRead(BTN_FAN);
}

void Button_Pump()
{
  btn_pumb = digitalRead(BTN_PUMP);
}

void Display_Manual()
{

  lcd.setCursor(0, 0);
  lcd.print("MANUAL");
  lcd.setCursor(13, 0);
  lcd.print("CONTROL");
  lcd.setCursor(0, 1);
  lcd.print("Humi:");
  lcd.print((val_Humi_Serson < 10 && val_Humi_Serson >= 0) ? "0" : "");
  lcd.print(val_Humi_Serson);
  lcd.print("%");
  lcd.setCursor(14, 1);
  lcd.print("Fan :");
  lcd.print(fan_setup);
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print((val_Temp_Serson < 10 && val_Temp_Serson >= 0) ? "0" : "");
  lcd.print(val_Temp_Serson);
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
  lcd.print((val_Soil_Serson < 10 && val_Soil_Serson >= 0) ? "0" : "");
  lcd.print(val_Soil_Serson);
  lcd.print("%");
}

void Display_Auto1()
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
  lcd.print(digitalRead(RELAY_FAN));
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print((temp_setup < 10 && temp_setup >= 0) ? "0" : "");
  lcd.print(temp_setup);
  lcd.write(byte(0));
  lcd.print("C");
  lcd.setCursor(14, 2);
  lcd.print("Led :");
  lcd.print(digitalRead(RELAY_LED));
  lcd.setCursor(14, 3);
  lcd.print("Pump:");
  lcd.print(digitalRead(RELAY_PUMP));
  lcd.setCursor(0, 3);
  lcd.print("Soil:");
  lcd.print((soil_setup < 10 && soil_setup >= 0) ? "0" : "");
  lcd.print(soil_setup);
  lcd.print("%");
}
void Display_Auto2()
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
  lcd.print(digitalRead(RELAY_FAN));
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print(( Temp_s_Max < 10 && Temp_s_Max >= 0) ? "0" : "");
  lcd.print(Temp_s_Max);
  lcd.write(byte(0));
  lcd.print("C");
  lcd.setCursor(14, 2);
  lcd.print("Led :");
  lcd.print(digitalRead(RELAY_LED));
  lcd.setCursor(14, 3);
  lcd.print("Pump:");
  lcd.print(digitalRead(RELAY_PUMP));
  lcd.setCursor(0, 3);
  lcd.print("Soil:");
  lcd.print((Soil_s_Min < 10 && Soil_s_Min >= 0) ? "0" : "");
  lcd.print(Soil_s_Min);
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
  lcd.print((val_Humi_Serson < 10 && val_Humi_Serson >= 0) ? "0" : "");
  lcd.print(val_Humi_Serson);
  lcd.print("%");
  lcd.setCursor(14, 1);
  lcd.print("Fan :");
  lcd.print(digitalRead(RELAY_FAN));
  lcd.setCursor(0, 2);
  lcd.print("Temp:");
  lcd.print((val_Temp_Serson < 10 && val_Temp_Serson >= 0) ? "0" : "");
  lcd.print(val_Temp_Serson);
  lcd.write(byte(0));
  lcd.print("C");
  lcd.setCursor(14, 2);
  lcd.print("Led :");
  lcd.print(digitalRead(RELAY_LED));
  lcd.setCursor(14, 3);
  lcd.print("Pump:");
  lcd.print(digitalRead(RELAY_PUMP));
  lcd.setCursor(0, 3);
  lcd.print("Soil:");
  lcd.print((val_Soil_Serson < 10 && val_Soil_Serson >= 0) ? "0" : "");
  lcd.print(val_Soil_Serson);
  lcd.print("%");
}

void Setup_humi()
{
  if (fl.btn_controlhumi)
  {
    if (!digitalRead(BTN_UP))
    {
      
      if (humi_setup < 99)
      {
        humi_setup++;
        Serial.println(humi_setup);
      }
    }
    Serial.println(digitalRead(BTN_UP));
    if (!digitalRead(BTN_DOWN))
    {
      if (humi_setup > 0)
      {
        humi_setup--;
      }
    }
    fl.btn_controlhumi = 0;
  }
}

void Setup_temp()
{
  if (fl.btn_controltemp)
  {
    if (!digitalRead(BTN_UP))
    {
      if (temp_setup < 99)
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
    fl.btn_controltemp = 0;
  }
}

void Setup_soil()
{
  if (fl.btn_controlsoil)
  {
    if (!digitalRead(BTN_UP))
    {
      if (soil_setup < 99)
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
    fl.btn_controlsoil = 0;
  }
}

void Setup_fan()
{
  if (fl.btn_checkfan)
  {
    
    stt_old_statusfan = stt_new_statusfan;
    stt_new_statusfan = digitalRead(BTN_FAN);
    if (stt_old_statusfan == 1 && stt_new_statusfan == 0)
    {
      cnt_fan++;
    }
    if (cnt_fan == 1)
    {
      fan_setup = 1;
      digitalWrite(RELAY_FAN, HIGH);
    }
    else if (cnt_fan == 2)
    {
      fan_setup = 0;
      cnt_fan = 0;
      digitalWrite(RELAY_FAN, LOW);
    }
    fl.btn_checkfan = 0;
  }
}

void Setup_led()
{
  if (fl.btn_checkled)
  {
    
    stt_old_statusled = stt_new_statusled;
    stt_new_statusled = digitalRead(BTN_LED);
    if (stt_old_statusled == 1 && stt_new_statusled ==0)
    {
      cnt_led++;
    }
    if (cnt_led == 1)
    {
      led_setup = 1;
      digitalWrite(RELAY_LED, HIGH);
    }
    else if (cnt_led == 2)
    {
      led_setup = 0;
      cnt_led = 0;
      digitalWrite(RELAY_LED, LOW);
    }
    fl.btn_checkled = 0;
  }
}

void Setup_pump()
{
  if (fl.btn_checkpump)
  {
    
    stt_old_statuspump = stt_new_statuspump;
    stt_new_statuspump = digitalRead(BTN_PUMP);
    if (stt_old_statuspump == 1 && stt_new_statuspump ==0)
    {
      cnt_pump++;
    }
    if (cnt_pump == 1)
    {
      pump_setup = 1;
      digitalWrite(RELAY_PUMP, HIGH);
    }
    else if (cnt_pump == 2)
    {
      pump_setup = 0;
      cnt_pump = 0;
      digitalWrite(RELAY_PUMP, LOW);
    }
    fl.btn_checkpump = 0;
  }
}

void Character_cnt2()
{
  lcd.setCursor(8, 1);
  lcd.print("<-");
}
void Character_cnt3()
{
  lcd.setCursor(8, 1);
  lcd.print("  ");
  lcd.setCursor(9, 2);
  lcd.print("<-");
}
void Character_cnt4()
{
  lcd.setCursor(9, 2);
  lcd.print("  ");
  lcd.setCursor(8, 3);
  lcd.print("<-");
}
void Character_cnt5()
{
  lcd.clear();
}
void Character_cnt6()
{
  lcd.clear();
}


