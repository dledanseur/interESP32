#include "button.hpp"

Button::Button(Image& img, uint16_t x, uint16_t y, TFT_eSPI& tft, Command& command):
        img(img), x(x), y(y), tft(tft), command(command) {
            Serial.printf("x,y:%i,%i\n", x, y);
}

boolean Button::isTouchInside(uint16_t x, uint16_t y) {
    uint16_t x1=this->x, y1=this->y, x2=x1+this->img.getW(), y2=y1+this->img.getH();

    //Serial.printf("Testing %i %i against x,y,x2,y2 %i %i% %i %i \n", x,y,x1,y1,x2,y2);
    boolean inside = (x>=x1 && x<=x2 &&y>=y1 && y<=y2);
    return inside;

}

void Button::maybeTrigger(uint16_t x, uint16_t y, CommandManager& manager) {
    if (isTouchInside(x,y)) {
        this->command.execute(manager);
    }
}

void Button::display() {
    this->img.display(this->tft, this->x, this->y);
}