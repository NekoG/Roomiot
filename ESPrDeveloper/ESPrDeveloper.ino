#include <Ticker.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ThingSpeak.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "ir_Panasonic.h"


#include "MyConst.h"

//===================================================
// BME280
//===================================================
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

//===================================================
// ESP8266 Wifi
//===================================================
WiFiClient client;
#define SSID MY_SSID
#define PASS MY_PASS

//===================================================
// ThingSpeak
//===================================================
#define DELAY_TIME 20000
unsigned long delayTime = DELAY_TIME;
unsigned long myChannelID = MY_THINGSPEAK_CHANNEL_ID;
const char * myWriteAPIKey = MY_THINGSPEAK_APIKEY_WRITE;
#define FIELD_NUMBER_TEMPERATURE 1
#define FIELD_NUMBER_PRESSURE 2
#define FIELD_NUMBER_HUMIDITY 3
//===================================================
// ThingSpeak (AirConditioner State)
//===================================================
unsigned long myChannelID_State = MY_THINGSPEAK_CHANNEL_ID_STATE;
const char * myReadAPIKey_State = MY_THINGSPEAK_APIKEY_READ_STATE;
const char * myWriteAPIKey_State = MY_THINGSPEAK_APIKEY_WRITE_STATE;
#define FIELD_NUMBER_STATE 1
#define NO_ACTION 0
#define TOBE_TURN_ON 1
#define TOBE_TURN_OFF 2

//===================================================
// IRcontroll
//===================================================
uint16_t rawDataOff[439] = {3446, 1774,  400, 468,  400, 1338,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  398, 470,  400, 1338,  400, 468,  402, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 1338,  400, 1338,  402, 1338,  400, 468,  400, 470,  400, 1338,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 468,  400, 470,  400, 470,  398, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 1338,  402, 1338,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 10040,  3446, 1772,  402, 470,  400, 1338,  400, 470,  396, 472,  402, 468,  400, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 468,  400, 468,  400, 468,  400, 1340,  400, 470,  398, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 470,  398, 1340,  400, 1338,  400, 1338,  400, 470,  400, 470,  398, 1338,  400, 470,  398, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 468,  400, 470,  400, 1338,  402, 468,  400, 470,  400, 470,  398, 1338,  400, 470,  400, 1338,  400, 1338,  402, 468,  400, 468,  400, 470,  398, 470,  400, 470,  400, 470,  398, 470,  398, 470,  400, 468,  400, 1338,  400, 1338,  402, 1338,  402, 1338,  400, 1338,  400, 470,  400, 1338,  400, 468,  400, 1338,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 468,  400, 468,  400, 470,  400, 470,  400, 468,  400, 470,  398, 470,  400, 468,  400, 1338,  400, 1340,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 470,  400, 470,  400, 468,  400, 468,  400, 470,  400, 1336,  400, 1340,  402, 470,  400, 1338,  400, 470,  400, 468,  400, 470,  400, 468,  400, 470,  400, 468,  400, 468,  400, 468,  400, 468,  400, 470,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 470,  400, 468,  400, 470,  400, 470,  400, 1338,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 1338,  400, 1338,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  398, 1340,  400, 1338,  400, 470,  400, 1338,  402, 468,  400, 470,  400, 1340,  400};
uint16_t rawDataOn[439] = {3446, 1774,  402, 468,  400, 1338,  402, 468,  400, 468,  400, 468,  400, 470,  400, 470,  400, 468,  400, 468,  400, 468,  400, 468,  400, 470,  400, 470,  400, 1338,  400, 470,  400, 468,  398, 472,  400, 470,  400, 468,  400, 470,  400, 470,  400, 1338,  402, 1338,  402, 1338,  400, 470,  400, 470,  400, 1338,  400, 468,  400, 468,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  398, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  402, 1338,  400, 1338,  400, 470,  400, 470,  400, 468,  400, 468,  400, 470,  400, 10040,  3446, 1774,  400, 470,  398, 1340,  400, 468,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 468,  400, 470,  400, 470,  400, 1338,  400, 470,  400, 468,  400, 468,  400, 468,  400, 470,  400, 470,  400, 470,  400, 1338,  402, 1338,  402, 1338,  400, 470,  400, 468,  400, 1338,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 470,  396, 1340,  402, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 1338,  400, 468,  402, 468,  400, 470,  400, 1338,  400, 470,  400, 1338,  402, 1338,  400, 470,  398, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 470,  400, 1338,  402, 1338,  400, 1338,  400, 1338,  400, 1338,  402, 468,  400, 1338,  400, 468,  400, 1340,  400, 468,  400, 468,  400, 468,  400, 468,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 470,  398, 470,  400, 468,  400, 470,  400, 1338,  400, 1338,  402, 468,  400, 468,  400, 468,  400, 468,  400, 468,  400, 470,  400, 470,  398, 470,  400, 468,  400, 470,  400, 1338,  402, 1338,  400, 470,  400, 1338,  402, 470,  400, 470,  400, 468,  400, 470,  400, 470,  398, 470,  400, 468,  400, 468,  400, 468,  400, 470,  400, 470,  400, 470,  398, 470,  400, 468,  400, 468,  400, 470,  400, 470,  400, 468,  400, 470,  400, 470,  400, 470,  400, 468,  400, 1338,  402, 470,  400, 470,  400, 470,  400, 470,  400, 468,  400, 470,  400, 468,  402, 468,  400, 470,  400, 1338,  394, 1344,  400, 470,  400, 468,  400, 470,  400, 470,  398, 470,  400, 1340,  402, 1338,  400, 1338,  402, 468,  400, 1338,  402, 468,  400, 468,  400, 1338,  400};
const uint16_t kIrLed = 13;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.


void setup()
{
  // シリアル通信の開始
  Serial.begin(9600);
  
  // I2C通信の開始
  Serial.println("Start I2C");
  Wire.begin();

  // BME280 - 設定
  Serial.println("Start Setup BME280");
  setupBME280();

  // Wifi - 設定
  Serial.println("Start Setup WiFi");
  setupWifi();

  // ThingSpeak - 設定
  setupThingSpeak();

  // IRControl
  irsend.begin();
}

void setupBME280() {
  bool status;
    
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin();  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void setupWifi() {
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(SSID);

  tryWifiConnection();
}

// loop()中での再接続用に関数にしてある
void tryWifiConnection(){
//  WiFi.disconnect();
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(SSID);
  while(WiFi.status() != WL_CONNECTED){
    WiFi.begin(SSID, PASS);
    Serial.print(".");
    delay(5000);
  } 
  Serial.println("\nConnected.");
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
}

void setupThingSpeak(){
  ThingSpeak.begin(client);
  // ThingSpeak(無料枠)の仕様に合わせて間隔を設定
  delayTime = DELAY_TIME;
}

void loop() { 
//  printValues();
  printValuesCompact();

  sendSensorValue();

  int ac_state = checkAirConditionerState();
  Serial.println("AC state: "+String(ac_state));

//  sample code for automation
  if(ac_state == TOBE_TURN_ON){
    sendAirConditionerOn();
    setAirConditionerNoAction();
  }else if(ac_state == TOBE_TURN_OFF){
    sendAirConditionerOff();
    setAirConditionerNoAction();
  }

  delay(delayTime);
}


void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    Serial.println();
}

void printValuesCompact() {
    Serial.print(bme.readTemperature());
    Serial.print(", ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.print(", ");

    Serial.print(bme.readHumidity());
    Serial.println();
}

int checkAirConditionerState(){
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    tryWifiConnection();
  }

  // Check state
  int fieldToCheck = 1;
  int ac_state = ThingSpeak.readIntField(myChannelID_State, 1, myReadAPIKey_State);
  
  return ac_state;
}

void sendSensorValue() {
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    tryWifiConnection();
  }

  // Get sensor value
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  
  // Fieldを一個ずつ書き込む方式は、無料枠の更新間隔制限(15秒)のため失敗する
  // setFieldで書き込む値をすべてセットした後、writeFields()を使ってまとめて書き込む
//  int code = ThingSpeak.writeField(myChannelID, FIELD_NUMBER_TEMPERATURE, temperature, myWriteAPIKey);
//  checkReturnCode(FIELD_NUMBER_TEMPERATURE, code);
//  code = ThingSpeak.writeField(myChannelID, FIELD_NUMBER_PRESSURE, pressure, myWriteAPIKey);
//  checkReturnCode(FIELD_NUMBER_PRESSURE, code);
//  code = ThingSpeak.writeField(myChannelID, FIELD_NUMBER_HUMIDITY, humidity, myWriteAPIKey);
//  checkReturnCode(FIELD_NUMBER_HUMIDITY, code);

  // Call setField() for each of the fields you want to write first.
  int code = ThingSpeak.setField(FIELD_NUMBER_TEMPERATURE, temperature);
  code = ThingSpeak.setField(FIELD_NUMBER_PRESSURE, pressure);
  code = ThingSpeak.setField(FIELD_NUMBER_HUMIDITY, humidity);
  // Write a multi-field update.
  code = ThingSpeak.writeFields(myChannelID, myWriteAPIKey);
  checkReturnCode(code);
  
}

void checkReturnCode(int code) {
  if(code == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(code));
  }
}

void sendAirConditionerOn(){
  Serial.println("Air Conditioner On");
  irsend.sendRaw(rawDataOn, 439, kPanasonicFreq); 
}

void sendAirConditionerOff(){
  Serial.println("Air Conditioner Off");
  irsend.sendRaw(rawDataOff, 439, kPanasonicFreq); 
}

void setAirConditionerNoAction(){
  int code = ThingSpeak.writeField(myChannelID_State, FIELD_NUMBER_STATE, NO_ACTION, myWriteAPIKey_State);
  checkReturnCode(code);
}
