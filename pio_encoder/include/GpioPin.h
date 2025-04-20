#pragma once
#include "pico/stdlib.h"

class GpioPin {
    public:
    GpioPin(uint pin, bool output = true);

    void write(bool value);
    bool read() const;
    void toggle();
    uint number() const;

    GpioPin(const GpioPin&) = delete;
    GpioPin& operator=(const GpioPin&) = delete;

    private:
    uint pin_;
};
