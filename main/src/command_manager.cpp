#include "command_manager.hpp"

void CommandManager::registerCommand(uint tag, Command& cmd) {
    this->registeredCommands[tag] = &cmd;
}

void CommandManager::triggerCommand(uint tag) {
    this->registeredCommands[tag]->execute(*this);
}

Command& CommandManager::getRegisteredCommand(uint tag) {
    Serial.printf("Asking for command %i\n", tag);
    Serial.flush();

    Command* cmd = registeredCommands[tag];
    Serial.printf("%p\n", cmd);
    Serial.flush();

    return *cmd;
}

void CommandManager::scheduleCommand(uint tag, unsigned long in, Command& cmd) {
    TimedCommand* tcmd = new TimedCommand(millis()+in, cmd);
    Serial.printf("Scheduling task %p\n", tcmd);
    Serial.flush();
    scheduledCommands[tag] = tcmd;
}

void CommandManager::cancelScheduledCommand(uint tag) {
    if (scheduledCommands.count(tag)) {
        TimedCommand* tcmd = scheduledCommands[tag];
        Serial.printf("Cancelling task %p\n", tcmd);
        Serial.flush();
        scheduledCommands.erase(tag);
        delete tcmd;
        Serial.printf("Done cancelling task %p\n", tcmd);
        Serial.flush();
    }
}

void CommandManager::loop() {
    auto it = scheduledCommands.begin();
    unsigned long now = millis();

    while (it != scheduledCommands.end()) {
        TimedCommand* tcmd = it->second;

        if (tcmd->when < now) {
            tcmd->cmd.execute(*this);
            Serial.printf("Cancelling task %p\n", tcmd);
            Serial.flush();
            it = scheduledCommands.erase(it);
            delete tcmd;
        }
        else {
            it++;
        }
    }
}

Command::~Command() {

}