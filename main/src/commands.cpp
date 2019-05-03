#include "commands.hpp"
#include "Arduino.h"
#include "esp32-hal-timer.h"


State state;
void cancelTimer(CommandManager& manager) {
    manager.cancelScheduledCommand(AUTOSTOP_TAG);
    //esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
}

void stopAll() {
    //state.action = STANDBY;
    digitalWrite(RELAIS_DOWN, 1);
    digitalWrite(RELAIS_UP, 1);
    Serial.println("Written relays state");
    Serial.flush();
    //esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
}

void beginTimer(CommandManager& manager) {
    Command& cmd = manager.getRegisteredCommand(STOP_CMD_TAG);
    manager.scheduleCommand(AUTOSTOP_TAG, AUTOSTOP_DELAY, cmd);
    //esp_sleep_enable_timer_wakeup(AUTOSTOP_DELAY * 1000);
}

void UpCommand::execute(CommandManager& manager) {
    Serial.println("Up command");
    cancelTimer(manager);
    //state.action = UP;
    digitalWrite(RELAIS_DOWN, 1);
    digitalWrite(RELAIS_UP, 0);
    Serial.println("Written relays state");
    Serial.flush();
    beginTimer(manager);    
}

void DownCommand::execute(CommandManager& manager) {
    Serial.println("Down command");
    cancelTimer(manager);
    //state.action = DOWN;
    digitalWrite(RELAIS_DOWN, 0);
    digitalWrite(RELAIS_UP, 1);
    Serial.println("Written relays state");
    Serial.flush();
    beginTimer(manager);
}

void StandbyCommand::execute(CommandManager& manager) {
    Serial.println("Standby command");
    cancelTimer(manager);
    stopAll();
}

/* Stop command is the same as the standby one, without cancelling the
timer. To be used with scheduled commands in order to stop relais
after some time */
void StopCommand::execute(CommandManager& manager) {
    Serial.println("Stop command");
    stopAll();
}
