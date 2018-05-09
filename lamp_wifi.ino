#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LAMP_ID                         ROOM "/" LAMP_NAME
#define TOPIC_PRESENCE                  "/wohnung/anwesenheit"
#define VALUE_TOPIC_PRESENCE_HELLO      LAMP_ID " zu Diensten!"
#define TOPIC_SWITCH                    "/wohnung/" LAMP_ID "/kontrollen/schalter"
#define VALUE_TOPIC_SWITCH_ON           "an"
#define VALUE_TOPIC_SWITCH_OFF          "aus"
#define TOPIC_BRIGHTNESS                "/wohnung/" LAMP_ID "/kontrollen/helligkeit"
#define TOPIC_STATUS_STATE              "/wohnung/" LAMP_ID "/status/schaltzustand"
#define TOPIC_STATUS_BRIGHTNESS         "/wohnung/" LAMP_ID "/status/helligkeit"

// edit these in config.h
extern const char* ssid; 
extern const char* password;
extern const char* mqtt_server;

unsigned long lastMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setupNetworking() {
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  String _topic = topic;
  String _payload = "";
  for (int i = 0; i < length; i++) {
    _payload += (char)payload[i];
  }
  if(_topic == TOPIC_SWITCH){
    if( _payload == VALUE_TOPIC_SWITCH_ON ) targetStatus = 1;
    if( _payload == VALUE_TOPIC_SWITCH_OFF ) targetStatus = 0;
  }
  if(_topic == TOPIC_BRIGHTNESS){
    overallBrightness = _payload.toInt();
    client.publish(TOPIC_STATUS_BRIGHTNESS, String(overallBrightness).c_str(), true);
  }

}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(LAMP_ID)) {
      client.publish(TOPIC_PRESENCE, VALUE_TOPIC_PRESENCE_HELLO);
      client.subscribe(TOPIC_SWITCH);
      client.subscribe(TOPIC_BRIGHTNESS);
      updateStatus(lampStatus);
      client.publish(TOPIC_STATUS_BRIGHTNESS, String(overallBrightness).c_str(), true);
    } else {
      delay(5000);
    }
  }
}
void loopNetworking() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}

void updateStatus(bool onoff){

  if (onoff) client.publish(TOPIC_STATUS_STATE, VALUE_TOPIC_SWITCH_ON, true);
  if(!onoff) client.publish(TOPIC_STATUS_STATE, VALUE_TOPIC_SWITCH_OFF, true);

}


