# MQTT Support for Gira Dual Q

## Hardware
 - Gira Dual q RM
 - Wemos D1 mine or something like this

 - Add GND from RM to GND on D1
 - Add 2-Pin-Bus from gira to a digital pin like D5
 - Change this line <code>#define giraPin D5 // Setze den Interruptpin für Gira 2 Draht Signal</code>

## Configuration Wifif and Mqtt
Change the following config variables
```
    #define WIFI_SSID "SSID-HERE"
    #define WIFI_PASSWORD "PASSWORD-HERE"
    #define MQTT_SERVER "MQTT-SERVER-HERE"
    //----------------------------------------------
    #define MQTT_USER ""
    #define MQTT_PASSWORD ""
    #define MQTT_TOPIC_ALARM "gira/alarm"
    const char* msgTopic = "gira/alarm";
```

## Dependencies
- https://github.com/knolleary/pubsubclient/tree/v2.8