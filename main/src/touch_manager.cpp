#include <stdlib.h>
#include "touch_manager.hpp"
#include <driver/dac.h>
#include <Arduino.h>

#define BLK_PIN 25
#define SCREEN_OFF_CMD_TAG 108
#define SCREEN_OFF_DELAY 30000

ScreenOffCommand::ScreenOffCommand(TouchManager& touchManager):touchManager(touchManager) {
}

void ScreenOffCommand::execute(CommandManager& manager) {
    digitalWrite(BLK_PIN, LOW);
    touchManager.screenOn = false;
}

TouchManager::TouchManager(TFT_eSPI& tft, CommandManager& command_manager):
    tft(tft), command_manager(command_manager) {
    pinMode(BLK_PIN, OUTPUT);
    this->screenOffCommand = new ScreenOffCommand(*this);
    //dac_output_enable(DAC_CHANNEL_1);
    //dac_output_voltage(DAC_CHANNEL_1, 255);
}

Image& TouchManager::createImage(uint8_t* data, uint16_t w, uint16_t h) {
    Image* img = new Image(data, w ,h);
    this->images.push_back(img);
    return *img;
}

Button& TouchManager::createButton(Image& img, uint16_t x, uint16_t y, Command &cmd) {
    Button* btn = new Button(img, x, y, this->tft, cmd);
    this->buttons.push_back(btn);
    btn->display();
    return *btn;
}

Button& TouchManager::createButton(uint8_t* data, uint16_t x, uint16_t y, uint16_t w, uint16_t h,Command& cmd) {
    Image& img = this->createImage(data, w ,h);
    return createButton(img, x, y, cmd);
}


void TouchManager::scheduleScreenOff() {
    this->command_manager.scheduleCommand(SCREEN_OFF_CMD_TAG, 
        SCREEN_OFF_DELAY, *this->screenOffCommand);
}

void TouchManager::loop() {
    uint16_t x;
    uint16_t y;
    boolean pressed = tft.getTouch(&x, &y, 20);
    if (pressed) {
        if (inScreen(x, y)) {
            this->command_manager.cancelScheduledCommand(SCREEN_OFF_CMD_TAG);
            digitalWrite(BLK_PIN, HIGH);
            scheduleScreenOff();
            if (screenOn) {
                for (auto b: this->buttons) {
                    b->maybeTrigger(x, y, this->command_manager);
                }
            }
            screenOn = true;
        }
    }
}

bool TouchManager::inScreen(int x, int y) {
    return (x>=0 && x<=TFT_WIDTH && y>=0 && y<=TFT_HEIGHT);
}

TouchManager::~TouchManager() {
    auto del = [](Image* img) { delete img; };
    
    for_each(this->images.begin(), this->images.end(), del);

    delete this->screenOffCommand;
    this->screenOffCommand = NULL;
}
