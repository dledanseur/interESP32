#include <stdlib.h>

#include "touch_manager.hpp"

TouchManager::TouchManager(TFT_eSPI& tft, CommandManager& command_manager):
    tft(tft), command_manager(command_manager) {
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

void TouchManager::loop() {
    uint16_t x;
    uint16_t y;
    boolean pressed = tft.getTouch(&x, &y, 20);
    if (pressed) {
        for (auto b: this->buttons) {
            b->maybeTrigger(x, y, this->command_manager);
        }
    }
}

TouchManager::~TouchManager() {
    auto del = [](Image* img) { delete img; };
    
    for_each(this->images.begin(), this->images.end(), del);
}
