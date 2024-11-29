#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Arduino.h"

// wifi name
#define WLAN_SSID       "ACLAB"
// wifi password
#define WLAN_PASS       "ACLAB2023"

// setup Adafruit 
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
// username 
#define AIO_USERNAME    "vunguyen883" 
// key 
// feed key   
#define FEED_KEY        "/feeds/temperature"

// private define
#define LED             2
#define BUSY            5 
#define START_IDX       0
#define DATA_IDX        5
#define END_IDX         7

// setup MQTT 
WiFiClient client; 
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY); 

//set publish 
Adafruit_MQTT_Publish publisher = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME FEED_KEY);

uint8_t buffer[20];
uint8_t len = 0; 
uint16_t temperature;
uint8_t counter = 50; 

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT); 
  pinMode(BUSY, OUTPUT); 

  // is setting up - esp is busy 
  digitalWrite(BUSY, HIGH); 
  digitalWrite(LED, HIGH); 

  // serial 
  Serial.begin(115200); 

  // connect wifi 
  WiFi.begin(WLAN_SSID, WLAN_PASS); 
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  // connect mqtt 
  while(mqtt.connect() != 0){
    delay(500); 
  }

  // finish set up - esp is idle
  digitalWrite(BUSY, LOW); 
  digitalWrite(LED, LOW);
}

uint8_t option = 1; 
void loop() {
  // put your main code here, to run repeatedly:

  if(counter <= 0){
    counter = 50; 

    if(Serial.available()){
      while(Serial.available()){
        buffer[len++] = Serial.read(); 
      }

      if(buffer[START_IDX] == '!' && buffer[END_IDX] == '#'){
        // rec data 
        memcpy(&temperature, buffer + DATA_IDX, 2); 
        // publish data to feed 
        publisher.publish(temperature); 
      }

      // clear buffer
      len = 0; 
    }
    
  }
  else counter--; 

  //busy ---> led 4
  //led ---> esp led 

  delay(10); 
}
