#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>
#include "TFT_eSPI.h"
#include "image.hpp"
#include "command_manager.hpp"

class Button {
    private:
    Image& img;
    uint16_t x;
    uint16_t y;
    TFT_eSPI& tft;
    Command& command;


    public:
    Button(Image& img, uint16_t x, uint16_t y, TFT_eSPI& tft, Command& command);
    boolean isTouchInside(uint16_t x, uint16_t y);
    void maybeTrigger(uint16_t x, uint16_t y, CommandManager& manager);
    void display();
};
#endif