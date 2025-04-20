
#include "led.h"

LED::LED(uint pin) : gpio_(pin) {}

void LED::on() { gpio_.write(true); }

void LED::off() { gpio_.write(false); }

void LED::toggle() { gpio_.toggle(); }

void LED::value(bool v) { gpio_.write(v); }

bool LED::value() const { return gpio_.read(); }
