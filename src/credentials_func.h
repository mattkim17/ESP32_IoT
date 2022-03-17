#ifndef H_CREDENTIALS_FUNC_H
#define H_CREDENTIALS_FUNC_H
#include <Arduino.h> //allows for String type vars

//---------------------------------------Declare Global Variables---------------------------------------------------------- 

extern const char* networkSSID; // Complete with your network SSID (name)
extern const char* networkPASSWORD; // Complete with your network password (use for WPA, or use as key for WEP)
extern const char* mqttSERVER; 
extern const char* mqttUSERNAME; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username
extern const char* mqttPASSWORD; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username

// const char* API_HOST = "https://api.swx.altairone.com";
// const char* updateps_func = "/spaces/ebike2/functions/updateproperties/invoke" ;
// const char* opentrack_path = "/spaces/ebike2/functions/opentracker3/invoke";
// const char* tokenserver = "https://api.swx.altairone.com/oauth2/token"; 

extern const char* client_id; //SmartWorks AppID
extern const char* client_secret; //SmartWorks App Secret PW
// const char* temp_token = "1Z-k8QAzHijmW1kG_fBPnzrthXlW6BEfDnKWLsHosdc.FPsKHZYCqCKhx0faq-UomU-1ZrG7H6rgGiBEhpdGEXY";
//char publishTopic_data[] = "set/ebike2/collections/esp32_data/things/"; //thing_id/data"

extern const char* ca_cert;

extern String uid;
extern String publishTopic_datas;
extern String publishTopic_props; 
extern String publishTopic_props1;
extern String mqtt_user; 


//------------------------------------------Header Functions------------------------------------------------------------------
// void set_globals(); 

const char* read_mqtt_user(String httpResponse);
String get_mqtt(const char* certificate, const char* client_id, const char* client_secret, String nuid);
void post_properties(const char* certificate, const char* client_id, const char* client_secret, char* payload, String nuid);
String run_uid(const char* certificate, const char* client_id, const char* client_secret);
String get_uid(const char* certificate, const char* client_id, const char* client_secret); 
int revoke(const char* token, const char* certificate, const char* client_id, const char* client_secret); 
const char* get_access_token(const char* certificate, const char* client_id, const char* client_secret); 

#endif