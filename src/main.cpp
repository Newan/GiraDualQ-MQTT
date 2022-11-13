
//           _____                    _____                    _____                    _____                    _____          
//          /\    \                  /\    \                  /\    \                  /\    \                  /\    \         
//         /::\____\                /::\    \                /::\____\                /::\    \                /::\____\        
//        /::::|   |               /::::\    \              /:::/    /               /::::\    \              /::::|   |        
//       /:::::|   |              /::::::\    \            /:::/   _/___            /::::::\    \            /:::::|   |        
//      /::::::|   |             /:::/\:::\    \          /:::/   /\    \          /:::/\:::\    \          /::::::|   |        
//     /:::/|::|   |            /:::/__\:::\    \        /:::/   /::\____\        /:::/__\:::\    \        /:::/|::|   |        
//    /:::/ |::|   |           /::::\   \:::\    \      /:::/   /:::/    /       /::::\   \:::\    \      /:::/ |::|   |        
//   /:::/  |::|   | _____    /::::::\   \:::\    \    /:::/   /:::/   _/___    /::::::\   \:::\    \    /:::/  |::|   | _____  
//  /:::/   |::|   |/\    \  /:::/\:::\   \:::\    \  /:::/___/:::/   /\    \  /:::/\:::\   \:::\    \  /:::/   |::|   |/\    \ 
// /:: /    |::|   /::\____\/:::/__\:::\   \:::\____\|:::|   /:::/   /::\____\/:::/  \:::\   \:::\____\/:: /    |::|   /::\____\
// \::/    /|::|  /:::/    /\:::\   \:::\   \::/    /|:::|__/:::/   /:::/    /\::/    \:::\  /:::/    /\::/    /|::|  /:::/    /
//  \/____/ |::| /:::/    /  \:::\   \:::\   \/____/  \:::\/:::/   /:::/    /  \/____/ \:::\/:::/    /  \/____/ |::| /:::/    / 
//          |::|/:::/    /    \:::\   \:::\    \       \::::::/   /:::/    /            \::::::/    /           |::|/:::/    /  
//          |::::::/    /      \:::\   \:::\____\       \::::/___/:::/    /              \::::/    /            |::::::/    /   
//          |:::::/    /        \:::\   \::/    /        \:::\__/:::/    /               /:::/    /             |:::::/    /    
//          |::::/    /          \:::\   \/____/          \::::::::/    /               /:::/    /              |::::/    /     
//          /:::/    /            \:::\    \               \::::::/    /               /:::/    /               /:::/    /      
//         /:::/    /              \:::\____\               \::::/    /               /:::/    /               /:::/    /       
//         \::/    /                \::/    /                \::/____/                \::/    /                \::/    /        
//          \/____/                  \/____/                  ~~                       \/____/                  \/____/         
//                                                                                                                              
//
// Version 0.1

#define WIFI_SSID "SSID-HERE"
#define WIFI_PASSWORD "PASSWORD-HERE"
#define MQTT_SERVER "MQTT-SERVER-HERE"
//----------------------------------------------
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC_ALARM "gira/alarm"
const char* msgTopic = "gira/alarm";
//----------------------------------------------
#define giraPin D5 // Setze den Interruptpin für Gira 2 Draht Signal
//----------------------------------------------
#define myTimeout  1000; //Wartezeit zwischen Peak für Alarmstatus
int alarmCount = 2; //mindeste abwarten von mytimout to alarm over mqtt
//----------------------------------------------
bool bCheckAlarm = false;
bool isAlarm = false;
long myTimer = 0;
//----------------------------------------------
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

// WiFi + MQTT
WiFiClient net;
PubSubClient client(net);

void IRAM_ATTR GiraPeak() {
  bCheckAlarm = false;
}

void setup() {
  Serial.begin(9600);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to SSID: ");
  Serial.println(WIFI_SSID);
  Serial.println("Begin1");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Begin2");

  Serial.println("Connecting.. status: " + String(WiFi.status()));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting.. status: " + String(WiFi.status()));
    Serial.print("....");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Setup Mqtt server");
  client.setServer(MQTT_SERVER, 1883);
  //client.setCallback(callback);

  Serial.println("Setup gira pins set");
  // Lege den Interruptpin als Inputpin mit Pull-Down-Widerstand fest
  //pinMode(interruptPin, INPUT);  

  // Lege Interrupt auf 3,3V HIGH für peak des Gira RM:
  attachInterrupt(digitalPinToInterrupt(giraPin), GiraPeak, RISING);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("smartcenter-esp", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      //Set default to false
      client.publish(msgTopic, "false");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  //Prüfe Timer und RM Status
  if (millis() >= myTimer) {
    //Setzen new time
    myTimer = millis() + myTimeout;
    Serial.println("Check time is over...");

    //check gira state
    if(!bCheckAlarm){
      //all good no alarm
      if (isAlarm) {
        //alarm is change, send it to mqtt
        client.publish(msgTopic, "false");
      }  
      isAlarm = false;
      Serial.println("No alarm");
    } else {
      //PANIK FIRE ALARM
      client.publish(msgTopic, "true");
      isAlarm = true;
      Serial.println("FIRE ALARM");
    }
    //Reset Gira Status
    bCheckAlarm = true;
  }

  client.loop();
  //reset alarm watch
  
  //Serial.println("LOOP");
}
