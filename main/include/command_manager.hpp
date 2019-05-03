#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include <Arduino.h>
#include <stdlib.h>
#include <map>

class CommandManager;

class Command {
    public:
    virtual void execute(CommandManager& manager);
};

class TimedCommand {
    public:
    TimedCommand(unsigned long when, Command& cmd):when(when), cmd(cmd) {};
    unsigned long when;
    Command& cmd;
};

class CommandManager {
    private:
    std::map<uint,Command*> registeredCommands;
    std::map<uint, TimedCommand*> scheduledCommands;

    public:
    void registerCommand(uint tag, Command& cmd);
    void triggerCommand(uint tag);
    void scheduleCommand(uint tag, unsigned long in, Command& cmd);
    void cancelScheduledCommand(uint tag);
    Command& getRegisteredCommand(uint tag);
    void loop();
};
#endif