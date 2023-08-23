#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <my_header.h>

WiFiClient espClient;
PubSubClient client(espClient);


const int Led = 27;
hw_timer_t *Timer0_Cfg = NULL;
uint32_t Freq = 0;
uint8_t flag_stt = 0;

String js_autoMQTT, js_manualMQTT, js_led, js_fan, js_pump, js_temp, js_humi, js_soil;
int val_AutoMQTT, val_ManualMQTT, val_LedMQTT, val_FanMQTT, val_PumpMQTT, val_TempMQTT, val_HumiMQTT, val_SoilMQTT;
int val_autoMQTT, val_manualMQTT, val_Led, val_Fan, val_Pump, val_Temp, val_Humi, val_Soil;
int cnt_Send = 0;

/*
  uint8_t led = 0 , fan = 0 , pump = 0, temp = 0, humi = 0, soil = 0;
  uint8_t led_mqtt , fan_mqtt , pump_mqtt , temp_mqtt, humi_mqtt, soil_mqtt;
  uint8_t setup_mqtt , control_mqtt ;
*/

void MQTT_Connect(void);
void MQTT_Pub_Sub();
void callback(char *topic, byte *payload, unsigned int length);
void readMQTTData(const char* payload, unsigned int length, String& js_autoMQTT, String& js_manualMQTT, String& js_led, String& js_fan, String& js_pump, String& js_temp, String& js_humi, String& js_soil);
void convertToDataMQTT(String& js_autoMQTT, String& js_manualMQTT, String& js_led, String& js_fan, String& js_pump, String& js_temp, String& js_humi, String& js_soil);
void sendSensorData(int val_Led, int val_Fan, int val_Pump, int val_Temp, int val_Humi, int val_Soil);

void IRAM_ATTR Blink()
{
  // doc gtri cam bien o day:...
  val_Led = 0, val_Fan = 1, val_Pump = 0, val_Temp = 30, val_Humi = 59, val_Soil = 50;

  flag_stt = 1;
}

void setup(){
  Serial.begin(9600);
  pinMode(Led, OUTPUT);

  // thiet lap ngat tgian:
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &Blink, true);
  timerAlarmWrite(Timer0_Cfg, 10000, true);
  timerAlarmEnable(Timer0_Cfg);

  // thiet lap wifi:
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  MQTT_Connect();
}

void loop(){
  if(flag_stt){
    cnt_Send++;
    flag_stt = 0;
  }
  
	client.loop();
  MQTT_Pub_Sub();
  val_Led = 0, val_Fan = 1, val_Pump = 0, val_Temp = 30, val_Humi = 59, val_Soil = 50;
  if(cnt_Send == 5){
    cnt_Send = 0;
    sendSensorData(val_Led, val_Fan, val_Pump, val_Temp, val_Humi, val_Soil);
  }

}

void MQTT_Connect(){ 
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if(client.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD)){
      Serial.println("Public emqx mqtt broker connected");
    }
    else{
      Serial.print("failed with state ");
      Serial.print(client.state());
    }
  }
}

void MQTT_Pub_Sub(){
	client.subscribe(TOPIC_SUB);
}

void callback(char *topic, byte *payload, unsigned int length){
	Serial.println("Message arrived in topic: " + String(topic));
  // Đọc và bóc tách dữ liệu từ payload
  readMQTTData((const char*)payload, length, js_autoMQTT, js_manualMQTT, js_led, js_fan, js_pump, js_temp, js_humi, js_soil);
}

void readMQTTData(const char* payload, unsigned int length, String& js_autoMQTT, String& js_manualMQTT, String& js_led, String& js_fan, String& js_pump, String& js_temp, String& js_humi, String& js_soil) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload, length);
  js_autoMQTT = doc["auto"].as<String>();
  js_manualMQTT = doc["manual"].as<String>();
  js_led = doc["led"].as<String>();
  js_fan = doc["fan"].as<String>();
  js_pump = doc["pump"].as<String>();
  js_temp = doc["temp"].as<String>();
  js_humi = doc["humi"].as<String>();
  js_soil = doc["soil"].as<String>();
  // chuyen doi data sang du lieu so
  convertToDataMQTT(js_autoMQTT, js_manualMQTT, js_led, js_fan, js_pump, js_temp, js_humi, js_soil);
}

void convertToDataMQTT(String& js_autoMQTT, String& js_manualMQTT, String& js_led, String& js_fan, String& js_pump, String& js_temp, String& js_humi, String& js_soil){
  val_AutoMQTT = js_autoMQTT.toInt();
  val_ManualMQTT = js_manualMQTT.toInt();
  val_LedMQTT = js_led.toInt();
  val_FanMQTT = js_fan.toInt();
  val_PumpMQTT = js_pump.toInt();
  val_TempMQTT = js_temp.toInt();
  val_HumiMQTT = js_humi.toInt();
  val_SoilMQTT = js_soil.toInt();
  Serial.println("Led: " + String(js_led));
  Serial.println("Fan: " + String(js_fan));
  Serial.println("Pump: " + String(js_pump));
  Serial.println("Temp: " + String(js_temp));
  Serial.println("Humi: " + String(js_humi  ));
  Serial.println("Soil: " + String(js_soil));
}

void sendSensorData(int val_Led, int val_Fan, int val_Pump, int val_Temp, int val_Humi, int val_Soil) {
  DynamicJsonDocument doc(256);
  doc["led"] = val_Led;
  doc["fan"] = val_Fan;
  doc["pump"] = val_Pump;
  doc["temp"] = val_Temp;
  doc["humi"] = val_Humi;
  doc["soil"] = val_Soil;
  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(TOPIC_SUB, buffer);
}