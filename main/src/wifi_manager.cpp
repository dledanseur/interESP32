#include "WiFiMulti.h"
#include <PubSubClient.h>
#include "esp_wifi.h"
#include "wifi_manager.hpp"
#include "commands.hpp"

//WiFiMulti WiFiMulti;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

CommandManager* WifiManager::command_manager = NULL;

WifiManager::WifiManager(CommandManager& cmd_manager) {
  WifiManager::command_manager = &cmd_manager;
}

void WifiManager::connect(const char* ssid, const char* password,
                         const char* mqtt_host, int mqtt_port, 
                         const char* mqtt_topic, const char* mqtt_state_topic) {
    // We start by connecting to a WiFi network
    WiFi.persistent(false);
    //WiFiMulti.addAP(ssid, password);
    esp_wifi_disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.printf("Connecting to %s\n",ssid);
    Serial.flush();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    
    wifi_config_t current_conf;
    esp_wifi_get_config(WIFI_IF_STA, &current_conf);
    current_conf.sta.listen_interval = 50;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &current_conf));
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    Serial.printf("Setting mqtt server and port to %s:%i\n", mqtt_host, mqtt_port);
    Serial.flush();
    client.setServer(mqtt_host, mqtt_port);

    client.setCallback(&callback);
    this->mqtt_state_topic = mqtt_state_topic;
    this->mqtt_topic = mqtt_topic;
    this->mqtt_host = mqtt_host;
    this->mqtt_port = mqtt_port;
    WiFi.onEvent(WiFiEvent);
}

void WifiManager::WiFiEvent(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection.  Attempting to reconnect...");
    WiFi.reconnect();
    break;
  case SYSTEM_EVENT_STA_START:
    Serial.println("ESP32 station start");
    break;
  case SYSTEM_EVENT_STA_CONNECTED:
    Serial.println("ESP32 station connected to AP");
    break;
  default:			
    Serial.println("Unhandled WiFi Event raised.");
    break;
  }

}

void WifiManager::reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("espClient")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void WifiManager::callback(char* topic, byte* payload, unsigned int length) {
  std::string s(reinterpret_cast<const char *>(payload), length);

  if (s == "up") {
    command_manager->triggerCommand(UP_CMD_TAG);
  }
  else if (s == "down") {
    command_manager->triggerCommand(DOWN_CMD_TAG);
  }
  else if (s == "stop") {
    command_manager->triggerCommand(STOP_CMD_TAG);
  }
  else {
    Serial.printf("Unkown command %s \n", s.c_str());
  }
}

void WifiManager::loop() {
  //esp_wifi_start()
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    /*esp_wifi_start();*/
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  }
  if (!client.connected()) {
      reconnect();
  }
  client.loop();
  delay(10);
  //esp_wifi_stop();
}

void WifiManager::sendState(const char* state) {
  uint8_t* bytes = (uint8_t*) state;

  client.publish(this->mqtt_state_topic, bytes, strlen(state), true);
} 

