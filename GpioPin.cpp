#include "GpioPin.h"

GpioPin::GpioPin(uint pin, bool output) : pin_(pin) {
    gpio_init(pin_);
    if (output) {
        gpio_set_dir(pin_, GPIO_OUT);
        gpio_put(pin_, false);
    } else {
        gpio_set_dir(pin_, GPIO_IN);
    }
}

void GpioPin::write(bool value) { gpio_put(pin_, value); }

bool GpioPin::read() const { return gpio_get(pin_); }

void GpioPin::toggle() { gpio_put(pin_, !gpio_get(pin_)); }

uint GpioPin::number() const { return pin_; }
