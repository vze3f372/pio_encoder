
#pragma once
#include "GpioPin.h"

class LED {
    public:
    LED(uint pin);

    void on();
    void off();
    void toggle();
    void value(bool v);
    bool value() const;

    LED(const LED&) = delete;
    LED& operator=(const LED&) = delete;

    private:
    GpioPin gpio_;
};
