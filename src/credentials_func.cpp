#include <Arduino.h>
#include <wifi-mqtt.h>
#include <delay.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "BLEDevice.h"
//#include "BLEScan.h"
// #include <topics_publish.h>

// The remote service we wish to connect to.
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "credentials_func.h"

//--------------------------------------------------Initialize Vars -------------------------------------------------------------
extern const char* ca_cert; 
extern const char* networkSSID; // Complete with your network SSID (name)
extern const char* networkPASSWORD; // Complete with your network password (use for WPA, or use as key for WEP)

//--------------------------------------------------HTTPS Functions -------------------------------------------------------------

//-----------------------------------------get_access_token function------------------------------------------------- 

const char* get_access_token(const char* certificate, const char* client_id, const char* client_secret){
  
    HTTPClient http;
    WiFiClient client;
    const char* server = "https://api.swx.altairone.com/oauth2/token";
    http.begin(server, certificate); //OAuth Token URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
    // Your Domain name with URL path or IP address with path
    http.begin(server, certificate);

    // Data to send with HTTP POST --> API_HOST + PATH
    String httpRequestData = "client_id=" + String(client_id) + "&client_secret=" + String(client_secret)\
    + "&grant_type=client_credentials&scope=data+thing+function";
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Send HTTP POST request 
    int httpResponseCode = http.POST(httpRequestData);

    //extra print statements
//    Serial.println(httpRequestData);
//    Serial.print("HTTP Response code: ");
//    Serial.println(httpResponseCode);
//    Serial.println("HTTP Response: ");
 
    DynamicJsonDocument doc(500); //allocating memory --> https://arduinojson.org/v6/assistant/
    //deserializeJson(doc, http.getStream()); //http.getStream is used to read http JSON 
 
    DeserializationError error = deserializeJson(doc, http.getString()); //deserialize JSON data and get access token

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      
    }
    Serial.println("Access Token: ");
    Serial.println(doc["access_token"].as<const char*>()); 
    const char* token = doc["access_token"].as<const char*>();
    http.end(); //Free the resources
    
    return token; 
    
  }
  else {
      Serial.println("WiFi Disconnected");
      return NULL;
    }
}

//--------------------------------------revoke token function-------------------------------------------
int revoke(const char* token, const char* certificate, const char* client_id, const char* client_secret){
    HTTPClient http;
    WiFiClient client;
    const char* server = "https://api.swx.altairone.com/oauth2/revoke";
    http.begin(server, certificate); //OAuth Token URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
      // Your Domain name with URL path or IP address with path
      http.begin(server, certificate);
  
      // Data to send with HTTP POST --> API_HOST + PATH
      String httpRequestData = "token=" + String(token) + "&client_id=" + String(client_id) + "&client_secret=" + String(client_secret);
      
      Serial.println(httpRequestData); 
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      // Send HTTP POST request 
      int httpResponseCode = http.POST(httpRequestData);
      //String httpResponse = http.getString(); 
      
      Serial.print("HTTP Revoked Response: ");
      Serial.println(httpResponseCode);
     
      http.end(); //Free the resources

      return httpResponseCode; 
    
    }else{
      return 0; 
    }
}


//---------------------------------------------get_uid function------------------------------------------
//const char*
String get_uid(const char* certificate, const char* client_id, const char* client_secret){
    //--------------------------- Getting Token ---------------------------------------------------------
    HTTPClient http;
    WiFiClient client;
    const char* server = "https://api.swx.altairone.com/oauth2/token";
    http.begin(server, certificate); //OAuth Token URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
    // Your Domain name with URL path or IP address with path
    http.begin(server, certificate);

    // Data to send with HTTP POST --> API_HOST + PATH
    String httpRequestData = "client_id=" + String(client_id) + "&client_secret=" + String(client_secret)\
    + "&grant_type=client_credentials&scope=data+thing+function";
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Send HTTP POST request 
    int httpResponseCode = http.POST(httpRequestData);
    //String httpResponse = http.getString(); 
    
    //extra print statements
//    Serial.println(httpRequestData);
//    Serial.print("HTTP Response code: ");
//    Serial.println(httpResponseCode);
//    Serial.println("HTTP Response: ");
 
    DynamicJsonDocument doc(500); //allocating memory --> https://arduinojson.org/v6/assistant/
    //deserializeJson(doc, http.getStream()); //http.getStream is used to read http JSON 
 
    DeserializationError error = deserializeJson(doc, http.getString()); //deserialize JSON data and get access token

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      
    }
    Serial.println("Access Token - opentracker3: ");
    Serial.println(doc["access_token"].as<const char*>()); 
    const char* token = doc["access_token"].as<const char*>();
    http.end(); //Free the resources ==> closing 'http' client
    

    //------------------------------------Invoking Function - opentracker3 ------------------------------------

    HTTPClient http; //start new HTTP client
    WiFiClient client;
    const char* func_server = "https://api.swx.altairone.com/spaces/ebike2/functions/opentracker3/invoke";
    http.begin(func_server, certificate); //server URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
    // Your Domain name with URL path or IP address with path
    http.begin(func_server, certificate);

    

    //extra print statements
//    Serial.print("Token not string: ");
//    Serial.println(token); 
//    Serial.println("Bearer " + String(token));
    
    http.addHeader("Authorization", "Bearer " + String(token));


    //test sending JSON
    // Send HTTP POST request 
    int httpResponseCode = http.POST("{\"empty\":\"empty\"}");
    String httpResponse = http.getString(); 


    //extra print statements
//    Serial.println(httpRequestData);
//    Serial.print("HTTP Response code (invoked): ");
//    Serial.println(httpResponseCode);
//    Serial.println("HTTP Response - Invoked: ");
//    Serial.println(httpResponse); 
    
    int revoke_message = revoke(token, ca_cert, client_id, client_secret); //Revoking Token 
    return httpResponse; 
    }
   }
  }


//---------------------------------------------run_uid function------------------------------------------


//----------------------------get uid of "opened" object---------------------------------------------------
String run_uid(const char* certificate, const char* client_id, const char* client_secret){
  String uid_num = get_uid(ca_cert, client_id, client_secret); //http response string for UID
  Serial.print("OPENED UID: "); 
  Serial.println(uid_num);

  //---------------------------Extracting UID from HTTP Response----------------------------------------------
  DynamicJsonDocument doc(4096); //allocating memory --> https://arduinojson.org/v6/assistant/
    //deserializeJson(doc, http.getStream()); //http.getStream is used to read http JSON 
 
  DeserializationError error = deserializeJson(doc, uid_num); //deserialize JSON data 

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    
  }
  Serial.println("UID: ");
  Serial.println(doc["data"][0]["uid"].as<char*>()); 
  const char* uid = doc["data"][0]["uid"].as<char*>();
  //String uid_str = String(uid); 

  return String(uid); 
  
}


//--------------------------------------POSTing Properties Function------------------------------------------------
void post_properties(const char* certificate, const char* client_id, const char* client_secret, char* payload, String nuid){
    //--------------------------- Getting Token ---------------------------------------------------------
    HTTPClient http;
    WiFiClient client;
    const char* server = "https://api.swx.altairone.com/oauth2/token";
    http.begin(server, certificate); //OAuth Token URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
    // Your Domain name with URL path or IP address with path
    http.begin(server, certificate);

    // Data to send with HTTP POST --> API_HOST + PATH
    String httpRequestData = "client_id=" + String(client_id) + "&client_secret=" + String(client_secret)\
    + "&grant_type=client_credentials&scope=data+thing+function";
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Send HTTP POST request 
    int httpResponseCode = http.POST(httpRequestData);
    //String httpResponse = http.getString(); 

    //extra print statements
//    Serial.println(httpRequestData);
//    Serial.print("HTTP Response code: ");
//    Serial.println(httpResponseCode);
//    Serial.println("HTTP Response: ");
 
    DynamicJsonDocument doc(500); //allocating memory --> https://arduinojson.org/v6/assistant/
    //deserializeJson(doc, http.getStream()); //http.getStream is used to read http JSON 
 
    DeserializationError error = deserializeJson(doc, http.getString()); //deserialize JSON data and get access token

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      
    }
    Serial.println("Access Token - updating properties: ");
    Serial.println(doc["access_token"].as<const char*>()); 
    const char* token = doc["access_token"].as<const char*>();
    http.end(); //Free the resources ==> closing 'http' client
    

    //------------------------------------Invoking Function - updateproperties2 ------------------------------------

    HTTPClient http; //start new HTTP client
    WiFiClient client;
    const char* func_server = "https://api.swx.altairone.com/spaces/ebike2/functions/updateproperties2/invoke";
    http.begin(func_server, certificate); //server URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
    // Your Domain name with URL path or IP address with path
    http.begin(func_server, certificate);

    
    // Data to send with HTTP POST --> API_HOST + PATH
    //String httpRequestData = "{ }";
    
    // Specify content-type header
    Serial.print("Update Token: ");
    Serial.println(token); 
    Serial.println("Bearer " + String(token));
    
    http.addHeader("Authorization", "Bearer " + String(token));

    //test sending JSON
    // Send HTTP POST request 
    String httpRequestData = "{\"uid\":\"" + nuid + "\", \"properties\":" + payload + "}";
    int httpResponseCode = http.POST(httpRequestData);
    

    Serial.print("Data payload for UPDATES: ");
    Serial.println(httpRequestData);
  
    Serial.print("HTTP Response - updated: ");
    Serial.println(httpResponseCode);
   
    
    int revoke_message = revoke(token, ca_cert, client_id, client_secret); //Revoking Token 
    http.end();
    }
   }
  }


//--------------------------------------MQTT username function------------------------------------------------
String get_mqtt(const char* certificate, const char* client_id, const char* client_secret, String nuid){
    //------------------------------------- Getting Token ----------------------------------------------------
    HTTPClient http;
    WiFiClient client;
    const char* server = "https://api.swx.altairone.com/oauth2/token";
    http.begin(server, certificate); //OAuth Token URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
    // Your Domain name with URL path or IP address with path
    http.begin(server, certificate);

    // Data to send with HTTP POST --> API_HOST + PATH
    String httpRequestData = "client_id=" + String(client_id) + "&client_secret=" + String(client_secret)\
    + "&grant_type=client_credentials&scope=data+thing+function+mqtt";
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Send HTTP POST request 
    int httpResponseCode = http.POST(httpRequestData);
    //String httpResponse = http.getString(); 
    
    //extra print statements
//    Serial.println(httpRequestData);
//    Serial.print("HTTP Response code: ");
//    Serial.println(httpResponseCode);
//    Serial.println("HTTP Response: ");
 
    DynamicJsonDocument doc(500); //allocating memory --> https://arduinojson.org/v6/assistant/
    //deserializeJson(doc, http.getStream()); //http.getStream is used to read http JSON 
 
    DeserializationError error = deserializeJson(doc, http.getString()); //deserialize JSON data and get access token

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      
    }
    Serial.println("Access Token - MQTT: ");
    Serial.println(doc["access_token"].as<const char*>()); 
    const char* token = doc["access_token"].as<const char*>();
    http.end(); //Free the resources ==> closing 'http' client
    

    //------------------------------------API call for MQTT Username ------------------------------------

    HTTPClient http; //start new HTTP client
    WiFiClient client;
    //String server = "https://api.swx.altairone.com/spaces/ebike2/mqtt/things/" + String(nuid); 
    String server = "https://api.swx.altairone.com/spaces/ebike2/collections/esp32_data/things/" + String(nuid) +"/mqtt-credentials"; 
    Serial.print("Server for MQTT: ");
    Serial.println(server);


    const char* func_server = server.c_str();
    http.begin(func_server, certificate); //server URL & Certificate to access HTTPS 
    
    http.addHeader("Authorization", "Bearer " + String(token));
    int httpCode = http.GET(); //Make the request
    Serial.print("MQTT RESPONSE CODE: ");
    Serial.println(httpCode);

    if (httpCode > 0) { //Check for the returning code
      String response = http.getString(); 
      Serial.println("MQTT RESPONSE OBJECT: "); 
      Serial.println(response); 
      
    int revoke_message = revoke(token, ca_cert, client_id, client_secret); //Revoking Token 
    http.end();

    return response;
    }
   }
  }

const char* read_mqtt_user(String httpResponse){
  DynamicJsonDocument doc(4096); //allocating memory --> https://arduinojson.org/v6/assistant/
    //deserializeJson(doc, http.getStream()); //http.getStream is used to read http JSON 
 
  DeserializationError error = deserializeJson(doc, httpResponse); //deserialize JSON data and get access token

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    
  }

  Serial.println("MQTT Username: ");
  Serial.println(doc["data"][0]["username"].as<const char*>()); 
  const char* mqtt_user = doc["data"][0]["username"].as<const char*>();
    
  return mqtt_user; 
}


//--------------------------------------------- Set GLOBAL VARIABLES ------------------------------------

// void set_globals(){
//     Serial.println("Starting Setup: 'UID', 'publishTopic', and 'MQTT_user' retrieval");
//     uid = run_uid(ca_cert, client_id, client_secret); 
//     publishTopic_datas = "set/ebike2/collections/esp32_data/things/" + String(uid) + "/data"; 
//     publishTopic_props = "set/ebike2/collections/esp32_data/things/" + String(uid) + "/properties/bpm";

//     //publishTopic_data = publishTopic.c_str(); //publishTopic link to send raw history data to
//     Serial.print("PublishTopic Link: "); 
//     Serial.println(publishTopic_datas);

//     String mqtt_response = get_mqtt(ca_cert, client_id, client_secret, uid); //retrieving MQTT username HTTP response
//     mqttUSERNAME = read_mqtt_user(mqtt_response).c_str(); //retrieving MQTT Username
//     Serial.print("MQTT USERNAME: "); 
//     Serial.println(mqttUSERNAME);
// }



// String retrieve_uid(){
//   uid = run_uid(ca_cert, client_id, client_secret);
//   return uid; 
// }