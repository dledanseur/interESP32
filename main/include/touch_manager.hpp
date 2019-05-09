#include <stdlib.h>
#include <map>

#ifndef TOUCHMANAGER_HPP
#define TOUCHMANAGER_HPP

#include "button.hpp"
#include "command_manager.hpp"

class Command;
class Button;

class ScreenOffCommand;

class TouchManager {
    private:
    std::vector<Button*> buttons;
    std::vector<Image*> images;
    std::map<uint, Command*> registeredCommands;
    ScreenOffCommand* screenOffCommand = NULL;
    TFT_eSPI& tft;
    CommandManager& command_manager;
    bool screenOn = true;

    void scheduleScreenOff();
    bool inScreen(int x, int y);

    public:
    TouchManager(TFT_eSPI& tft, CommandManager& command_manager);
    Image& createImage(uint8_t* data, uint16_t w, uint16_t h);
    Button& createButton(Image& img, uint16_t x, uint16_t y, Command &cmd);
    Button& createButton(uint8_t* data, uint16_t x, uint16_t y, uint16_t w, uint16_t h, Command& cmd);
    void registerCommand(uint tag, Command& cmd);

    
    void loop();
    ~TouchManager();

    friend class ScreenOffCommand;
};

class ScreenOffCommand:public Command {   
    private:
    TouchManager& touchManager;

    public: 
    ScreenOffCommand(TouchManager& manager);
    virtual void execute(CommandManager& manager);
};

#endif