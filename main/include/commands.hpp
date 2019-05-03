#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "button.hpp"
#include "command_manager.hpp"

#define RELAIS_UP 32
#define RELAIS_DOWN 33
#define STOP_CMD_TAG 1
#define UP_CMD_TAG 2
#define DOWN_CMD_TAG 3
#define AUTOSTOP_TAG 1
// Time after which all commands will be canceled
#define AUTOSTOP_DELAY 15000

enum Action {
    UP,
    DOWN,
    STANDBY
};

class State {
    public:
    Action action;
};

class StopCommand : public Command {
    public:
    virtual void execute(CommandManager& manager);
};

class UpCommand : public Command {
    public:
    virtual void execute(CommandManager& manager);
};

class DownCommand : public Command {
    public:
    virtual void execute(CommandManager& manager);
};

class StandbyCommand : public Command {
    public:
    virtual void execute(CommandManager& manager);
};

#endif