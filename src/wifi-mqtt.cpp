/* This file stores the functions needed to connect to both your WIFI and MQTT broker.
The following libraries are needed:
-. SPI.h:  allows you to communicate with SPI devices, with the Arduino as the master device.
-. WiFiNINA.h: allows you to use the following Arduinos' capabilities: UNO, NANO 33 IoT,MKR 1010 and MKR VIDOR 4000 WiFi.
-. PubSubClient.h: allows for MQTT messaging. 
-. credentials.h: stores the credentials needed for the WIFI and MQTT connection. 
-. wifi-mqtt.h: allows to connect to your WIFI and to the MQTT broker. Also allow for publish and subscribe from it.
-. delay.h: allows to get the delay value from the platform
-. topics_subscribe.h: stores the subscribe topic

Reference: arduino.cc
*/

#include <SPI.h>
#include <WiFi.h> 
#include <PubSubClient.h>
// #include <credentials.h>
#include <wifi-mqtt.h>
#include <delay.h>
#include <topics_subscribe.h>
#include <HTTPCLient.h>   //added to execute HTTP requests 

#include "credentials_func.h"

//Get the sensitive data stored at credentials.h

extern const char* ca_cert;
extern const char* networkSSID; // Complete with your network SSID (name)
extern const char* networkPASSWORD; // Complete with your network password (use for WPA, or use as key for WEP)
extern const char* mqttSERVER; 
extern const char* mqttUSERNAME; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username
extern const char* mqttPASSWORD; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username

const char* mqttServer = mqttSERVER; // SmartWorks MQTT host
const char* ssid = networkSSID; // your network SSID (name)
const char* password = networkPASSWORD; // your network password


const char* mqttUsername = mqttUSERNAME; // MQTT Username of your thing in SmartWorks IoT platform
const char* mqttPassword = mqttPASSWORD; //MQTT password of your thing in SmartWorks IoT platform

int status = WL_IDLE_STATUS; //Initialice the Wifi radio's status

WiFiClient wifiClient; // Initialize the client library
PubSubClient pubclient(wifiClient); //Creates a client that can connect to the specified WIFI 

// Callback function handles the messages that are returned from the broker on our subscribed channel
void callback(char* subscribeTopic, byte* payload, unsigned int length){

  Serial.print("Message arrived to "); //Check whenever a message is received to the subscribed topic
  Serial.print(subscribeTopic);
  parse_delay(subscribeTopic,payload, length); //Converts the received message to get the delay value as an integer

}

//wifi_mqtt_init function initializes wifi connection
void wifi_mqtt_init(){
  // Attempt to connect to Wifi network:
  while (status != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, password);
    // wait 10 seconds for connection:
    delay(10000);
  }

  // Connection successfull, print out the related data
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //Sets the mqtt server details: host and port
  Serial.println(mqttSERVER);
  pubclient.setServer(mqttSERVER, 1883);
  //Sets the message callback function for subscribing to the specify topic
  pubclient.setCallback(callback);
  delay(200);

}

//mqtt_reconnect function initializes or reestablished MQTT connection
void mqtt_reconnect(){
// Loop until we're reconnected
  while (!pubclient.connected())
  {
     // Attempt to connect to MQTT broker
    Serial.println("Attempting MQTT thing connection...\n");
    Serial.print("mqttUSERNAME GLOBAL: ");
    Serial.println(mqttUSERNAME); 
    Serial.print("mqttPASSWORD GLOBAL: ");
    Serial.println(mqttPASSWORD); 

    // Try to connect to SmartWorks MQTT Broker, give it Arduino as the name
    if (pubclient.connect("Arduino", mqttUSERNAME, mqttPASSWORD))  //attempting to use global password 
    {
      // Connection successfull, print out the related data
      Serial.print("connected\n");
      // printf("connected\n");
      pubclient.subscribe(subscribeTopic);
     
    } else{
      // Connection not successful, print out error message
      Serial.print("failed, rc=");
      printf("fail");
      Serial.print(pubclient.state());
      // Wait 5 seconds before retrying
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    
  }

}

// wifi_mqtt_send function sends data to the platform
void wifi_mqtt_send(char sentence[200], char pubTopic[300]){
// If the connection is lost, reconnect 
  if (!pubclient.connected()){
    Serial.println("pubclient is not connected when sending!!!!");
    mqtt_reconnect();
  }
  // Allow the client to process incoming messages and maintain its connection to the server.
  pubclient.loop();
  //Publishes a message to the specified topic.
  pubclient.publish(pubTopic, sentence);
  Serial.print("New message sent: ");
  Serial.println(pubTopic);
  
}
