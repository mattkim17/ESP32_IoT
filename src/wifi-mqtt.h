#ifndef H_WIFI_MQTT_H
#define H_WIFI_MQTT_H
void wifi_mqtt_init();
void wifi_mqtt_send(char sentence[200], char pubTopic[300]);
void mqtt_reconnect();

#endif