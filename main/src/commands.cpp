#include "commands.hpp"
#include "Arduino.h"
#include "esp32-hal-timer.h"


State state;
void cancelTimer(CommandManager& manager) {
    manager.cancelScheduledCommand(AUTOSTOP_TAG);
}

void stopAll() {
    digitalWrite(RELAIS_DOWN, 1);
    digitalWrite(RELAIS_UP, 1);
}

void beginTimer(CommandManager& manager) {
    Command& cmd = manager.getRegisteredCommand(STOP_CMD_TAG);
    manager.scheduleCommand(AUTOSTOP_TAG, AUTOSTOP_DELAY, cmd);
}

BaseCommand::BaseCommand(WifiManager& wifiManager): wifiManager(wifiManager) {
}

void UpCommand::execute(CommandManager& manager) {
    cancelTimer(manager);
    digitalWrite(RELAIS_DOWN, 1);
    digitalWrite(RELAIS_UP, 0);
    beginTimer(manager);
    state.action = UP;
    if (state.state != UP) {
        this->wifiManager.sendState("middle");
    }
}

void DownCommand::execute(CommandManager& manager) {
    cancelTimer(manager);
    digitalWrite(RELAIS_DOWN, 0);
    digitalWrite(RELAIS_UP, 1);
    beginTimer(manager);
    state.action = DOWN;
    if (state.state != DOWN) {
        this->wifiManager.sendState("middle");
    }
}

void StandbyCommand::execute(CommandManager& manager) {
    cancelTimer(manager);
    stopAll();
}

/* Stop command is the same as the standby one, without cancelling the
timer. To be used with scheduled commands in order to stop relays
after some time */
void StopCommand::execute(CommandManager& manager) {
    if (state.action == UP) {
        this->wifiManager.sendState("up");
        state.state = UP;
    }
    else {
        this->wifiManager.sendState("down");
        state.state = DOWN;
    }

    stopAll();
    
}
