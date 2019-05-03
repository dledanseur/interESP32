#include <Arduino.h>
#include <WiFi.h>
#include "command_manager.hpp"

class WifiManager {
    private:
    const char *mqtt_topic;
    const char *mqtt_host;
    int mqtt_port;
    static CommandManager* command_manager;

    public:
    WifiManager(CommandManager& command_manager);
    void connect(const char* ssid, const char* password,
                const char* mqtt_host, int mqtt_port,
                const char* mqtt_topic);
    static void callback(char* topic, byte* payload, unsigned int length);
    static void WiFiEvent(WiFiEvent_t event);
    void reconnect();
    void loop();
};