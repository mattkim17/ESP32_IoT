/* This file stores the execution of the blink project, that allows to control the on-board LED from your Arduino board.
NOTE: On the UNO, MEGA and ZERO it is attached to digital pin 13, on MKR1000 on pin 6.
LED_BUILTIN is set to the correct LED pin independent of which board is used.

The following libraries are needed:
-. Arduino.h: includes Arduino specific commands for coding
-. wifi-mqtt.h: allows to connect to your WIFI and to the MQTT broker. Also allow for publish and subscribe from it.
-. delay.h: allows to get the delay value from the platform
-. Blink.h: stores the logic to make the on-board LED blink
-. SPI.h: This library allows you to communicate with SPI devices, with the Arduino as the master device.
-. ESP8266WiFi.h: This library i sused to connect ESP8266 WiFi module to the wifi
-. PubSubClient.h: A client library for MQTT messaging.
-. DHT.h: Arduino library for DHT11, DHT22, etc Temp & Humidity Sensors
-. humidity.h: stores the logic to get data from humidity sensor

Reference: arduino.cc
*/

#include <Arduino.h>
#include <wifi-mqtt.h>
#include <delay.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

// #include <DHT.h>
// #include <humidity.h>

#include "BLEDevice.h"
//#include "BLEScan.h"
// #include <topics_publish.h>
// The remote service we wish to connect to.

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "credentials_func.h"


static BLEUUID serviceUUID("0000180d-0000-1000-8000-00805f9b34fb"); //PUT heart monitor serviceUUID
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID(BLEUUID((uint16_t)0x2A37));

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static boolean notification = false; //added

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

//--------------------------------------------- DEFINE GLOBAL VARS --------------------------------------------------------
const char* networkSSID = "Device-Northwestern"; // Complete with your network SSID (name)
const char* networkPASSWORD = ""; // Complete with your network password (use for WPA, or use as key for WEP)
const char* mqttSERVER = "mqtt.swx.altairone.com"; 
const char* mqttUSERNAME; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username
const char* mqttPASSWORD = "ebike"; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username

// const char* API_HOST = "https://api.swx.altairone.com";
// const char* updateps_func = "/spaces/ebike2/functions/updateproperties/invoke" ;
// const char* opentrack_path = "/spaces/ebike2/functions/opentracker3/invoke";
// const char* tokenserver = "https://api.swx.altairone.com/oauth2/token"; 

const char* client_id = "app::01FX17QYTEGXHECC68X1PAW7T5"; //SmartWorks AppID
const char* client_secret = ""; //SmartWorks App Secret PW
// const char* temp_token = "";
//char publishTopic_data[] = "set/ebike2/collections/esp32_data/things/"; //thing_id/data"

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"\
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"\
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"\
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"\
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"\
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"\
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"\
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"\
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"\
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"\
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"\
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"\
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"\
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"\
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"\
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"\
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"\
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"\
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"\
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"\
"-----END CERTIFICATE-----\n";

String publishTopic_datas;
String publishTopic_props;
String publishTopic_props1;
String uid; 

// -------------------------------------------------FUNCTIONS---------------------------------------------------------------

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
    char json_heartrate_data[] = "{ \"bpm\":\"%i\", \"assist_level\":\"%i\" }";
    
    if (length < 20){ // ==4 does not include 6, 8
      Serial.print(pData[1], DEC);
      Serial.println("bpm");
      int bpm = ((int)pData[1]);
      int assist_level; 
      //Sending Voltage to Motor 
      if (pData[1]<= 90){
      Serial.println("level 1");
      assist_level = 1;
      dacWrite(26,159); 
      delay(1000); 
      }else if((pData[1]>= 91) && (pData[1] <= 110)){
        Serial.println("level 2");
        assist_level = 2;
        dacWrite(26,175); 
        delay(1000);   
      }else if((pData[1]>= 111) && (pData[1] <= 130)){
        Serial.println("level 3");
        assist_level = 3;
        dacWrite(26,188); 
        delay(1000);
      }
      else if((pData[1]>= 131) && (pData[1] <= 150 )){
        Serial.println("level 4");
        assist_level = 4;
        dacWrite(26,231); 
        delay(1000);
      }else{
        Serial.println("level 5");
        assist_level = 5;
        dacWrite(26,250); 
        delay(1000);
      }

      char json_char_heartrate[100]="";
      snprintf(json_char_heartrate, sizeof(json_char_heartrate), json_heartrate_data, bpm, assist_level);

      publishTopic_datas = "set/ebike2/collections/esp32_data/things/" + uid + "/data"; 
      publishTopic_props = "set/ebike2/collections/esp32_data/things/" + uid + "/properties/bpm";
      publishTopic_props1 = "set/ebike2/collections/esp32_data/things/" + uid + "/properties/assist_level";

      
      // Serial.print("publishTopic_datas: ");
      // Serial.println(publishTopic_datas);
      // Serial.print("publishTopic_properties: ");
      // Serial.println(publishTopic_props);


      int data_size = publishTopic_datas.length() + 1;
      char raw_buffer[data_size];
      publishTopic_datas.toCharArray(raw_buffer, data_size);

      int prop_size = publishTopic_props.length() + 1;
      char prop_buffer[prop_size];
      publishTopic_props.toCharArray(prop_buffer,prop_size);

      int prop_size1 = publishTopic_props1.length() + 1;
      char prop_buffer1[prop_size1];
      publishTopic_props1.toCharArray(prop_buffer1,prop_size1);


      // Serial.print("Raw Buffer: ");
      // Serial.println(raw_buffer);
      // Serial.print("Prop Buffer: ");
      // Serial.println(prop_buffer);
      // Serial.print("JSON DATA: ");
      // Serial.println(json_char_heartrate); 

      wifi_mqtt_send(json_char_heartrate, raw_buffer); // Publish a message to the raw history section
      wifi_mqtt_send(json_char_heartrate, prop_buffer); // Publish a message to the property topic
      wifi_mqtt_send(json_char_heartrate, prop_buffer1); // Publish a message to the property topic
      Serial.println("bpm recorded"); // Message to check the led is on at PlatformIO Serial Monitor
      delay(return_delay()); // Set the delay based on the value enter in the delay property in SmartWorks IoT platform
    }

}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


// --------------------------------------------------- VOID SETUP & LOOP -------------------------------------------------------------


// setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200); // Open serial communications
  delay(4000);
  
  WiFi.begin(networkSSID, networkPASSWORD);

  // -------------------------TRYING LOCAL MQTT-----------------------------------

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");

  uid = run_uid(ca_cert, client_id, client_secret); 
  String mqtt_response = get_mqtt(ca_cert, client_id, client_secret, uid); //retrieving MQTT username HTTP response
  mqttUSERNAME = read_mqtt_user(mqtt_response); //retrieving MQTT Username

  Serial.print("MQTT USERNAME: ");
  Serial.println(mqttUSERNAME);

  Serial.print("mqttPASSWORD: ");
  Serial.println(mqttPASSWORD);

  WiFi.disconnect(); //added to disconnect this wifi shield

  //---------------------------END OF ATTEMPT----------------------------------------

  wifi_mqtt_init(); // Initialize wifi connection and mqtt server
  mqtt_reconnect(); // Initialize MQTT connection
  init_delay(1000); //Set a default delay of 1000 miliseconds

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

}


// loop function runs over and over 
void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

//  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
//  // with the current time since boot.
  if (connected){
    if (notification == false){
      Serial.println("Turning Notifs On");
      const uint8_t onPacket[] = {0x01, 0x0};
      pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)onPacket, 2, true);

      notification = true; 
    }
  }
}


